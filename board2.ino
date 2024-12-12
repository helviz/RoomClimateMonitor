#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <avr/io.h>
#include <avr/interrupt.h>


LiquidCrystal_I2C lcd(0x27, 16, 2); /


float temperature = 0;
float humidity = 0;
float comfortIndex = 0;
volatile uint8_t mode = 0; 

void setup() {
    Serial.begin(9600);        
    Wire.begin();              
    lcd.begin(16, 2);          
    lcd.backlight();           

    // Retrieve last mode from EEPROM
    EEPROM.get(0, mode);
    if (mode > 2) mode = 0;    

    // Timer1 Configuration
    cli(); 
    TCCR1A = 0;                        // Normal mode
    TCCR1B = (1 << WGM12) | (1 << CS12); // prescaler = 256
    OCR1A = 15625;                     // Compare value for 1 Hz interrupt 
    TIMSK1 |= (1 << OCIE1A);           
    sei(); 

    Serial.println("LCD Board Initialized");
}

void loop() {
    
}

ISR(TIMER1_COMPA_vect) {
    // Request mode from Board 3 (Slave at address 3)
    Wire.requestFrom(3, 1);
    if (Wire.available()) {
        mode = Wire.read();
        Serial.println("Mode received from Board 3: " + String(mode));

        // Save mode to EEPROM for persistence
        EEPROM.put(0, mode);
    } else {
        Serial.println("Failed to receive mode");
        return;
    }

    // Request data from Board 1 (Slave at address 8)
    Wire.requestFrom(8, 8); // Request 8 bytes from Board 1
    if (Wire.available() == 8) { // Ensure we receive the full data
        uint8_t data[8];
        for (int i = 0; i < 8; i++) {
            data[i] = Wire.read();
        }

        // Unpack the data into temperature and humidity
        memcpy(&temperature, data, 4); // Extract 4 bytes for temperature
        memcpy(&humidity, data + 4, 4); // Extract 4 bytes for humidity

        Serial.print("Received Temp: ");
        Serial.print(temperature);
        Serial.print(" C, Hum: ");
        Serial.print(humidity);
        Serial.println(" %");
    } else {
        Serial.println("Failed to receive data from Board 1");
        return;
    }

    // Update LCD based on the mode
    lcd.clear();
    if (mode == 0) {
        lcd.setCursor(0, 0);
        lcd.print("Temp: " + String(temperature, 1) + "C");

        
    } else if (mode == 1) {
        lcd.setCursor(0, 0);
        lcd.print("Hum: " + String(humidity, 1) + "%");


    } else if (mode == 2) {
        // Send data to Board 3 for comfort index calculation
        Wire.beginTransmission(3); // Start transmission to Board 3
        Wire.write((uint8_t*)&temperature, 4); // Send temperature 
        Wire.write((uint8_t*)&humidity, 4);    // Send humidity 
        Wire.endTransmission(); // End transmission

        delay(50); 

        // Request the calculated comfort index
        Wire.requestFrom(3, 4); // Request 4 bytes for comfort index
        if (Wire.available() == 4) {
            uint8_t indexData[4];
            for (int i = 0; i < 4; i++) {
                indexData[i] = Wire.read();
            }
            memcpy(&comfortIndex, indexData, 4); // Extract the float value

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
}
