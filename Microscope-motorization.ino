#include <GyverEncoder.h>
// #include <EEPROM.h>
#include <avr/eeprom.h>

#define CLK 7 // S1 pin of encoder
#define DT 8 // S2 pin of encoder
#define SW 9 // key pin of encoder

Encoder enc(CLK, DT, SW); // Encoder class

// Initialize pins of stepper driver
int dir = 2;
int step = 3;
int ms3 = 4;
int ms2 = 5;
int ms1 = 6;
int svet1 = 10;
int svet2 = 11;
int svet3 = 12;

// Initialize variables for function
int speed = 80; // Setting amount of iteration of cycle rotation
int mode = eeprom_read_word(6); // Setting speed modes (second speed mode in default)
int possible_val = eeprom_read_word(4); // Possible steps for rotation
int pos_counter = eeprom_read_word(0); // Position of motor
int speedak; // Steps size
// Mode variables
bool v1;
bool v2;
bool v3;

// Starting setup of controller
void setup()
{
    enc.setTickMode(AUTO); // Autochecking encoder input
    enc.setType(TYPE2); // Setting encoder type

    // Setting pins of stepper driver in output mode
    pinMode(step, OUTPUT);
    pinMode(dir, OUTPUT);
    pinMode(ms1, OUTPUT);
    pinMode(ms2, OUTPUT);
    pinMode(ms3, OUTPUT);
    pinMode(svet1, OUTPUT);
    pinMode(svet2, OUTPUT);
    pinMode(svet3, OUTPUT);
    // pinMode(enable, OUTPUT);
    Serial.begin(9600);
}

// working func
void loop() {
    // Changing speed mode number (possible 1, 2, 3)
    if (enc.isLeftH()) { // Left rotation + click
        mode++;
        if (mode>3){
        mode = 3;
        }
    }

    if (enc.isRightH()){ // Right rotation + click
        mode--;
        if (mode<1) {
            mode = 1;
        }
    }
    
    // Changing speed mode with mode number from 1 to 3
    if (mode == 1){
        v1 = LOW;
        v2 = LOW;
        v3 = LOW;
        digitalWrite(svet1, HIGH);
        digitalWrite(svet2, LOW);
        digitalWrite(svet3, LOW);
        speedak = speed*2;
    } else if (mode == 2) {
            v1 = LOW;
            v2 = HIGH;
            v3 = LOW;
            digitalWrite(svet2, HIGH);
            digitalWrite(svet1, LOW);
            digitalWrite(svet3, LOW);
            speedak = speed;
        } else if (mode == 3){
                v1 = HIGH;
                v2 = HIGH;
                v3 = HIGH;
                digitalWrite(svet3, HIGH);
                digitalWrite(svet1, LOW);
                digitalWrite(svet2, LOW);
                speedak = speed/8;
            }
    // Setting up step speed with variables
    digitalWrite(ms1, v1);
    digitalWrite(ms2, v2);
    digitalWrite(ms3, v3);

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
    if (enc.isDouble()){
        possible_val = 8001;
    }
    // Calibration of possible steps amount
    if (enc.isSingle()) {
        possible_val = abs(pos_counter);
        eeprom_update_word(4, possible_val);
    }

    // Record position and mode to memory
    eeprom_update_word(0, pos_counter);
    eeprom_update_word(6, mode);
    // Serial.print(possible_val);
    // Serial.print("\n");
    // Serial.print(pos_counter);
    // Serial.print("\n");
}
