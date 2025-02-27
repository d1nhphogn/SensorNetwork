
   #include <Arduino.h>
   #include <Adafruit_Sensor.h>
   #include <DHT.h>
   #include <DHT_U.h>
   
   #define DHTPIN 4       // Chân kết nối cảm biến DHT11
   #define DHTTYPE DHT11  // Loại cảm biến
   
   DHT dht(DHTPIN, DHTTYPE);
   
   void setup() {
       Serial.begin(9600);  // Khởi động Serial Monitor với baudrate 115200
       Serial.println("Khởi động cảm biến DHT11...");
       dht.begin();
   }
   
   void loop() {
       float temperature = dht.readTemperature();
       float humidity = dht.readHumidity();
   
       if (isnan(temperature) || isnan(humidity)) {
           Serial.println("Lỗi đọc cảm biến!");
       } else {
           Serial.print("Nhiệt độ: ");
           Serial.print(temperature);
           Serial.print(" °C  |  Độ ẩm: ");
           Serial.print(humidity);
           Serial.println(" %");
       }
   
       delay(5000);  // Cập nhật dữ liệu mỗi 2 giây
   }
   /*
#include <Arduino.h>

#include "DHT.h"

#define DHTPIN 4 
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
    Serial.begin(9600);
    Serial.println(F("DHTxx test!"));

    dht.begin();
}

void loop()
{
    delay(5000);

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f))
    {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }
    float hif = dht.computeHeatIndex(f, h);
    float hic = dht.computeHeatIndex(t, h, false);
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(f);
    Serial.print(F("°F  Heat index: "));
    Serial.print(hic);
    Serial.print(F("°C "));
    Serial.print(hif);
    Serial.println(F("°F"));
}
    */