#include <Wire.h>

float calculateComfortIndex(float temp, float hum) {
  return temp - ((100 - hum) / 5); // Simplified formula
}

void setup() {
  Serial.begin(9600);
  Wire.begin(3); // Set I2C address to 3
  Wire.onRequest(sendComfortIndex); // Register request handler
  Wire.onReceive(receiveData); // Register data handler
  Serial.println("Board 3: Initialized");
}

float temperature = 0;
float humidity = 0;
float comfortIndex = 0;

void receiveData(int bytes) {
  if (bytes == 8) { // Expecting 8 bytes: 4 for temp, 4 for hum
    uint8_t data[8];
    for (int i = 0; i < 8; i++) {
      data[i] = Wire.read();
    }
    memcpy(&temperature, data, 4); // Extract 4 bytes for temperature
    memcpy(&humidity, data + 4, 4); // Extract 4 bytes for humidity

    comfortIndex = calculateComfortIndex(temperature, humidity);

    Serial.print("Received Temp: ");
    Serial.print(temperature);
    Serial.print(", Hum: ");
    Serial.print(humidity);
    Serial.print(", Calculated Comfort Index: ");
    Serial.println(comfortIndex);
  }
}

void sendComfortIndex() {
  Wire.write((uint8_t*)&comfortIndex, 4); // Send comfort index as 4 bytes
}
