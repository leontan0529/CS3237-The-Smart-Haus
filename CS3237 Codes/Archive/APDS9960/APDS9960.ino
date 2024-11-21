uint8_t address = 0x39; 
uint8_t MSByte = 0, LSByte = 0; 
uint16_t redValue = 0, greenValue = 0, blueValue = 0, clearValue = 0;

#include <Wire.h> 

void setup() { 
  Wire.begin(); 
  Serial.begin(9600); 
  Wire.beginTransmission(address); 
  Wire.write(0x80); 
  Wire.write(0x03);  
  Wire.endTransmission(); 

  delay(500); 
} 

void loop() {   
    // Read Red value
    Wire.beginTransmission(address); 
    Wire.write(0x96);  // Red Low Byte
    Wire.endTransmission(); 
    Wire.requestFrom(address, 1); 
    if(Wire.available()){ 
          LSByte = Wire.read(); 
    } 
    Wire.beginTransmission(address); 
    Wire.write(0x97);  // Red High Byte
    Wire.endTransmission(); 
    Wire.requestFrom(address, 1); 
    if(Wire.available()){ 
          MSByte = Wire.read(); 
    } 
    redValue = (MSByte << 8) + LSByte;

    // Read Green value
    Wire.beginTransmission(address); 
    Wire.write(0x98);  // Green Low Byte
    Wire.endTransmission(); 
    Wire.requestFrom(address, 1); 
    if(Wire.available()){ 
          LSByte = Wire.read(); 
    } 
    Wire.beginTransmission(address); 
    Wire.write(0x99);  // Green High Byte
    Wire.endTransmission(); 
    Wire.requestFrom(address, 1); 
    if(Wire.available()){ 
          MSByte = Wire.read(); 
    } 
    greenValue = (MSByte << 8) + LSByte;

    // Read Blue value
    Wire.beginTransmission(address); 
    Wire.write(0x9A);  // Blue Low Byte
    Wire.endTransmission(); 
    Wire.requestFrom(address, 1); 
    if(Wire.available()){ 
          LSByte = Wire.read(); 
    } 
    Wire.beginTransmission(address); 
    Wire.write(0x9B);  // Blue High Byte
    Wire.endTransmission(); 
    Wire.requestFrom(address, 1); 
    if(Wire.available()){ 
          MSByte = Wire.read(); 
    } 
    blueValue = (MSByte << 8) + LSByte;

    // Read Clear value
    Wire.beginTransmission(address); 
    Wire.write(0x94);  // Blue Low Byte
    Wire.endTransmission(); 
    Wire.requestFrom(address, 1); 
    if(Wire.available()){ 
          LSByte = Wire.read(); 
    } 
    Wire.beginTransmission(address); 
    Wire.write(0x95);  // Blue High Byte
    Wire.endTransmission(); 
    Wire.requestFrom(address, 1); 
    if(Wire.available()){ 
          MSByte = Wire.read(); 
    } 
    clearValue = (MSByte << 8) + LSByte;

    // Print the values
    Serial.print("Red Value: "); 
    Serial.print(redValue);
    Serial.print("  Green Value: ");
    Serial.print(greenValue);
    Serial.print("  Blue Value: ");
    Serial.print(blueValue);
    Serial.print("  Clear Value: ");
    Serial.println(clearValue);

    // Wait 1 second before next reading
    delay(1000); 
}
