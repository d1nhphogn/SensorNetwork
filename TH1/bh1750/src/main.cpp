#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>

#define SDA_PIN 21  // Chân SDA của ESP32
#define SCL_PIN 22  // Chân SCL của ESP32

BH1750 lightMeter; // Khai báo đối tượng BH1750

void setup() {
    Serial.begin(9600);
    Wire.begin(SDA_PIN, SCL_PIN); // Khởi tạo I2C với SDA & SCL tùy chỉnh
    if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
        Serial.println("Không tìm thấy cảm biến BH1750!");
        while (1);
    }
    Serial.println("Cảm biến BH1750 sẵn sàng!");
}

void loop() {
    float lux = lightMeter.readLightLevel(); // Đọc giá trị ánh sáng (lux)

    if (lux < 0) {
        Serial.println("Lỗi! Không thể đọc dữ liệu từ BH1750");
    } else {
        Serial.print("Cường độ ánh sáng: ");
        Serial.print(lux);
        Serial.println(" lx");
    }

    delay(1000); // Cập nhật mỗi 1 giây
}