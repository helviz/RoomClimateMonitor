#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD at I2C address 0x27

float temperature = 0;
float humidity = 0;
float comfortIndex = 0;
int mode = 0; // 0: Temp, 1: Hum, 2: Comfort Index

void setup() {
  Serial.begin(9600);  // Debugging
  Wire.begin();        // Initialize I2C as master
  lcd.begin(16, 2);    // Initialize LCD
  lcd.backlight();     // Turn on the LCD backlight

  Serial.println("Board 2: Initialized");
}

void loop() {
  // Request mode from Board 3 (Slave at address 3)
  Wire.requestFrom(3, 1); // Request 1 byte from Board 3
  if (Wire.available()) {
    mode = Wire.read(); // Read the mode value
    Serial.println("Mode received from Board 3: " + String(mode)); // Debugging
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

    // Debugging: Log received data
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
    // Send data to Board 3 for comfort index calculation
    Wire.beginTransmission(3); // Start transmission to Board 3
    Wire.write((uint8_t*)&temperature, 4); // Send temperature (4 bytes)
    Wire.write((uint8_t*)&humidity, 4);    // Send humidity (4 bytes)
    Wire.endTransmission(); // End transmission

    delay(50); // Give Board 3 time to calculate and respond

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

  delay(1000); // Update every second
}
