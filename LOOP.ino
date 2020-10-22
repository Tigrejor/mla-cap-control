#include <Arduino.h>
#include <NeoHWSerial.h>
#include <AccelStepper.h>
#include <LiquidCrystal.h>
#include <Encoder.h>

#define END_SWITCH 10 // End switch for the stepper
#define BUTTON 2 // Button for mode selection

// Uncomment the #define line below if you want to use a potentiometer to control the speed
// Band mode will have max speed of 1000, fine mode will have max speed of 500
// #define POT A4

Encoder myEnc(3, 4); // Encoder, first pin interrupt for better performance (need the other interrupt for the button)
AccelStepper stepper = AccelStepper(8, 5, 6, 7, 8); // Half step mode, 4 pins, 5 through 8
LiquidCrystal lcd(12, 11, A0, A1, A2, A3); // Analog pins as digital, not enough digital pins

#ifdef POT
    int potRead = 0;
    int potValue = 0;
#endif

int position = 0; // Stepper position, stored for LCD and calculation usage
int mode = 1; // Controller operation mode

bool end_switch_status = false; // variable to check whether end switch is active

// Interrupt debouncing variables
const int debouncing_time = 15;
volatile unsigned long last_micros;

void setup()
{
    NeoSerial.begin(115200); // Used only for debugging purposes
    lcd.begin(16, 2);

    lcd.setCursor(1,1);
    lcd.print("LOOP CAPACITOR");
    lcd.setCursor(6, 2);
    lcd.print("V0.1");

    pinMode(END_SWITCH, INPUT_PULLUP);
    pinMode(BUTTON, INPUT_PULLUP);

    attachInterrupt(0, debounceInterrupt, FALLING);

    stepper.setMaxSpeed(1000); // Blocking max speed to 1000 steps/s, anything above may be unreliable

    if (END_SWITCH == LOW){ end_switch_status = true; } // Check whether capacitor is already at the minimum value
    
    delay(500);

    lcd.clear();
    lcd.setCursor(1, 1);
    lcd.print("Calibrating...");
    startup(); // Call stepper position calibration function
    lcd.setCursor(6, 2);
    lcd.print("Done");
    delay(500);
    lcd.clear();
}

void loop()
{
    switch (mode){
        case 1:
            fineAdjust();
            break;
        case 2:
            bandAdjust();
            break;
        default:
            break;
        }
    NeoSerial.println(stepper.currentPosition() + " " + (String)position + " " + stepper.speed());
}

// Move the stepper backwards until it reaches end switch to have a reference
// Only runs at startup, no reason to run it again unless the stepper is missing steps and code is rewritten to add any way of running the calibration again, but if the stepper is missing steps you probably want to fix that instead
void startup()
{
    while(!end_switch_status){
        stepper.setSpeed(-500);
        stepper.runSpeed();
        if (END_SWITCH == LOW)
        {
            end_switch_status = true;
        }
        else
        {
            end_switch_status = false;
        }
    }
    stepper.setCurrentPosition(0);
}

// Fine adjust of capacitor
void fineAdjust()
{
    if(position != stepper.currentPosition()){
        stepper.moveTo(position);
        #ifndef POT
            stepper.setSpeed(100);
        #else
            potRead = analogRead(POT);
            potValue = map(potRead, 0, 1023, 0, 500)
            stepper.setSpeed()
        #endif
        stepper.runSpeedToPosition();
    }
}

// Coarse adjust of capacitor to select bands faster. Will add another mode for predefined band positions
void bandAdjust()
{
    stepper.setSpeed(1000); // Max speed, values above 1000 are blocked earlier in the code
}



// Interrupt code for the button

void debounceInterrupt()
{
    if((long)(micros() - last_micros) >= debouncing_time * 1000){
        Interrupt();
        last_micros = micros();
    }
}

void Interrupt()
{
    if(mode == 2){ mode = 0; } // Reset mode according to max modes
    ++mode;
    NeoSerial.println("Mode change: ");
    NeoSerial.print(mode);
}