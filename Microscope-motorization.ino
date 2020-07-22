#include <GyverEncoder.h>
// #include <set.h>
#define CLK 9 // S1 pin of encoder
#define DT 10 // S2 pin of encoder
#define SW 11 // key pin of encoder

Encoder enc(CLK, DT, SW); // Encoder class

// Initialize pins of stepper driver
int step = 4;
int dir = 5;
int ms1 = 8;
int ms2 = 7;
int ms3 = 6;

// Initialize variables for function
int speed = 60; // Setting amount of iteration of cycle rotation
int mode = 2; // Setting speed modes (second speed mode in default)
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
    } else if (mode == 2) {
            v1 = HIGH;
            v2 = LOW;
            v3 = LOW;
        } else if (mode == 3){
                v1 = HIGH;
                v2 = HIGH;
                v3 = LOW;
            }
    // Setting up step speed with variables
    digitalWrite(ms1, v1);
    digitalWrite(ms2, v2);
    digitalWrite(ms3, v3);

    // Rotating of motor (change "speed" for more/less faster running)
    if (enc.isRight()) { // Right rotation
        digitalWrite(dir, HIGH);
        for (int i=0; i<speed; i++) {
            digitalWrite(step, HIGH);
            delayMicroseconds(1000);
            digitalWrite(step, LOW);
            delayMicroseconds(1000);
        }
    }
    if (enc.isLeft()) { // Left rotation
        digitalWrite(dir, LOW);
        for (int i=0; i<speed; i++) {
            digitalWrite(step, HIGH);
            delayMicroseconds(1000);
            digitalWrite(step, LOW);
            delayMicroseconds(1000);
        }
    }
}
