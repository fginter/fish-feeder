#include <Servo.h> 
#include "rtc.h"
Servo myservo;

void sweep() {
  for (int i=20; i<180; i+=10) {
      myservo.write(i);
      delay(40);
  }
  delay(1500);
  for (int i=160; i>0; i-=10) {
      myservo.write(i);
      delay(40);
  }
  delay(1500);
/*  for (int i=0; i<10; i++) {
      if (i%2) {
          myservo.write(myservo.read()+40);
      }
      else {
          myservo.write(myservo.read()-40);
      }
      delay(250);
  }*/
  myservo.write(10);
}

void feed(int times) {
  for (int i=0; i<times; i++) {
      sweep();
      delay(4000);
  }
}

uint8_t last; //hold the last hour/minute/whatever an action was taken
void setup() 
{ 
  //          sec min hour dow day month year
  //setup_rtc(0,15,9,6,21,12,2014);
  pinMode(3,INPUT);
  digitalWrite(3,HIGH); //Set 3 to input, pull-up on
  Serial.begin(9600);
  Serial.println(F("DS1302 Real Time Clock"));
  Serial.println(F("Version 2, March 2013"));
  last=60; //impossible number, arms
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
  myservo.write(10);
  delay(1); //let everything stabilize
  if (digitalRead(3)==LOW) feed(2); //Ground on 0 is test mode, feed twice
} 

int check_feed(uint8_t hour, uint8_t minute, uint8_t second) {
    if (hour==22 && minute==10 && second<10 && last!=10) {
        last=10;
        return 2;
    }
    else if (hour==15 && minute==05 && second<10 && last!=05) {
        last=05;
        return 4;
    }
    return 0;
}

void loop() {
  ds1302_struct rtc;
  char buffer[80];     // the code uses 70 characters.

  // Read all clock data at once (burst mode).
  DS1302_clock_burst_read( (uint8_t *) &rtc);
  sprintf( buffer, "Time = %02d:%02d:%02d, ", \
    bcd2bin( rtc.h24.Hour10, rtc.h24.Hour), \
    bcd2bin( rtc.Minutes10, rtc.Minutes), \
    bcd2bin( rtc.Seconds10, rtc.Seconds));

  Serial.println(buffer);

  uint8_t hour=bcd2bin( rtc.h24.Hour10, rtc.h24.Hour);
  uint8_t minute=bcd2bin( rtc.Minutes10, rtc.Minutes);
  uint8_t second=bcd2bin( rtc.Seconds10, rtc.Seconds);
  int feeds=check_feed(hour,minute,second);
  feed(feeds);
  delay(1000);
}
