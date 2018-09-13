#include <SoftwareSerial.h>

SoftwareSerial BTSerial(8, 7);  // RX, TX
long previousMillis = 0;
long interval = 5000;

void setup() {
  Serial.begin(9600);
  Serial.println("Welcome! This is Pasarica v0.1a!");
  Serial.println("This device monitors the temperature of the hot water pipe");
  Serial.println("and sends the value to Home Assistant.");
  Serial.println("----------------------------------------------------------");
  Serial.println("Why \"Pasarica\"? Because \"O pasarica mi-a zis ca apa e gata de dus!\"");
  Serial.println("");
  BTSerial.begin(9600);
  Serial.println("Configuring Bluetooth LE module...");
  BTSerial.write("AT+DEFAULT\r\n");
  BTSerial.write("AT+RESET\r\n");
  BTSerial.write("AT+NAME=Pasarica\r\n");
  BTSerial.write("AT+ROLE1\r\n");
  BTSerial.write("AT+TYPE\r\n");
  Serial.println("Done, starting the loop!");
}

void loop()
{
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    float temp = analogRead(1) * 0.48828125;
    String jsonStr = String("{ \"temp\": ");
    jsonStr += String(temp, 2);
    jsonStr += String(" }\r\n");
    char charBuffer[jsonStr.length() + 1];
    jsonStr.toCharArray(charBuffer, jsonStr.length());
    BTSerial.write(charBuffer);
    //Serial.print(jsonStr);
  }
  if (BTSerial.available())
    Serial.write(BTSerial.read());
  if (Serial.available())
    BTSerial.write(Serial.read());
}
