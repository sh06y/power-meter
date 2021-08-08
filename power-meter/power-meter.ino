#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#define powerledPin A0
const int ledPin = 13;
const int buzzerPin = 12;
const int lcdPin = 2;
const int sensor_VccPin = 7; // 两个光敏传感器

const int powerSupply_testPin = 5; // 供电情况

// x imp/kWh
const int powerledPin_rate = 800;

// 最大功率 (w)
int maxPower = 7000;


const int beep_delay = 400;


int light1;

int powerledPin_on, powerledPin_off, powerledPin_average;

unsigned long timeA;
unsigned long timeB;
unsigned long time_light;

unsigned long beepP = 0;

float power = 0;
bool beep = false;

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27  0x3F for a 16 chars and 2 line display


void setup(){
	// LCD屏幕初始化
	pinMode(lcdPin, OUTPUT);
	digitalWrite(lcdPin, HIGH);
	lcd.init();  //initialize the lcd
	lcd.backlight();  //open the backlight

	Serial.begin(9600);
	pinMode(buzzerPin, OUTPUT);
	digitalWrite(buzzerPin, LOW);
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, LOW);
	pinMode(sensor_VccPin, OUTPUT);
	digitalWrite(sensor_VccPin, HIGH);

	
	// 重置eeprom
	int powerSupply_status = analogRead(A6);
	if(powerSupply_status != 1023){
		EEPROM.write(1,0);

		lcd.print("eeprom clear!");
		digitalWrite(buzzerPin, HIGH);
		delay(1000);
		digitalWrite(buzzerPin, LOW);
		lcd.clear();
	}


	// 上电读取eeprom中存储的功率数据
	int eepromData = EEPROM.read(1) * 39;
	lcd.print(eepromData);
	if(eepromData != 0){
		// 如果有数据
		maxPower = eepromData;
	}
	
	delay(2000);
	lcd.clear();

	digitalWrite(ledPin, HIGH);
	lcd.print("setting...");
	int light_a, light_b;
	while(1){
		light_a = analogRead(powerledPin);
		delay(50);
		light_b = analogRead(powerledPin);
		if(light_b - light_a >= 50){
			powerledPin_on = light_b;
			powerledPin_off = light_a;
			powerledPin_average = (powerledPin_on + powerledPin_off) / 2;

			Serial.print("powerledPin_on is ");
			Serial.println(powerledPin_on);

			lcd.setCursor(0,1);
			lcd.print("success");
			delay(1000);
			lcd.clear();

			lcd.setCursor(0,0);
			lcd.print("on:");
			lcd.print(powerledPin_on);
			lcd.setCursor(0,1);
			lcd.print("off:");
			lcd.print(powerledPin_off);
			
			for(int i = 1; i <= 5; i++){
				digitalWrite(ledPin, LOW);
				delay(100);
				digitalWrite(ledPin, HIGH);
				delay(100);
			}
			
			delay(1000);
			lcd.clear();

			break;
		}

	}
	

}


void powerSupply_test(){
	int powerSupply_status = analogRead(A6);
	// Serial.println(powerSupply_status);
	if(powerSupply_status != 1023){
		digitalWrite(sensor_VccPin, LOW);
		digitalWrite(buzzerPin, LOW);
		digitalWrite(ledPin, LOW);
		digitalWrite(lcdPin, LOW);

		int tmp = power / 39;
		EEPROM.write(1, tmp);
		delay(5000);


	}
	
}


void loop(){
	while(1){
		powerSupply_test();

		light1 = analogRead(A0);
		if(light1 > powerledPin_average){
			// led on
			timeA = micros();
			delay(300);
			break;
		}
	}
	
	// 计算两次亮灯之间的间隙
	time_light = timeA - timeB;

	power = 1. / powerledPin_rate * 1000 * 3600 * 1000000 / time_light;

	Serial.println(power);

	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print(power);

	if(power >= maxPower){
		if(!beep){
			if(millis() - beepP >= beep_delay){
				beep = true;
				digitalWrite(buzzerPin, HIGH);
				beepP = millis();
			}
		}else{
			if(millis() - beepP >= beep_delay){
				beep = false;
				digitalWrite(buzzerPin, LOW);
			}
		}
		
	}else{
		beep = false;
		digitalWrite(buzzerPin, LOW);
	}
	// Serial.println(beep);


	while(1){
		powerSupply_test();

		light1 = analogRead(A0);
		if(light1 > powerledPin_average){
			// led on
			timeB = micros();
			delay(300);
			break;
		}
	}
	
	// 计算两次亮灯之间的间隙
	time_light = timeB - timeA;

	power = 1. / powerledPin_rate * 1000 * 3600 * 1000 * 1000 / time_light;

	Serial.println(power);

	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print(power);

	if(power >= maxPower){
		if(!beep){
			if(millis() - beepP >= beep_delay){
				beep = true;
				digitalWrite(buzzerPin, HIGH);
				beepP = millis();
			}
		}else{
			if(millis() - beepP >= beep_delay){
				beep = false;
				digitalWrite(buzzerPin, LOW);
			}
		}
		
	}else{
		beep = false;
		digitalWrite(buzzerPin, LOW);
	}
	// Serial.println(beep);
}
