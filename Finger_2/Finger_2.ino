//FirebaseESP8266.h must be included before ESP8266WiFi.h
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include "RTClib.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>

#define WIFI_SSID "KONTRAKAN ANAK2"
#define WIFI_PASSWORD "bukaopen"
#define FIREBASE_HOST "finger-scheduler.firebaseio.com" //Without http:// or https:// schemes
#define FIREBASE_AUTH "Ad5JNpZqepilbQQGgMTE6dzR9DcmiXqDr1cPITjZ"

FirebaseJson json;

FirebaseData firebaseData;
SoftwareSerial mySerial(0, 2); //D3, D4

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

const int checkBtn = 14; //D5 //Pencet == LOW
const int regisBtn = 12; //D6 //Pencet == LOW
const int emergencyBtn = 16; //D0 //Pencet == LOW
const int door = 13;
const int contactor = 15;
bool checkState = false;
bool regisState = false;
bool doorState = false;
bool contactorState = false;
bool emergency = false;
bool login = false;
int finishKuliah = 0;
bool daruratState = false;


String jsonStr;
String path;
const int deviceId = 2;

char daysOfTheWeek[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};

size_t len;

// Global vars Data Jadwal dan Dosen
String nowNamaMatkul, nowDosen, nowId, nowStart, nowEnd, nowRuangan;
int matkulCount;

int id;
int startFetch = 3000;
int startTime = 0;
int blinkState = 0;
int doorCount = 0;
int loginCount = 0;
int daruratCount = 0;
String nextDay = "";

void setup()
{
    if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
  }

   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  lcd.begin();

  lcd.backlight();
  lcd.print("Scheduler App");
  delay(1000);
  lcd.setCursor(0,0);
  lcd.print("Ruangan M205 ");
  lcd.setCursor(0,1);
  lcd.print("Device Id = ");
  lcd.setCursor(12, 1);
  lcd.print(deviceId);
  delay(3000);
  lcd.clear();

  Serial.begin(57600);
  Serial.println();
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
      Serial.print(".");
      delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  firebaseData.setBSSLBufferSize(1024, 1024);

  //Set the size of HTTP response buffers in the case where we want to work with large data.
  firebaseData.setResponseSize(1024);
  finger.begin(57600);  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  finger.getTemplateCount();

    if (finger.templateCount == 0) {
     Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
    lcd.print("Sensor not found");
  } 
  else {
    Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }

    id = readnumber();
    Serial.print("Counter = ");
    Serial.println(id);

    Serial.println("------------------------------------");
    Serial.println("Get JSON test...");

    pinMode(checkBtn, INPUT_PULLUP);
    pinMode(regisBtn, INPUT_PULLUP);
    pinMode(door, OUTPUT);
    pinMode(contactor, OUTPUT);
}

