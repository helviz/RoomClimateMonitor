#include <Wire.h>
#include <EEPROM.h>

float temperature = 0;
float humidity = 0;
float comfortIndex = 0;

float calculateComfortIndex(float temp, float hum) {
    return temp - ((100 - hum) / 5); 
}

void setup() {
    Serial.begin(9600);
    Wire.begin(3); 
    Wire.onRequest(sendComfortIndex);
    Wire.onReceive(receiveData);

    // Retrieve the last comfort index from EEPROM
    EEPROM.get(0, comfortIndex);
    Serial.println("Board 3: Initialized");
}

void loop() {
    
}

void receiveData(int bytes) {
    if (bytes == 8) {
        uint8_t data[8];
        for (int i = 0; i < 8; i++) {
            data[i] = Wire.read();
        }

        memcpy(&temperature, data, 4);
        memcpy(&humidity, data + 4, 4);

        comfortIndex = calculateComfortIndex(temperature, humidity);

        // Save the calculated comfort index to EEPROM
        EEPROM.put(0, comfortIndex);

        Serial.print("Temp: ");
        Serial.print(temperature);
        Serial.print(" C, Hum: ");
        Serial.print(humidity);
        Serial.print(" %, Comfort Index: ");
        Serial.println(comfortIndex);
    }
}

void sendComfortIndex() {
    Wire.write((uint8_t*)&comfortIndex, 4); // Send comfort index
}
