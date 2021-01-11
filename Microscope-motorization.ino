#include <GyverEncoder.h> // library for encoder commands
// #include <timer-api.h> // library for timer
#include <avr/eeprom.h> // library for flash memory
#include <LiquidCrystal.h> // include the library code


#define CLK 10 // S1 pin of encoder
#define DT 11 // S2 pin of encoder
#define SW 12 // key pin of encoder

Encoder enc(CLK, DT, SW); // Encoder class

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 13, en = A0, d4 = A1, d5 = A2, d6 = A3, d7 = A4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Initialize pins of stepper driver
int dir = 2;
int step = 3;
int ms3 = 6;
int ms2 = 7;
int ms1 = 8;

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
    // timer_init_ISR_1Hz(TIMER_DEFAULT); // Timer start
    lcd.begin(16, 2); // set up the LCD's number of columns and rows:

    // Setting pins of stepper driver in output mode
    pinMode(step, OUTPUT);
    pinMode(dir, OUTPUT);
    pinMode(ms1, OUTPUT);
    pinMode(ms2, OUTPUT);
    pinMode(ms3, OUTPUT);
    lcd.setCursor(6,0);
    lcd.print("pos");
    lcd.setCursor(0, 1);
    lcd.print("v =   max");
    // Serial.begin(9600);
}

// working func
void loop() {
    // Change normal mode to calibration mode and vice versa
    if (enc.isHolded()) {
        calibration_enable = !calibration_enable;
        lcd.home();
        lcd.print("     ");
        lcd.setCursor(10, 1);
        lcd.print("     ");

    }

    // working in normal or calibration mode
    switch (calibration_enable) {
        case false:
            // Changing speed mode number (possible 1, 2, 3)
            if (enc.isLeftH()) { // Left rotation + click
                mode++;
                if (mode>3) mode = 3;
            }

            if (enc.isRightH()){ // Right rotation + click
                mode--;
                if (mode<1) mode = 1;
            }
            // Changing speed mode with mode number from 1 to 3
            switch (mode) {
                case 3:
                    // Motor rotate speed
                    digitalWrite(ms1, LOW);
                    digitalWrite(ms2, LOW);
                    digitalWrite(ms3, LOW);
                    speedak = speed*4;
                    break;
                case 2:
                    // Motor rotate speed
                    digitalWrite(ms1, LOW);
                    digitalWrite(ms2, HIGH);
                    digitalWrite(ms3, LOW);
                    speedak = speed;
                    break;
                case 1:
                    // Motor rotate speed
                    digitalWrite(ms1, HIGH);
                    digitalWrite(ms2, HIGH);
                    digitalWrite(ms3, HIGH);
                    speedak = speed/4;
                    break;
            }
            lcd.setCursor(4,1);
                lcd.print(mode);
            break;
        case true:
            lcd.setCursor(0, 0);
            lcd.print("Calib");
            // Motor rotate speed
            digitalWrite(ms1, LOW);
            digitalWrite(ms2, LOW);
            digitalWrite(ms3, LOW);
            speedak = speed*4;

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

    // Print out info to lcd monitor
    lcd.setCursor(11, 0);
    lcd.print("     ");
    lcd.setCursor(10, 0);
    lcd.print(pos_counter);
    lcd.setCursor(10, 1);
    lcd.print(possible_val);
    // lcd.clear();
}

// void timer_handle_interrupts(int timer) {
//     lcd.setCursor(6,0);
//     lcd.print("pos");
//     lcd.setCursor(0, 1);
//     lcd.print("v =   max");
//     lcd.setCursor(10, 0);
//     lcd.print(pos_counter);
//     lcd.setCursor(10, 1);
//     lcd.print(possible_val);
//     lcd.setCursor(4,1);
//     lcd.print(mode);
//     // lcd.clear();
// }