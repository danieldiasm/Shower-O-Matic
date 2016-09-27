/*
Project Started in 29/07/2015
This is the software for a device which control a shower through bluetooth with liters spent shown on a display
Created by Daniel Zulato Dias de Moraes - daniel_diasm@hotmail.com

There is a lot of issues with the code, bad pratices and can be optimized in many ways. Feel free to improve it!
*/

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
char displaybuffer[4] = {' ', ' ', ' ', ' '};
char BTval;
char lastValue;

LiquidCrystal_I2C lcd(0x27,20,4);

// which pin to use for reading the sensor? can use any pin!
#define FLOWSENSORPIN 2
// count how many pulses!
volatile uint16_t pulses = 0;
// track the state of the pulse pin
volatile uint8_t lastflowpinstate;
// you can try to keep time of how long it is between pulses
volatile uint32_t lastflowratetimer = 0;
// and use that to calculate a flow rate
volatile float flowrate;
// Interrupt is called once a millisecond, looks for any pulses from the sensor!
SIGNAL(TIMER0_COMPA_vect) {
  uint8_t x = digitalRead(FLOWSENSORPIN);

  
  
  if (x == lastflowpinstate) {
    lastflowratetimer++;
    return; // nothing changed!
  }
  
  if (x == HIGH) {
    //low to high transition!
    pulses++;
  }
  lastflowpinstate = x;
  flowrate = 1000.0;
  flowrate /= lastflowratetimer;  // in hertz
  lastflowratetimer = 0;
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
  }
}



void setup() {
  
  alpha4.begin(0x70);
  alpha4.writeDigitRaw(3, 0x0);
  alpha4.writeDigitRaw(0, 0xFFFF);
  alpha4.writeDisplay();
  delay(200);
  alpha4.writeDigitRaw(0, 0x0);
  alpha4.writeDigitRaw(1, 0xFFFF);
  alpha4.writeDisplay();
  delay(200);
  alpha4.writeDigitRaw(1, 0x0);
  alpha4.writeDigitRaw(2, 0xFFFF);
  alpha4.writeDisplay();
  delay(200);
  alpha4.writeDigitRaw(2, 0x0);
  alpha4.writeDigitRaw(3, 0xFFFF);
  alpha4.writeDisplay();
  delay(200);
  alpha4.clear();
  alpha4.writeDisplay();
  
   //Bluetooth  
   pinMode(3,OUTPUT);
  
   //LED BarGraph  
   pinMode(13, OUTPUT);
   pinMode(12, OUTPUT);
   pinMode(11, OUTPUT);
   pinMode(10, OUTPUT);
   pinMode(9 , OUTPUT);
   pinMode(8 , OUTPUT);
   pinMode(7 , OUTPUT);
   pinMode(6 , OUTPUT);
   pinMode(5 , OUTPUT);
   pinMode(4 , OUTPUT);
   
   //LCD
   lcd.init();
   lcd.backlight();   
   lcd.begin(16, 2);
   lcd.clear();
   lcd.setCursor(0,1);
   lcd.print("Solenoid is OFF "); 
   
   //Bluetooth Serial
   Serial.begin(9600);
   
   //Flow Sensor
   pinMode(FLOWSENSORPIN, INPUT);
   digitalWrite(FLOWSENSORPIN, HIGH);
   lastflowpinstate = digitalRead(FLOWSENSORPIN);
   useInterrupt(true);
   
   //Solenoid
   digitalWrite(3,LOW);
   
}