void printResult(FirebaseData &data)
{

    if (data.dataType() == "int")
        Serial.println(data.intData());
    else if (data.dataType() == "float")
        Serial.println(data.floatData(), 5);
    else if (data.dataType() == "double")
        printf("%.9lf\n", data.doubleData());
    else if (data.dataType() == "boolean")
        Serial.println(data.boolData() == 1 ? "true" : "false");
    else if (data.dataType() == "string")
        Serial.println(data.stringData());
    else if (data.dataType() == "json")
    {
        Serial.println();
        FirebaseJson &json = data.jsonObject();
        //Print all object data
//        Serial.println("Pretty printed JSON data:");
        json.toString(jsonStr, true);
//        Serial.println(jsonStr);
//        Serial.println();
//        Serial.println("Iterate JSON data:");
//        Serial.println();
        len = json.iteratorBegin();
//        Serial.print("Size = ");
//        Serial.println(len);
        String key, value = "";
        int type = 0;
        matkulCount = len;
        for (size_t i = 0; i < len; i++)
        {
            json.iteratorGet(i, type, key, value);
            if (type == FirebaseJson::JSON_OBJECT)
            {
//                Serial.print(", Key: ");
//                Serial.print(key);
            }
//            Serial.print(", Value: ");
//            Serial.println(value);
            if(finishKuliah == 0){
              if(i == 2){
                nowId = value;
              } else if(i == 3){
                nowDosen = value;
              } else if(i == 4){
                nowEnd = value;
              } else if(i == 5){
                nowNamaMatkul = value;
              } else if(i == 6){
                nowRuangan = value;
              } else if(i == 7){
                nowStart = value;
              }  
            } else if(finishKuliah == 1){
              if(i == 10){
                nowId = value;
              } else if(i == 11){
                nowDosen = value;
              } else if(i == 12){
                nowEnd = value;
              } else if(i == 13){
                nowNamaMatkul = value;
              } else if(i == 14){
                nowRuangan = value;
              } else if(i == 15){
                nowStart = value;
              } 
            } else if(finishKuliah == 2){
              if(i == 18){
                nowId = value;
              } else if(i == 19){
                nowDosen = value;
              } else if(i == 20){
                nowEnd = value;
              } else if(i == 21){
                nowNamaMatkul = value;
              } else if(i == 22){
                nowRuangan = value;
              } else if(i == 23){
                nowStart = value;
              } else if(finishKuliah == 3){
              if(i == 26){
                nowId = value;
              } else if(i == 27){
                nowDosen = value;
              } else if(i == 28){
                nowEnd = value;
              } else if(i == 29){
                nowNamaMatkul = value;
              } else if(i == 30){
                nowRuangan = value;
              } else if(i == 31){
                nowStart = value;
              } 
            } 
            }
        }
        json.iteratorEnd();
    }
    else if (data.dataType() == "array")
    {
        Serial.println();
        //get array data from FirebaseData using FirebaseJsonArray object
        FirebaseJsonArray &arr = data.jsonArray();
        //Print all array values
        Serial.println("Pretty printed Array:");
        String arrStr;
        arr.toString(arrStr, true);
        Serial.println(arrStr);
        Serial.println();
        Serial.println("Iterate array values:");
        Serial.println();
        for (size_t i = 0; i < arr.size(); i++)
        {
            Serial.print(i);
            Serial.print(", Value: ");

            FirebaseJsonData &jsonData = data.jsonData();
            //Get the result data from FirebaseJsonArray object
            arr.get(jsonData, i);
            if (jsonData.typeNum == FirebaseJson::JSON_BOOL)
                Serial.println(jsonData.boolValue ? "true" : "false");
            else if (jsonData.typeNum == FirebaseJson::JSON_INT)
                Serial.println(jsonData.intValue);
            else if (jsonData.typeNum == FirebaseJson::JSON_DOUBLE)
                printf("%.9lf\n", jsonData.doubleValue);
            else if (jsonData.typeNum == FirebaseJson::JSON_STRING ||
                     jsonData.typeNum == FirebaseJson::JSON_NULL ||
                     jsonData.typeNum == FirebaseJson::JSON_OBJECT ||
                     jsonData.typeNum == FirebaseJson::JSON_ARRAY)
                Serial.println(jsonData.stringValue);
        }
    }
}

