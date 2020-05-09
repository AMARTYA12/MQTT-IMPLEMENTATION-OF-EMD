// i2c scan
#include "SoftwareI2C.h"

SoftwareI2C Wire;

void setup()
{
    Serial.begin(115200);
    Wire.begin(A4, A5);       // sda, scl
    Serial.println("begn to scan...");
}

void loop()
{
    for(unsigned char i=1; i<=127; i++)
    {
        if(Wire.beginTransmission(i))
        {
            Serial.print("0x");
            Serial.println(i, HEX);
            
            while(1);
        }
        Wire.endTransmission();
    }
    
    Serial.println("find nothing");
    while(1);
    
}