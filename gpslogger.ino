#include <NMEAGPS.h>
#include <GPSport.h>
#include <Streamers.h>
#include <LiquidCrystal.h>

#include <SPI.h>
#include <SD.h>

// ----------Setting---------- //

static const int SDCardCSPin = 10;
static const int switchPin = A0;

static const int timezone = +8; // -x or +x
static const int totalPage = 4;

// -------------------- //

File dataLog;
bool SDCardReady = false;
unsigned long lastSaveTime = 0;
String fileName = String();
unsigned int fileNumber = 0;

static NMEAGPS  gps;
static gps_fix  fix;

double latitude = 0;
double longitude = 0;
//String localDate = "None"; // Canceled
String localTime = "None";
String altitude = "None";
String heading = "None";
String speed = "None";


int currentPageNo = 0;

int prevstate = HIGH;
int currstate;

LiquidCrystal lcd(9, 8, 5, 4, 3, 2);

static void GPSloop()
{

  //   DEBUG_PORT.println("Arduino Started!" );

  if (gps.available( gpsPort )) {
    DEBUG_PORT.println("[GPS] Runging..." );

    fix = gps.read();

    if (fix.valid.time) {

      //localDate = getLocalDate( fix.dateTime.year , fix.dateTime.month , fix.dateTime.day , fix.dateTime.hours );
      localTime = getLocalTime( fix.dateTime.hours , fix.dateTime.minutes , fix.dateTime.seconds );

      DEBUG_PORT.print("LocalTime: ");
      DEBUG_PORT.println(localTime);
    }

    if (fix.valid.location) {
      latitude = fix.latitudeL() / 10000000.0f;
      longitude = fix.longitudeL() / 10000000.0f ;

      DEBUG_PORT.print( "location : " );
      DEBUG_PORT.print( latitude );
      DEBUG_PORT.print( ',' );
      DEBUG_PORT.println( longitude );
    }

    if ( fix.valid.altitude ) {
      altitude = fix.altitude();
      DEBUG_PORT.print( "Height : " );
      DEBUG_PORT.println( altitude );
    }

    if ( fix.valid.speed ) {
      speed = fix.speed_kph();
      DEBUG_PORT.print( "Speed(km/h) : " );
      DEBUG_PORT.println( speed );
    }

    if ( fix.valid.heading ) {
      heading = fix.heading();
      DEBUG_PORT.print( "Heading : " );
      DEBUG_PORT.println( heading );
    }

    GPSlog();

    DEBUG_PORT.println( "===============================================" );

  }
  LCDdisplay();
}

void GPSlog() {


  // https://github.com/arduino/Arduino/issues/3607
  // Hot Swap

  if (!SDCardReady) {
    if ( currentPageNo == 4 ) { // Do it only on Page 4 , or it will spam and cause lag
      DEBUG_PORT.println("[SD Card] Try to Init again");
      initSDCard();
    }
    return;
  }

  dataLog = SD.open(fileName, FILE_WRITE);

  if (dataLog) {
    if (fix.status != 0) {
      char formatedTDate[8];
      sprintf (formatedTDate, "%02d-%02d-%02d", fix.dateTime.year, fix.dateTime.month, fix.dateTime.day);

      char formatedTime[8];
      sprintf (formatedTime, "%02d:%02d:%02d", fix.dateTime.hours, fix.dateTime.minutes, fix.dateTime.seconds);

      dataLog.print( formatedTDate );
      dataLog.print( "," );
      dataLog.print( formatedTime );
      dataLog.print( "," );
      dataLog.print( latitude, 7 );
      dataLog.print( "," );
      dataLog.print( longitude, 7 );
      dataLog.print( ',' );
      dataLog.print( altitude );
      dataLog.print( ',' );
      dataLog.print( speed );
      dataLog.print( ',' );
      dataLog.println( heading );

      lastSaveTime = millis() / 1000;

      DEBUG_PORT.println( "[SD Card] Log inserted to " + fileName );

    }

  } else {
    DEBUG_PORT.println( "[SD Card] Error!" );
    SDCardReady = false;
  }

  dataLog.close();

  return;
}