void loop()                     // run over and over again
{ 
  
    if(Serial.available())
  {//if there is data being recieved
    BTval=Serial.read(); //read it
  }
  if (BTval=='n')
  {//if value from bluetooth serial is n
    digitalWrite(3,HIGH);            //switch on LED
    if (lastValue!='n')
      Serial.println(F("LED is on")); //print LED is on
    lcd.setCursor(0,1);
    lcd.print("Solenoid is ON  ");      
    lastValue=BTval;
  }
  else if (BTval=='f')
  {//if value from bluetooth serial is n
    digitalWrite(3,LOW);             //turn off LED
    if (lastValue!='f')
      Serial.println(F("LED is off")); //print LED is on
    lcd.setCursor(0,1);
    lcd.print("Solenoid is OFF ");     
    lastValue=BTval;
  }
  
  lcd.setCursor(0,0);
  lcd.print("Pulses:"); lcd.print(pulses);
  //lcd.print("Hz:");   lcd.print(flowrate);
  
  
  //lcd.print(flowrate);
 /*Serial.print("Freq: "); Serial.println(flowrate, DEC);
  Serial.print("Pulses: "); Serial.println(pulses, DEC);*/
  
  // if a plastic sensor use the following calculation
  // Sensor Frequency (Hz) = 8.1 * Q (Liters/min)
  // Liters = Q * time elapsed (seconds) / 60 (seconds/minute)
  // Liters = (Frequency (Pulses/second) / 8.1) * time elapsed (seconds) / 60
  // Liters = Pulses / (8.1 * 60)
  /*float liters = pulses;

  // if a brass sensor use the following calculation (For chinese flow sensor 11? was wrong)*/
  
  float liters = pulses;
  liters /= 8.1;
  liters -= 6;
  liters /= 60.0;

  //Serial.print(liters); Serial.println(" Liters");
  /*lcd.setCursor(0, 1);
  lcd.print(liters); lcd.print(" Liters        ");*/

//====================================== LED DISPLAY ======================= 
  
  //displaybuffer = liters;  
  //set every digit to the buffer
  dtostrf(liters,2,2,displaybuffer);
  alpha4.writeDigitAscii(0, displaybuffer[0]);
  alpha4.writeDigitAscii(1, displaybuffer[1]);
  alpha4.writeDigitAscii(2, displaybuffer[2]);
  alpha4.writeDigitAscii(3, displaybuffer[3]);
  
  // write it out!
  alpha4.writeDisplay();
  

//====================================== LED BARGRAPH ======================= 
  if (flowrate > 5)
  {digitalWrite(4 , LOW);
   digitalWrite(5 , LOW);
   digitalWrite(6 , LOW);
   digitalWrite(7 , LOW);
   digitalWrite(8 , LOW);
   digitalWrite(9 , LOW);
   digitalWrite(10, LOW);
   digitalWrite(11, LOW);
   digitalWrite(12, LOW);
   digitalWrite(13, LOW);}
   
  if ((flowrate > 10) && (flowrate < 20))
   {digitalWrite(4 , LOW);
   digitalWrite(5 , LOW);
   digitalWrite(6 , LOW);
   digitalWrite(7 , LOW);
   digitalWrite(8 , LOW);
   digitalWrite(9 , LOW);
   digitalWrite(10, LOW);
   digitalWrite(11, LOW);
   digitalWrite(12, LOW);
   digitalWrite(13, HIGH);}
  
  if ((flowrate > 20) && (flowrate < 30))
   {digitalWrite(4 , LOW);
   digitalWrite(5 , LOW);
   digitalWrite(6 , LOW);
   digitalWrite(7 , LOW);
   digitalWrite(8 , LOW);
   digitalWrite(9 , LOW);
   digitalWrite(10, LOW);
   digitalWrite(11, LOW);
   digitalWrite(12, HIGH);
   digitalWrite(13, HIGH);}
  
  if ((flowrate > 30) && (flowrate < 40))
  {digitalWrite(4 , LOW);
   digitalWrite(5 , LOW);
   digitalWrite(6 , LOW);
   digitalWrite(7 , LOW);
   digitalWrite(8 , LOW);
   digitalWrite(9 , LOW);
   digitalWrite(10, LOW);
   digitalWrite(11, HIGH);
   digitalWrite(12, HIGH);
   digitalWrite(13, HIGH);}
  
    if ((flowrate > 40) && (flowrate < 50))
  {digitalWrite(4 , LOW);
   digitalWrite(5 , LOW);
   digitalWrite(6 , LOW);
   digitalWrite(7 , LOW);
   digitalWrite(8 , LOW);
   digitalWrite(9 , LOW);
   digitalWrite(10, HIGH);
   digitalWrite(11, HIGH);
   digitalWrite(12, HIGH);
   digitalWrite(13, HIGH);}
  
    if ((flowrate > 50) && (flowrate < 60))
  {digitalWrite(4 , LOW);
   digitalWrite(5 , LOW);
   digitalWrite(6 , LOW);
   digitalWrite(7 , LOW);
   digitalWrite(8 , LOW);
   digitalWrite(9 , HIGH);
   digitalWrite(10, HIGH);
   digitalWrite(11, HIGH);
   digitalWrite(12, HIGH);
   digitalWrite(13, HIGH);}
   
    if ((flowrate > 60) && (flowrate < 70))
  {digitalWrite(4 , LOW);
   digitalWrite(5 , LOW);
   digitalWrite(6 , LOW);
   digitalWrite(7 , LOW);
   digitalWrite(8 , HIGH);
   digitalWrite(9 , HIGH);
   digitalWrite(10, HIGH);
   digitalWrite(11, HIGH);
   digitalWrite(12, HIGH);
   digitalWrite(13, HIGH);}
   
     if ((flowrate > 70) && (flowrate < 80))
  {digitalWrite(4 , LOW);
   digitalWrite(5 , LOW);
   digitalWrite(6 , LOW);
   digitalWrite(7 , HIGH);
   digitalWrite(8 , HIGH);
   digitalWrite(9 , HIGH);
   digitalWrite(10, HIGH);
   digitalWrite(11, HIGH);
   digitalWrite(12, HIGH);
   digitalWrite(13, HIGH);}
   
     if ((flowrate > 80) && (flowrate < 90))
  {digitalWrite(4 , LOW);
   digitalWrite(5 , LOW);
   digitalWrite(6 , HIGH);
   digitalWrite(7 , HIGH);
   digitalWrite(8 , HIGH);
   digitalWrite(9 , HIGH);
   digitalWrite(10, HIGH);
   digitalWrite(11, HIGH);
   digitalWrite(12, HIGH);
   digitalWrite(13, HIGH);}
   
      if ((flowrate > 90) && (flowrate < 100))
  {digitalWrite(4 , LOW);
   digitalWrite(5 , HIGH);
   digitalWrite(6 , HIGH);
   digitalWrite(7 , HIGH);
   digitalWrite(8 , HIGH);
   digitalWrite(9 , HIGH);
   digitalWrite(10, HIGH);
   digitalWrite(11, HIGH);
   digitalWrite(12, HIGH);
   digitalWrite(13, HIGH);}
   
      if ((flowrate > 100) && (flowrate < 1000))
  {digitalWrite(4 , HIGH);
   digitalWrite(5 , HIGH);
   digitalWrite(6 , HIGH);
   digitalWrite(7 , HIGH);
   digitalWrite(8 , HIGH);
   digitalWrite(9 , HIGH);
   digitalWrite(10, HIGH);
   digitalWrite(11, HIGH);
   digitalWrite(12, HIGH);
   digitalWrite(13, HIGH);}
    
   
   delay(100);
}


