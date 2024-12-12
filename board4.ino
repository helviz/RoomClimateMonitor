#include <Wire.h>


#define BUTTON_PIN PD2  
#define LED_PIN PB0      
#define COUNTER_MAX 3    

volatile uint8_t pressCount = 0;

void setup() {
    
    Serial.begin(9600);
    Wire.begin(3); /

    // Initialize button as input with pull-up
    DDRD &= ~(1 << BUTTON_PIN); // Set PD2 as input
    PORTD |= (1 << BUTTON_PIN); // Enable pull-up resistor

    // Initialize LED as output
    DDRB |= (1 << LED_PIN);     // Set PB0 as output
    PORTB &= ~(1 << LED_PIN);   // Ensure LED is off

    // Attach external interrupt on falling edge (button press)
    EICRA |= (1 << ISC01);      // Falling edge generates an interrupt
    EIMSK |= (1 << INT0);       // Enable INT0

    Wire.onRequest(sendButtonCount);
    Serial.println("Board with button initialized");
}

void loop() {
    // Main loop can remain empty; all logic is interrupt-driven
}

// Interrupt Service Routine for INT0
ISR(INT0_vect) {
    // Debounce: Wait for button release before incrementing
    if (!(PIND & (1 << BUTTON_PIN))) {
        _delay_ms(50); // Debounce delay
        if (!(PIND & (1 << BUTTON_PIN))) {
            pressCount++;
            if (pressCount > COUNTER_MAX) {
                pressCount = 0;
            }

            // Blink LED to indicate press
            PORTB |= (1 << LED_PIN);  // Turn LED on
            _delay_ms(100);
            PORTB &= ~(1 << LED_PIN); // Turn LED off

            Serial.print("Press count: ");
            Serial.println(pressCount);
        }
    }
}

void sendButtonCount() {
    Wire.write(pressCount); // Send the button press count
    Serial.println("Button count sent");
}
