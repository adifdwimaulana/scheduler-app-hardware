// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include "RTClib.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);

char daysOfTheWeek[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};

String dayUrl;
const int deviceId = 1;
bool isKuliah = false;

void setup () {
  Serial.begin(57600);

#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
//    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
   rtc.adjust(DateTime(2020, 6, 11, 21, 18, 45));

  lcd.begin();

  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print("Scheduler App");
  delay(1000);
  lcd.setCursor(0,0);
  lcd.print("Ruangan JJ209");
  lcd.setCursor(0,1);
  lcd.print("Device Id = ");
  lcd.setCursor(12, 1);
  lcd.print(deviceId);
  delay(3000);
  lcd.clear();
}

void loop () {
  // RTC Configuration
  DateTime now = rtc.now();
  int currentYear = now.year();
  int currentMonth = now.month();
  int currentDate = now.day();
  String today = daysOfTheWeek[now.dayOfTheWeek()];
  int currentHour = now.hour();
  int currentMinute = now.minute();
  int currentSecond = now.second();
  String currentTime = String(currentHour) + ":" + String(currentMinute) + ":" + String(currentSecond);
  Serial.println(currentTime);
  
  lcd.setCursor(0,0);
  lcd.print(today);
  lcd.print(" ");
  lcd.print(currentDate);
  lcd.print("-");
  lcd.print(currentMonth);
  lcd.print("-");
  lcd.print(currentYear);


  // URL    
  today.toLowerCase();

  String url = 

  if(isKuliah == false) {
    // Firebase
    
    lcd.setCursor(0,1);
    lcd.print("Menunggu Kuliah");
  } else {
    // Firebase
  
    lcd.setCursor(0,1);
    lcd.print("Ada Kuliah...");
  }
  
  




  delay(1000);
//  lcd.clear();
}