void createLog() {

  fileNumber = 0;

  do {
    fileNumber++;
    fileName = "gps_";
    fileName += fileNumber;
    fileName += ".txt";

  } while (SD.exists(fileName));

  dataLog = SD.open(fileName, FILE_WRITE);

  if (dataLog) {
    DEBUG_PORT.println("[SD Card] Created : " + fileName);
    dataLog.println("date,time,latitude,longitude,altitude,speed(km/h),heading");
    dataLog.close();
  }

  return;
}

void initSDCard() {

  if (!SD.begin(SDCardCSPin)) {
    DEBUG_PORT.println("[SD Card] Init failed!");
    SDCardReady = false;
  } else {
    SDCardReady = true;
    createLog();
  }
  return;
}

String getLocalTime(int hour , int minute , int second )
{
  hour += timezone;

  if (hour >= 24) {
    hour -= 24;
  }

  char buffer[9];
  sprintf(buffer, "%02d:%02d:%02d", hour, minute, second);

  return buffer;
}


String getRunningTime()
{

  int Time = millis() / 1000;

  int hour = Time / 3600;
  int minute = Time / 60 % 60;
  int second = Time % 60;

  char buffer[9];
  sprintf(buffer, "%02d:%02d:%02d", hour, minute, second);

  return buffer;
}

String getLastChangeTime()
{

  if (lastSaveTime == 0) {
    return "No Last Save";
  }

  int Time = millis() / 1000 - lastSaveTime;

  int hour = Time / 3600;
  int minute = Time / 60 % 60;
  int second = Time % 60;

  char buffer[9];
  sprintf(buffer, "%02d:%02d:%02d", hour, minute, second);

  return buffer;
}

static void LCDdisplay() {
  switch (currentPageNo) {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print(latitude , 7 );
      lcd.setCursor(0, 1);
      lcd.print(longitude, 7);
      break;
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("GPSTime:" + localTime);
      lcd.setCursor(0, 1);
      lcd.print("Height:" + altitude);
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print("Heading:" + heading);
      lcd.setCursor(0, 1);
      lcd.print("km/h:" + speed);
      break;
    case 3:
      lcd.setCursor(0, 0);
      lcd.print(getRunningTime() );
      lcd.setCursor(0, 1);
      lcd.print("Status:");
      lcd.print(fix.status );
      break;
    case 4:
      lcd.setCursor(0, 0);
      lcd.print("SDReady:");
      lcd.print(SDCardReady ? "True" : "False");
      if (SDCardReady) {
        lcd.print("-");
        lcd.print(fileNumber);
      }
      lcd.setCursor(0, 1);
      lcd.print( getLastChangeTime() );
      break;
  }
}

//

void setup()
{

  gpsPort.begin(9600);
  DEBUG_PORT.begin(19200);
  //  while (!Serial);  //failed

  DEBUG_PORT.println("[Arduino] SETUP!" );
  pinMode(switchPin, INPUT);
  digitalWrite(switchPin, HIGH);

  initSDCard();

  lcd.begin(16, 2);
  lcd.print("GPS Wating...");

  lcd.clear();
  LCDdisplay();
}

void loop()
{
  //DEBUG_PORT.println("Loop" + getRunningTime()  );

  currstate = digitalRead(switchPin);

  if ((currstate != prevstate) && (currstate == HIGH)) {
    currentPageNo += 1;
    if (currentPageNo > totalPage) {
      currentPageNo = 0;
    }

    lcd.clear();
    LCDdisplay();

  }

  if ( (millis() % 1000 == 0) && (currentPageNo == 3)) {
    lcd.clear();
    LCDdisplay();
  }

  GPSloop();
  prevstate = currstate;
}