String getValue(String input, char separator, int index){
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = input.length();

  for(int i=0; i <= maxIndex && found<= index; i++){
    if(input.charAt(i) == separator || i == maxIndex){
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? input.substring(strIndex[0], strIndex[1]) : "";
}

uint8_t key_led( uint8_t level)
{
  uint8_t state;
  digitalWrite(emergencyBtn, 1);
  pinMode(emergencyBtn, INPUT);
  state = digitalRead(emergencyBtn);
  pinMode(emergencyBtn, OUTPUT);
  digitalWrite(emergencyBtn, level);
  return state;
}

void loop(){  
  DateTime now = rtc.now();
  int currentYear = now.year();
  int currentMonth = now.month();
  int currentDate = now.day();
  String today = daysOfTheWeek[now.dayOfTheWeek()];
  if(nextDay != today){
    finishKuliah = 0;  
  }
  nextDay = today;
  int currentHour = now.hour();
  int currentMinute = now.minute();
  int currentSecond = now.second();
  String stringMinute = String(currentMinute);
  String stringSecond = String(currentSecond);
  String stringHour = String(currentHour);

  if(currentMinute < 10){
    stringMinute = "0" + stringMinute;
  }
  if(currentSecond < 10){
    stringSecond = "0" + stringSecond;
  }
  if(currentHour < 10){
    stringHour = "0" + stringHour;  
  }
  String currentTime = stringHour + ":" + stringMinute + ":" + stringSecond;
  Serial.println(currentTime);
  Serial.print("Fetch = ");
  Serial.println(startFetch);
  Serial.print("Time = ");
  Serial.println(startTime);
  Serial.print("Blink = ");
  Serial.println(blinkState);

  if(startTime % 2 == 0){
    lcd.clear();
      if(blinkState % 2 == 0){
        lcd.setCursor(0,0);
        lcd.print(today);
        lcd.print(" ");
        lcd.print(currentTime);        
      } else {
        lcd.setCursor(0,0);
        lcd.print(today);
        lcd.print(" ");
        lcd.print(currentDate);
        lcd.print("-");
        lcd.print(currentMonth);
        lcd.print("-");
        lcd.print(currentYear);      
      }
      blinkState++;
  }
  
  // URL    
  today.toLowerCase();
  path = "/" + String(deviceId) + "/matkul/" + today;
   if (Firebase.get(firebaseData, path)){
        if (firebaseData.dataType() == "json")
        {
            jsonStr = firebaseData.jsonString();
            Serial.println(jsonStr);
            printResult(firebaseData);
        }
        Serial.println("Fetching Data Success ...");
    }
    else
    {
        Serial.println("FAILED");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
    } 
   
      String startHour;
      String startMinute;
      String endHour;
      String endMinute;
      String currentMatkul;
      String currentDosenId;

      startHour = getValue(nowStart, ':', 0);
      startMinute = getValue(nowStart, ':', 1);
      endHour = getValue(nowEnd, ':', 0);
      endMinute = getValue(nowEnd, ':', 1);
      
    if(startHour.toInt() < 10){
        startHour = startHour[1];
    }
    if(startMinute.toInt() < 10){
        startMinute = startMinute[1];
    }
    if(endHour.toInt() < 10){
        endHour = endHour[1];
    }
    if(endMinute.toInt() < 10){
        endMinute = endMinute[1];
    }

      Serial.print("Start Hour = ");
      Serial.println(startHour);
      Serial.print("End Hour = ");
      Serial.println(endHour);
      Serial.print("Start Minute = ");
      Serial.println(startMinute);
      Serial.print("End Minute = ");
      Serial.println(endMinute);
      Serial.print("Current Hour = ");
      Serial.println(currentHour);
      Serial.print("Current Minute = ");
      Serial.println(currentMinute);

    
    Serial.print("Matkul = ");
    Serial.println(nowNamaMatkul);

    if(currentHour <= startHour.toInt() && currentMinute < startMinute.toInt()){
      tidakAdaKuliah();  
    } else if(currentHour >= endHour.toInt() && currentMinute > endMinute.toInt()){
      tidakAdaKuliah();  
    } else if(currentHour >= startHour.toInt() && currentMinute >= startMinute.toInt()){
      adaKuliah();  
    } else if(currentHour <= endHour.toInt() && currentMinute <= endMinute.toInt()){
      adaKuliah();  
    } else if(currentHour > startHour.toInt() && currentHour <= endHour.toInt()){
      adaKuliah();  
    }
    
    Serial.print("Check State = ");
    Serial.println(checkState);

    while(checkState == true && login == false){
      int dosenId = getFingerprintIDez();
      delay(50);
      Serial.print("Id = ");
      Serial.println(dosenId);
      if(dosenId == nowId.toInt()){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Berhasil");
        delay(1500);
        checkState == false;
        login = true;
        loginCount = 0;
        // break;
      }
    }

    Serial.print("Login = ");
    Serial.println(login);
    Serial.print("Finish = ");
    Serial.println(finishKuliah);
    uint8_t led = ((millis() % 1000) < 500);
    uint8_t darurat = key_led(led);
    Serial.print("D0 = ");
    Serial.println(darurat);

    // Register Function
    if(digitalRead(regisBtn) == LOW && regisState == false){
      regisState = true;
      lcd.setCursor(0,1);
      lcd.print("Registrasi !!!");
    } else if(digitalRead(regisBtn) == HIGH && regisState == false){
      regisState = false;  
    } else if(digitalRead(regisBtn) == HIGH && regisState == true){
      regisState = true;  
    } else if(digitalRead(regisBtn) == LOW && regisState == true){
      regisState = false;  
    }

    Serial.print("Regis State = ");
    Serial.println(regisState);

    while(regisState == true){
        id = readnumber();
        id++;
        Serial.print("Enrolling ID #");
        Serial.println(id);
        json.set("id", id);
        
        while (!  getFingerprintEnroll() );
    }

    while(login == true){
      DateTime now = rtc.now();
      int currentYear = now.year();
      int currentMonth = now.month();
      int currentDate = now.day();
      String today = daysOfTheWeek[now.dayOfTheWeek()];
      int currentHour = now.hour();
      int currentMinute = now.minute();
      int currentSecond = now.second();
      String stringMinute = String(currentMinute);
      String stringSecond = String(currentSecond);
      String stringHour = String(currentHour);
    
      if(currentMinute < 10){
        stringMinute = "0" + stringMinute;
      }
      if(currentSecond < 10){
        stringSecond = "0" + stringSecond;
      }
      if(currentHour < 10){
        stringHour = "0" + stringHour;  
      }

      String currentTime = today + " " + stringHour + ":" + stringMinute + ":" + stringSecond;

      
      Serial.print("Time : "); Serial.print(currentHour); Serial.print(":"); Serial.print(currentMinute); Serial.print(":"); Serial.println(currentSecond);  
      Serial.print("Start : "); Serial.print(startHour.toInt()); Serial.print(":"); Serial.println(startMinute.toInt());
      Serial.print("End : "); Serial.print(endHour.toInt()); Serial.print(":"); Serial.println(endMinute.toInt());
       if(currentHour > endHour.toInt() && currentMinute < endMinute.toInt()){
        selesaiKuliah(1);
      } else if(currentHour >= endHour.toInt() && currentMinute >= endMinute.toInt()){
        selesaiKuliah(1);
      }
      lcd.setCursor(0,0);
      lcd.print(currentTime);
      lcd.setCursor(0,1);
      lcd.print("Ada Kuliah ...");
      if(loginCount < 180){
        pintuBuka();  
      } else {
        pintuTutup();  
      }

      uint8_t led = ((millis() % 1000) < 500);
      uint8_t darurat = key_led(led);
      Serial.print("D0 = ");
      Serial.println(darurat);
      if(daruratState ==  false && darurat == 0){
         daruratState = true; 
      } else if(daruratState == true && darurat == 1){
        daruratState = true;
      }
      if(daruratState == true && daruratCount < 180){
         pintuBuka();
         daruratCount++;
      } else if(daruratCount >= 180){
        daruratState = false;
        daruratCount = 0;
        pintuTutup(); 
      }
      Serial.print("Darurat State = "); Serial.println(daruratState);
      Serial.print("Login Count = "); Serial.println(loginCount);
      Serial.print("Darurat Count = "); Serial.println(daruratCount);
      loginCount++;  
      delay(1000);
    }
    
  startFetch++;
  startTime++;
//  lcd.clear();
  delay(100);
}

void adaKuliah(){
    Serial.println("Ada Kuliah !!!");
    if(digitalRead(checkBtn) == HIGH && checkState == false){
      lcd.setCursor(0,1);
      lcd.print("Menunggu Dosen...   ");
      checkState = false;
      // Relay Mati
    } else if(digitalRead(checkBtn) == LOW && checkState == false) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Tempelkan Jari");
      delay(1500);
      checkState = true;
    }
}

void tidakAdaKuliah(){
    Serial.println("TIDAK ADA KULIAH");  
    lcd.setCursor(0,1);
    lcd.print("Tidak Ada Kuliah");
    // pintuTutup();
    if(digitalRead(checkBtn) == LOW){
      lcd.setCursor(0,1);
      lcd.print("Tidak Bisa Login");  
    }
}

void selesaiKuliah(int time){
  Serial.println("Selesai");
  for(int i = 0; i < time*60; i++){
    lcd.setCursor(0, 0);
    lcd.print("Kuliah Selesai");
    lcd.setCursor(0,1);
    lcd.print("Tinggalkan Kelas");
    pintuBuka();
    delay(1000);  
  }
  lcd.clear();
  pintuTutup();
  login = false;
  checkState = false;
  finishKuliah++;
}

void pintuBuka(){
  digitalWrite(door, HIGH);
}

void pintuTutup(){
  digitalWrite(door, LOW);  
}


/////////////////////CHECK//////////////////
  uint8_t getFingerprintID() {
    uint8_t p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println("No finger detected");
        return p;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        return p;
      default:
        Serial.println("Unknown error");
        return p;
    }
  
    // OK success! 
  
    p = finger.image2Tz();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;
      case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
        return p;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
      case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        return p;
      case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        return p;
      default:
        Serial.println("Unknown error");
        return p;
    }
    
    // OK converted!
    p = finger.fingerFastSearch();
    if (p == FINGERPRINT_OK) {
      Serial.println("Found a print match!");
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
      Serial.println("Communication error");
      return p;
    } else if (p == FINGERPRINT_NOTFOUND) {
      Serial.println("Did not find a match");
      return p;
    } else {
      Serial.println("Unknown error");
      return p;
    }   
    
    // found a match!
    Serial.print("Found ID #"); Serial.print(finger.fingerID); 
    Serial.print(" with confidence of "); Serial.println(finger.confidence); 
  
    return finger.fingerID;
  }
  
  // returns -1 if failed, otherwise returns ID #
  int getFingerprintIDez() {
    uint8_t p = finger.getImage();
    if (p != FINGERPRINT_OK)  return -1;
  
    p = finger.image2Tz();
    if (p != FINGERPRINT_OK)  return -1;
  
    p = finger.fingerFastSearch();
    if (p != FINGERPRINT_OK)  return -1;
    
    // found a match!
    Serial.print("Found ID #"); Serial.print(finger.fingerID); 
    Serial.print(" with confidence of "); Serial.println(finger.confidence);
    return finger.fingerID; 
  }


