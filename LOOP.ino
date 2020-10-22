#include <Arduino.h>
#include <NeoHWSerial.h>
#include <AccelStepper.h>
#include <LiquidCrystal.h>
#include <Encoder.h>

#define END_SWITCH 10
#define BUTTON 2

Encoder myEnc(3, 4);
AccelStepper stepper = AccelStepper(8, 5, 6, 7, 8);
LiquidCrystal lcd(12, 11, A0, A1, A2, A3);

int position = 0;
int temp_pos = 0;
int pot_read = 0;
bool end_switch_status = false;

void setup()
{
    NeoSerial.begin(115200);
    lcd.begin(16, 2);

    lcd.setCursor(1,1);
    lcd.print("LOOP CAPACITOR");
    lcd.setCursor(6, 2);
    lcd.print("V1.0");

    pinMode(END_SWITCH, INPUT_PULLUP);
    pinMode(BUTTON, INPUT_PULLUP);

    stepper.setMaxSpeed(1000);

    if (END_SWITCH == LOW){ end_switch_status = true; }   
    
    delay(500);

    lcd.clear();
    lcd.setCursor(1, 1);
    lcd.print("Calibrating...");
    startup();
    lcd.setCursor(6, 2);
    lcd.print("Done");
    delay(500);
    lcd.clear();
}

void loop()
{

}

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
