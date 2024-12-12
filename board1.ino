#include <Wire.h>
#include <EEPROM.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <DHT.h>

#define DHTPIN 2     
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


volatile bool updateReady = false; 
float temperature = 0.0;
float humidity = 0.0;

void setup() {
    Serial.begin(9600);    
    Wire.begin(8);         
    Wire.onRequest(sendSensorData); 
    dht.begin();         

    // Retrieve saved data from EEPROM
    EEPROM.get(0, temperature);
    EEPROM.get(sizeof(temperature), humidity);

    // Timer1 Configuration
    cli(); 
    TCCR1A = 0;                        // Normal mode
    TCCR1B = (1 << WGM12) | (1 << CS12); // prescaler = 256
    OCR1A = 31250;                     // Compare value for ~2 seconds 
    TIMSK1 |= (1 << OCIE1A);           // Enable Timer1 compare interrupt
    sei(); 

    Serial.println("Board 1: Initialized");
}

void loop() {
    if (updateReady) {
        updateReady = false; // Reset the flag

        // Read data from the DHT sensor
        float newTemp = dht.readTemperature();
        float newHum = dht.readHumidity();

        // Validate sensor readings
        if (!isnan(newTemp) && !isnan(newHum)) {
            temperature = newTemp;
            humidity = newHum;

            // Save the updated values to EEPROM
            EEPROM.put(0, temperature);
            EEPROM.put(sizeof(temperature), humidity);

            Serial.print("Updated Temp: ");
            Serial.print(temperature);
            Serial.print(" C, Hum: ");
            Serial.print(humidity);
            Serial.println(" %");
        } else {
            Serial.println("Failed to read from DHT sensor");
        }
    }
}

ISR(TIMER1_COMPA_vect) {
    // Set flag to indicate sensordata should be updated
    updateReady = true;
}

void sendSensorData() {
    // Prepare (temperature and humidity)to send 
    uint8_t data[8];
    memcpy(data, &temperature, 4);
    memcpy(data + 4, &humidity, 4);

    // Send data over I2C
    Wire.write(data, 8);

    Serial.println("Sensor data sent");
}
