#include <SoftwareSerial.h>

SoftwareSerial myserial(10,11);

void setup()
{
    myserial.begin(57600);
}

void flushRx()
{
    while(myserial.available())
        uint8_t tmp = myserial.read();
}

void loop()
{
    char myString [] = "Computer Engineering";
    for (int i=0; myString[i] != '\0'; i++)
    {
        myserial.write(myString[i]);
        delay(10);
        flushRx();
    }
    delay(500);
    myserial.write('\n');
}