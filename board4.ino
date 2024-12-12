#include <Wire.h>
#include <EEPROM.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define BUTTON_PIN PD2   

volatile uint8_t pressCount = 0;
volatile bool debounceFlag = false; 

void setup() {
    Serial.begin(115200);
    Wire.begin(3);
    Wire.onRequest(sendButtonCount);

    
    DDRD &= ~(1 << BUTTON_PIN);   // Set PD2  as input
    PORTD |= (1 << BUTTON_PIN);   // Enable pull-up resistor on PD2

    // Retrieve press count from EEPROM
    EEPROM.get(0, pressCount);

    // Configure Timer1 
    cli(); 
    TCCR1A = 0;                  // Normal mode
    TCCR1B = (1 << WGM12) | (1 << CS12); // prescaler = 256
    OCR1A = 62500;               // Compare value for 1-second interrupt
    TIMSK1 |= (1 << OCIE1A);     // Enable Timer1 compare interrupt
    sei(); 
    Serial.println("Board with custom timer initialized");
}

void loop() {
    
}

ISR(TIMER1_COMPA_vect) {
    static bool lastButtonState = HIGH;
    bool currentButtonState = (PIND & (1 << BUTTON_PIN)) == 0; 

    // Check for button press
    if (!debounceFlag && currentButtonState && lastButtonState == HIGH) {
        debounceFlag = true; 

        pressCount++;
        if (pressCount > COUNTER_MAX) {
            pressCount = 0;
        }

        // Save updated count to EEPROM
        EEPROM.put(0, pressCount);

        Serial.print("Press count: ");
        Serial.println(pressCount);
    }

    // Reset debounce after the button is released
    if (!currentButtonState) {
        debounceFlag = false;
    }

    lastButtonState = currentButtonState;
}

void sendButtonCount() {
    Wire.write(pressCount);
}
