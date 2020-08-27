#include <GyverEncoder.h>
#include <timer-api.h>
#include <avr/eeprom.h>

#define CLK 9 // S1 pin of encoder
#define DT 8 // S2 pin of encoder
#define SW 7 // key pin of encoder

Encoder enc(CLK, DT, SW); // Encoder class

// Initialize pins of stepper driver
int dir = 2;
int step = 3;
int ms3 = 4;
int ms2 = 5;
int ms1 = 6;
int svetR = 10;
int svetG = 11;
int svetB = 12;

// Initialize variables for function
int speed = 80; // Setting amount of iteration of cycle rotation
int mode = eeprom_read_word(6); // Setting speed modes (second speed mode in default)
int possible_val = eeprom_read_word(4); // Possible steps for rotation
int pos_counter = eeprom_read_word(0); // Position of motor
int speedak; // Steps size

// calibration on = true, calibration off = false
int calibration_enable = false;

// Starting setup of controller
void setup()
{
    enc.setTickMode(AUTO); // Autochecking encoder input
    enc.setType(TYPE2); // Setting encoder type
    timer_init_ISR_1Hz(TIMER_DEFAULT); // Timer start

    // Setting pins of stepper driver in output mode
    pinMode(step, OUTPUT);
    pinMode(dir, OUTPUT);
    pinMode(ms1, OUTPUT);
    pinMode(ms2, OUTPUT);
    pinMode(ms3, OUTPUT);
    pinMode(svetR, OUTPUT);
    pinMode(svetG, OUTPUT);
    pinMode(svetB, OUTPUT);
    Serial.begin(9600);
}

// working func
void loop() {
    // Change normal mode to calibration mode and vice versa
    if (enc.isHolded()) {
        calibration_enable = !calibration_enable;
    }

    // working in normal or calibration mode
    switch (calibration_enable) {
        case false:
            // Changing speed mode number (possible 1, 2, 3)
            if (enc.isLeftH()) { // Left rotation + click
                mode++;
                if (mode>3){ mode = 3; }
            }

            if (enc.isRightH()){ // Right rotation + click
                mode--;
                if (mode<1) { mode = 1; }
            }
            // Changing speed mode with mode number from 1 to 3
            switch (mode) {
                case 1:
                    // Motor rotate speed
                    digitalWrite(ms1, LOW);
                    digitalWrite(ms2, LOW);
                    digitalWrite(ms3, LOW);
                    // Light color
                    digitalWrite(svetG, HIGH);
                    digitalWrite(svetR, LOW);
                    digitalWrite(svetB, LOW);
                    speedak = speed*4;
                    break;
                case 2:
                    // Motor rotate speed
                    digitalWrite(ms1, LOW);
                    digitalWrite(ms2, HIGH);
                    digitalWrite(ms3, LOW);
                    // Light color
                    digitalWrite(svetB, HIGH);
                    digitalWrite(svetR, LOW);
                    digitalWrite(svetG, LOW);
                    speedak = speed;
                    break;
                case 3:
                    // Motor rotate speed
                    digitalWrite(ms1, HIGH);
                    digitalWrite(ms2, HIGH);
                    digitalWrite(ms3, HIGH);
                    // Light color
                    digitalWrite(svetR, HIGH);
                    digitalWrite(svetB, LOW);
                    digitalWrite(svetG, LOW);
                    speedak = speed/4;
                    break;
            }
            break;
        case true:
            // Motor rotate speed
            digitalWrite(ms1, LOW);
            digitalWrite(ms2, LOW);
            digitalWrite(ms3, LOW);
            // Light color
            digitalWrite(svetR, HIGH);
            digitalWrite(svetG, HIGH);
            digitalWrite(svetB, HIGH);

            // Setting position counter to zero for calibration 
            if  (enc.isRightH() || enc.isLeftH()) {
                pos_counter = 0;
            }

            // Maximaze possible rotation value
            if (enc.isDouble()) possible_val = 32000;

            // Calibration of possible steps amount
            if (enc.isSingle()) {
                possible_val = abs(pos_counter/2);
                eeprom_update_word(4, possible_val);
            }
            break;
        }

    //returning rotor to normal position
    if (pos_counter >= possible_val){
        digitalWrite(dir, LOW);
        for (int i=0; i<speed; i++){
            digitalWrite(step, HIGH);
            delayMicroseconds(1000);
            digitalWrite(step, LOW);
            delayMicroseconds(1000);
        }
        pos_counter-=speedak;
    }
    if (pos_counter <= -possible_val){
        digitalWrite(dir, HIGH);
        for (int i=0; i<speed; i++){
            digitalWrite(step, HIGH);
            delayMicroseconds(1000);
            digitalWrite(step, LOW);
            delayMicroseconds(1000);
        }
        pos_counter+=speedak;
    }
    // Rotating of motor (change "speed" for more/less faster running)
    // If rotation exceeds pos_counter amount of rotation it stops
    if (pos_counter < possible_val){
        if (enc.isRight()) { // Right rotation
            digitalWrite(dir, HIGH);
            for (int i=0; i<speed; i++) {
                digitalWrite(step, HIGH);
                delayMicroseconds(1000);
                digitalWrite(step, LOW);
                delayMicroseconds(1000);
            }
            pos_counter+=speedak;
        }
    }
    if (pos_counter > -possible_val){
        if (enc.isLeft()) { // Left rotation
            digitalWrite(dir, LOW);
            for (int i=0; i<speed; i++) {
                digitalWrite(step, HIGH);
                delayMicroseconds(1000);
                digitalWrite(step, LOW);
                delayMicroseconds(1000);
            }
            pos_counter-=speedak;
        }
    }

    // Record position and mode to memory
    eeprom_update_word(0, pos_counter);
    eeprom_update_word(6, mode);
}

void timer_handle_interrupts(int timer) {
    Serial.print(possible_val);
    Serial.print("\n");
    Serial.print(pos_counter);
    Serial.print("\n");
}