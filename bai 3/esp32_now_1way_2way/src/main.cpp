/*
  * ESP-NOW DHT11 Sender (ESP32) - Gửi dữ liệu DHT11 đến ESP-NOW Receiver (ESP32)
  
#include <WiFi.h>
#include <esp_now.h>
#include <DHT.h>

// --- Cấu hình DHT11 ---
#define DHTPIN    4       // Chân DATA DHT11
#define DHTTYPE   DHT11
DHT dht(DHTPIN, DHTTYPE);

// --- MAC của Node Slave (ESP32 sẽ gửi đến) ---
uint8_t slaveMAC[] = {0x3C, 0x8A, 0x1F, 0x5E, 0x29, 0x88};

// --- Struct chứa dữ liệu DHT11 ---
typedef struct {
  float temperature;
  float humidity;
} SensorData;

SensorData dataToSend;

// Callback báo trạng thái gửi
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status to ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", mac_addr[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.print(" -> ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Failed");
}

void setup() {
  Serial.begin(9600);    // <-- chuyển xuống 9600 bps
  dht.begin();

  // Khởi ESP‑NOW
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    while (1) { delay(1000); }
  }

  // Đăng ký callback send
  esp_now_register_send_cb(onDataSent);

  // Thêm peer (Slave)
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, slaveMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    while (1) { delay(1000); }
  }
}

void loop() {
  // Đọc DHT11
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT11!");
  } else {
    dataToSend.temperature = t;
    dataToSend.humidity    = h;
    // Gửi struct
    esp_err_t res = esp_now_send(slaveMAC, (uint8_t *)&dataToSend, sizeof(dataToSend));
    if (res != ESP_OK) {
      Serial.println("Error sending DHT data");
    }
    Serial.printf("Sent DHT -> T:%.2f°C  H:%.2f%%\n", t, h);
  }
  delay(2000);
}
  */ 

  #include <WiFi.h>
#include <esp_now.h>
#include <DHT.h>

#define DHTPIN    4
#define DHTTYPE   DHT11
DHT dht(DHTPIN, DHTTYPE);

// Thay bằng MAC của Slave (in ra ở Serial Monitor của Slave)
uint8_t peerMAC[] = {0x3C, 0x8A, 0x1F, 0x5E, 0x29, 0x88};

typedef struct {
  uint8_t type;
  float   temperature;
  float   humidity;
  float   lux;
} DataPacket;

DataPacket packet;

void onDataSent(const uint8_t *mac, esp_now_send_status_t status) {
  Serial.print("Sent to ");
  for (int i=0; i<6; i++){
    Serial.printf("%02X", mac[i]);
    if (i<5) Serial.print(":");
  }
  Serial.print(" -> ");
  Serial.println(status==ESP_NOW_SEND_SUCCESS?"Success":"Fail");
}

void onDataRecv(const uint8_t *mac, const uint8_t *buf, int len) {
  DataPacket pkt;
  memcpy(&pkt, buf, sizeof(pkt));
  Serial.print("Recv from ");
  for (int i=0; i<6; i++){
    Serial.printf("%02X", mac[i]);
    if (i<5) Serial.print(":");
  }
  if (pkt.type==2) {
    Serial.printf(" -> Lux: %.2f lx\n", pkt.lux);
  }
}

void setup(){
  Serial.begin(9600);
  Serial.println("=== Master ===");
  Serial.print("Local MAC: ");
  Serial.println(WiFi.macAddress());

  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init()!=ESP_OK) { Serial.println("ESP-NOW init failed"); while(1); }
  esp_now_register_send_cb(onDataSent);
  esp_now_register_recv_cb(onDataRecv);

  esp_now_peer_info_t pi = {};
  memcpy(pi.peer_addr, peerMAC, 6);
  pi.channel = 0; pi.encrypt = false;
  if (esp_now_add_peer(&pi)!=ESP_OK) { Serial.println("Add peer failed"); while(1); }
}

void loop(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (!isnan(h) && !isnan(t)){
    packet.type        = 1;
    packet.temperature = t;
    packet.humidity    = h;
    packet.lux         = 0;
    esp_err_t r = esp_now_send(peerMAC, (uint8_t*)&packet, sizeof(packet));
    if (r!=ESP_OK) Serial.println("Send error");
    else Serial.printf("Sent DHT -> T:%.2f°C H:%.2f%%\n", t, h);
  } else {
    Serial.println("DHT read failed");
  }
  delay(1000);
}