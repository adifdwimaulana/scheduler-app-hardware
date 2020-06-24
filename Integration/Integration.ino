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

//Define Firebase Data object
FirebaseData firebaseData;
SoftwareSerial mySerial(0, 2); //D3, D4

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

const int checkBtn = 14; //D5 //Pencet == LOW
const int regisBtn = 12; //D6 //Pencet == LOW
bool checkState = false;
bool regisState = false;


// void printResult(FirebaseData &data);
String jsonStr;
//String path = "/1/matkul/selasa";
String path;
const int deviceId = 1;

char daysOfTheWeek[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};

size_t len;

// Global vars Data Jadwal dan Dosen
String aNamaMatkul, aDosen, aId, aStart, aEnd, aRuangan;
String bNamaMatkul, bDosen, bId, bStart, bEnd, bRuangan;
String cNamaMatkul, cDosen, cId, cStart, cEnd, cRuangan;
int matkulCount;

String aDosenId, aDosenNama, aDosenNip;
String bDosenId, bDosenNama, bDosenNip;
String cDosenId, cDosenNama, cDosenNip;

int id;
int startFetch = 3000;
int startTime = 50;
int blinkState = 0;

void setup()
{
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
   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
//   rtc.adjust(DateTime(2020, 6, 11, 21, 18, 45));

  lcd.begin();

  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print("Scheduler App");
  delay(1000);
  lcd.setCursor(0,0);
  lcd.print("Ruangan JJ208");
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


//    String jsonStr = "";
//
//    FirebaseJson json1;
//
//    FirebaseJsonData jsonObj;
//      json.set("id", 200);
//
//    json1.set("Hi/myInt", 200);
//    json1.set("Hi/myDouble", 0.0023);
//    json1.set("Who/are/[0]", "you");
//    json1.set("Who/are/[1]", "they");
//    json1.set("Who/is/[0]", "she");
//    json1.set("Who/is/[1]", "he");
//    json1.set("This/is/[0]", false);
//    json1.set("This/is/[1]", true);
//    json1.set("This/is/[2]", "my house");
//    json1.set("This/is/[3]/my", "world");
//
//    Serial.println("------------------------------------");
//    Serial.println("JSON Data");
//    json1.toString(jsonStr, true);
//    Serial.println(jsonStr);
//    Serial.println("------------------------------------");
//
//    Serial.println("------------------------------------");
//    Serial.println("Set JSON test...");
//
//    if (Firebase.set(firebaseData, path, json1))
//    {
//        Serial.println("PASSED");
//        Serial.println("PATH: " + firebaseData.dataPath());
//        Serial.println("TYPE: " + firebaseData.dataType());
//        Serial.print("VALUE: ");
//        printResult(firebaseData);
//        Serial.println("------------------------------------");
//        Serial.println();
//    }
//    else
//    {
//        Serial.println("FAILED");
//        Serial.println("REASON: " + firebaseData.errorReason());
//        Serial.println("------------------------------------");
//        Serial.println();
//    }

    id = readnumber();
    Serial.print("Counter = ");
    Serial.println(id);

    Serial.println("------------------------------------");
    Serial.println("Get JSON test...");

    pinMode(checkBtn, INPUT_PULLUP);
    pinMode(regisBtn, INPUT_PULLUP);
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
            if(len <= 8){
              if(i == 2){
                aId = value;
              } else if(i == 3){
                aDosen = value;
              } else if(i == 4){
                aEnd = value;
              } else if(i == 5){
                aNamaMatkul = value;
              } else if(i == 6){
                aRuangan = value;
              } else if(i == 7){
                aStart = value;
              }  
            } else if(len > 8 && len <= 16){
              if(i-8 == 2){
                bId = value;
              } else if(i-8 == 3){
                bDosen = value;
              } else if(i-8 == 4){
                bEnd = value;
              } else if(i-8 == 5){
                bNamaMatkul = value;
              } else if(i-8 == 6){
                bRuangan = value;
              } else if(i-8 == 7){
                bStart = value;
              } 
            } else if(len > 16 && len <= 24){
              if(i-16 == 2){
                cId = value;
              } else if(i-16 == 3){
                cDosen = value;
              } else if(i-16 == 4){
                cEnd = value;
              } else if(i-16 == 5){
                cNamaMatkul = value;
              } else if(i-16 == 6){
                cRuangan = value;
              } else if(i-16 == 7){
                cStart = value;
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

void loop(){  
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
  String currentTime = stringHour + ":" + stringMinute + ":" + stringSecond;
  Serial.println(currentTime);
  Serial.print("Fetch = ");
  Serial.println(startFetch);
  Serial.print("Time = ");
  Serial.println(startTime);
  Serial.print("Blink = ");
  Serial.println(blinkState);

//  lcd.setCursor(0,0);
//  lcd.print(today);
//  lcd.print(" ");
//  lcd.print(currentTime);
  if(startTime % 10 == 0){
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
  if(startFetch % 3000 == 0){
   if (Firebase.get(firebaseData, path)){
        if (firebaseData.dataType() == "json")
        {
            jsonStr = firebaseData.jsonString();
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
  }
   

      String startHour;
      String startMinute;
      String endHour;
      String endMinute;
      String currentMatkul;
      String currentDosenId;
      
    if(matkulCount <= 6){
      startHour = getValue(aStart, ':', 0);
      startMinute = getValue(aStart, ':', 1);
      endHour = getValue(aEnd, ':', 0);
      endMinute = getValue(aEnd, ':', 1);
      currentMatkul = aNamaMatkul;
      currentDosenId = aId;
    } else if(matkulCount > 6 && matkulCount <= 12){
      startHour = getValue(bStart, ':', 0);
      startMinute = getValue(bStart, ':', 1);
      endHour = getValue(bEnd, ':', 0);
      endMinute = getValue(bEnd, ':', 1);  
      currentMatkul = bNamaMatkul;
      currentDosenId = bId;
    } else if(matkulCount > 12 && matkulCount <= 18){
      startHour = getValue(cStart, ':', 0);
      startMinute = getValue(cStart, ':', 1);
      endHour = getValue(cEnd, ':', 0);
      endMinute = getValue(cEnd, ':', 1);
      currentMatkul = cNamaMatkul;      
      currentDosenId = cId;  
    }
    
    bool isAdaDosen = false;
    if(currentHour >= startHour.toInt() && (currentHour <= endHour.toInt() && currentMinute <= endMinute.toInt())){
      Serial.print("Mata Kuliah: ");
      Serial.println(aNamaMatkul);
      Serial.print("Dosen: ");
      Serial.println(aDosen);    
      Serial.print("Start = ");
      Serial.println(aStart);
      Serial.print("End Hour = ");
      Serial.println(aEnd);
      if(digitalRead(checkBtn) == HIGH && checkState == false){
        lcd.setCursor(0,1);
        lcd.print("Menunggu Dosen...   ");
        checkState = false;
        // Relay Mati
      } else if(digitalRead(checkBtn) == LOW && checkState == false) {
        // Button Pencet
        lcd.setCursor(0,1);
        lcd.print("Tempelkan Jari...   ");
        checkState = true;
      } else if(digitalRead(checkBtn) == HIGH && checkState == true){
        lcd.setCursor(0,1);
        lcd.print("Tempelkan Jari Lagi ");
        checkState = true;  
      } else if(digitalRead(checkBtn) == LOW && checkState == true){
        lcd.setCursor(0,1);
        lcd.print("Menunggu Dosen...   ");
        checkState = false;  
      }
    } else {
      Serial.println("TIDAK ADA KULIAH");  
      lcd.setCursor(0,1);
      lcd.print("Menunggu Kuliah ");
      if(digitalRead(checkBtn) == LOW){
        lcd.setCursor(0,1);
        lcd.print("Tidak Bisa Login");  
      }
    }
    Serial.print("Check State = ");
    Serial.println(checkState);

    while(checkState == true){
      int dosenId = getFingerprintIDez();
      delay(50);
      Serial.print("Id = ");
      Serial.println(dosenId);
      if(dosenId == currentDosenId.toInt()){
        lcd.setCursor(1, 0);
        lcd.print("Berhasil Login");
        // Relay Menyala


        checkState == false;
        break;
      }
    }

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

    if(regisState == true){
      lcd.setCursor(0,1);
      lcd.print("Tempelkan Jari !!");  
      delay(2000); 
    }

    while(regisState == true){
        id = readnumber();
        id++;
        Serial.print("Enrolling ID #");
        Serial.println(id);
        json.set("id", id);
        
        
        while (!  getFingerprintEnroll() );
    }
    
  startFetch++;
  startTime++;
//  lcd.clear();
  delay(100);
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
  lcd.setCursor(0,1);
  lcd.print("Berhasil"); 
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  lcd.setCursor(0,1);
  lcd.print("Tempelkan Lagi !!"); 
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
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
  lcd.setCursor(0,1);
  lcd.print("Angkat !!"); 

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
      lcd.print("Ruangan JJ208"); 
      lcd.setCursor(0,1);
      lcd.print("ID = " + id);
      delay(3000);
    }
    
    
  regisState = false; 
}