/////////////////////ENROL//////////////////

uint8_t readnumber(void) {
  uint8_t num;

  if (Firebase.getInt(firebaseData, "/" + String(deviceId) + "/count")) {
    if (firebaseData.dataType() == "int") {
      num = firebaseData.intData();
      Serial.println(firebaseData.intData());
    }
  } else {
    Serial.println(firebaseData.errorReason());
  }
  
  
  return num;
}

uint8_t getFingerprintEnroll() {
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Tempelkan Jari");
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  Serial.println("Remove finger");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Angkat Jari"); 
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again"); 
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Tempelkan Lagi !!");
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

    // OK success!
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Registrasi");
  lcd.setCursor(0,1);
  lcd.print("Sukses !!");
  delay(1500); 

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    lcd.setCursor(0,1);
    lcd.print("ID = " + id); 
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }  
    if (Firebase.setInt(firebaseData, "/" + String(deviceId) + "/count", id))
    {
      printResult(firebaseData);
    }
    if (Firebase.push(firebaseData, "/" + String(deviceId) + "/dosen", json))
    {
      lcd.clear();  
      lcd.setCursor(0,0);
      lcd.print("Ruangan M205"); 
      String idDisplay = "ID = " + String(id);
      lcd.setCursor(0,1);
      lcd.print(idDisplay);
      delay(3000);
    }
    
  regisState = false; 
}
