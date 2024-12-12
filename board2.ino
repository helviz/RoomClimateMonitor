#include <Wire.h>
#include <LiquidCrystal_I2C.h>


#define SET_BIT(port, bit) ((port) |= (1 << (bit)))
#define CLEAR_BIT(port, bit) ((port) &= ~(1 << (bit)))
#define TOGGLE_BIT(port, bit) ((port) ^= (1 << (bit)))
#define READ_BIT(port, bit) (((port) >> (bit)) & 1)


LiquidCrystal_I2C lcd(0x27, 16, 2); /


float temperature = 0;
float humidity = 0;
float comfortIndex = 0;
int mode = 0; 

void setup() {
   
    Serial.begin(9600);

    
    Wire.begin();

    
    lcd.begin(16, 2);
    lcd.backlight(); 
    Serial.println("Board 2: Initialized");
}

void loop() {
    // Request mode from Board 3 (Slave at address 3)
    Wire.requestFrom(3, 1); // Request 1 byte from Board 3
    if (Wire.available()) {

        // Read the mode value
        mode = Wire.read(); 
        Serial.println("Mode received from Board 3: " + String(mode)); 
    } else {
        Serial.println("Failed to receive mode");
        return;
    }

    // Request data from Board 1 (Slave at address 8)
    Wire.requestFrom(8, 8); 
    if (Wire.available() == 8) { \
        uint8_t data[8];
        for (int i = 0; i < 8; i++) {
            data[i] = Wire.read();
        }

        // Unpack the data into temperature and humidity
        memcpy(&temperature, data, 4); 
        memcpy(&humidity, data + 4, 4); 

        //Log received data
        Serial.print("Received Temp: ");
        Serial.print(temperature);
        Serial.print(" C, Hum: ");
        Serial.print(humidity);
        Serial.println(" %");
    } else {
        Serial.println("Failed to receive data from Board 1");
        return;
    }

    // Handle mode and display on LCD
    lcd.clear();
    if (mode == 0) {
        lcd.setCursor(0, 0);
        lcd.print("Temp: " + String(temperature, 1) + "C");
    } else if (mode == 1) {
        lcd.setCursor(0, 0);
        lcd.print("Hum: " + String(humidity, 1) + "%");
    } else if (mode == 2) {
        // Send data(temp, humidity) to Board 3 for comfort index calculation
        Wire.beginTransmission(3); 
        Wire.write((uint8_t*)&temperature, 4); 
        Wire.write((uint8_t*)&humidity, 4);    
        Wire.endTransmission(); 

        // Give Board 3 time to calculate and respond
        delay(50);


        // Request the calculated comfort index
        Wire.requestFrom(3, 4); \
        if (Wire.available() == 4) {
            uint8_t indexData[4];
            for (int i = 0; i < 4; i++) {
                indexData[i] = Wire.read();
            }

            // Extract the float value
            memcpy(&comfortIndex, indexData, 4); 

            lcd.setCursor(0, 0);
            lcd.print("Comfort Index:");
            lcd.setCursor(0, 1);
            lcd.print(String(comfortIndex, 1));
        } else {
            lcd.setCursor(0, 0);
            lcd.print("Calc Error");
            Serial.println("Failed to receive comfort index from Board 3");
        }
    }

    delay(1000); 
}
