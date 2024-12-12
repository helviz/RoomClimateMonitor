#include <Wire.h>


#define SET_BIT(port, bit) ((port) |= (1 << (bit)))
#define CLEAR_BIT(port, bit) ((port) &= ~(1 << (bit)))
#define TOGGLE_BIT(port, bit) ((port) ^= (1 << (bit)))
#define READ_BIT(port, bit) (((port) >> (bit)) & 1)


#define STATUS_LED_PIN PB0 // LED to indicate data processing


float temperature = 0;
float humidity = 0;
float comfortIndex = 0;

// calculate comfort index 
float calculateComfortIndex(float temp, float hum) {
    return temp - ((100 - hum) / 5); 
}

// handle received I2C data
void receiveData(int bytes) {
    if (bytes == 8) { /
        uint8_t data[8];
        for (int i = 0; i < 8; i++) {
            data[i] = Wire.read();
        }

        // Extract temperature and humidity from received data
        memcpy(&temperature, data, 4);
        memcpy(&humidity, data + 4, 4);

        // Calculate the comfort index
        comfortIndex = calculateComfortIndex(temperature, humidity);

        // Log data to Serial Monitor
        Serial.print("Received Temp: ");
        Serial.print(temperature);
        Serial.print(", Hum: ");
        Serial.print(humidity);
        Serial.print(", Calculated Comfort Index: ");
        Serial.println(comfortIndex);

        // Indicate successful data processing using LED
        SET_BIT(PORTB, STATUS_LED_PIN);
        delay(100);
        CLEAR_BIT(PORTB, STATUS_LED_PIN);
    }
}

// Function to send comfort index when requested
void sendComfortIndex() {
    Wire.write((uint8_t*)&comfortIndex, 4); // Send comfort index as 4 bytes

    // Debugging: Indicate data sent
    Serial.println("Comfort index sent");

    // Blink LED to confirm transmission
    TOGGLE_BIT(PORTB, STATUS_LED_PIN);
    delay(100);
    TOGGLE_BIT(PORTB, STATUS_LED_PIN);
}

void setup() {
    
    Serial.begin(9600);

    // Initialize I2C communication
    Wire.begin(3); // Set I2C address to 3
    Wire.onRequest(sendComfortIndex); 
    Wire.onReceive(receiveData); 

    // Configure STATUS_LED_PIN as output
    DDRB |= (1 << STATUS_LED_PIN);
    CLEAR_BIT(PORTB, STATUS_LED_PIN); // Ensure LED is off initially

    Serial.println("Board 3: Initialized");
}

void loop() {
    
}
