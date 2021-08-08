#include <EEPROM.h>

void loop(){
	EEPROM.write(1,0);
	delay(100000);
}