#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2
#define DHTTYPE DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);


#define SET_BIT(port, bit) ((port) |= (1 << (bit)))
#define CLEAR_BIT(port, bit) ((port) &= ~(1 << (bit)))
#define TOGGLE_BIT(port, bit) ((port) ^= (1 << (bit)))
#define READ_BIT(port, bit) (((port) >> (bit)) & 1)


#define ERROR_LED_PIN PB0 

float temperature = 0;
float humidity = 0;

void setup() {
    
    Serial.begin(9600);

    
    Wire.begin(8); 

    
    dht.begin();

    
    DDRB |= (1 << ERROR_LED_PIN); 
    CLEAR_BIT(PORTB, ERROR_LED_PIN); 

    Serial.println("Board 1: Sensor setup complete");

    
    Wire.onRequest(sendData);
}

void loop() {
    
    sensors_event_t tempEvent, humEvent;
    dht.temperature().getEvent(&tempEvent);
    dht.humidity().getEvent(&humEvent);

    if (!isnan(tempEvent.temperature) && !isnan(humEvent.relative_humidity)) {
        
        temperature = tempEvent.temperature;
        humidity = humEvent.relative_humidity;

        
        Serial.print("Temp: ");
        Serial.print(temperature);
        Serial.print(" C, Hum: ");
        Serial.print(humidity);
        Serial.println(" %");

        
        CLEAR_BIT(PORTB, ERROR_LED_PIN);// Turn off error LED if data is valid
    } else {
        
        Serial.println("Sensor error: Unable to read data");

        
        SET_BIT(PORTB, ERROR_LED_PIN);// Turn on error LED to signal an issue
    }

    delay(2000); // Wait 2 seconds between readings
}

void sendData() {
    // Pack temperature and humidity into a byte array
    uint8_t data[8];
    memcpy(data, &temperature, 4); 
    memcpy(data + 4, &humidity, 4); 

    
    Wire.write(data, 8);

    
    Serial.println("Data sent via I2C");
}
