#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <SmoothADC.h>

SmoothADC adc;
SoftwareSerial BTSerial(8, 7);  // RX, TX
const size_t jsonBufferSize = JSON_OBJECT_SIZE(1);
DynamicJsonDocument doc;

unsigned long previousMillis = 0; 
const long interval = 500;
double lastTemp = 0.0;

void setup() {
  Serial.begin(9600);
  Serial.println("Welcome! This is Pasarica v0.2a!");
  Serial.println("This device monitors the temperature of the hot water pipe");
  Serial.println("and sends the value to Home Assistant.");
  Serial.println("----------------------------------------------------------");
  Serial.println("Why \"Pasarica\"? Because \"O pasarica mi-a zis ca apa e gata de dus!\"");
  Serial.println("");
  BTSerial.begin(9600);
  adc.init(1, TB_US, 500);
  if (adc.isDisabled()) { 
    adc.enable(); 
  }
  Serial.println("Configuring Bluetooth LE module...");
  BTSerial.write("AT+RESET\r\n");
  BTSerial.write("AT+NAME=Pasarica\r\n");
  Serial.println("Done, starting the loop!");
}

void loop()
{
  adc.serviceADCPin();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    double temp = round((adc.getADCVal() * 0.48828125)* 100) / 100.0;
    if (fabs(lastTemp-temp) > 1.0) {
      JsonObject root = doc.to<JsonObject>();
      root["temp"] = temp;
      serializeJson(doc, BTSerial);
      lastTemp = temp;
      //serializeJson(doc, Serial); Serial.println();
    }
  }
  
  if (BTSerial.available()) {
    String message = BTSerial.readStringUntil('\n');
    if (message == "HELLO\r") {
      // Client just connected, force resending of temperature
      lastTemp = 0.0;
    }
  }

//  if (BTSerial.available())
//    Serial.write(BTSerial.read());
//  if (Serial.available())
//    BTSerial.write(Serial.read());
}
