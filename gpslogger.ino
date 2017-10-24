#include <NMEAGPS.h>
#include <GPSport.h>
#include <Streamers.h>
#include <LiquidCrystal.h>

#define RXPIN 1
#define TXPIN 0


static NMEAGPS  gps;
static gps_fix  fix;

static int timezone = +8; // -x or +x 

double latitude = 0;
double longitude = 0;
//String localDate = "None"; // Canceled 
String localTime = "None";
String altitude = "None";
String heading = "None";
String speed = "None";

int switchPin = 7;
int pageNo = 0;
int prevstate = HIGH;
int currstate;

//bool GPSReady;

LiquidCrystal lcd(15, 14, 5, 4, 3, 2);

static void GPSloop()
{
  if (gps.available( gpsPort )) {
    Serial.println("GPS Runging..." );
    
    fix = gps.read();

    if (fix.valid.time) {

      //localDate = getLocalDate( fix.dateTime.year , fix.dateTime.month , fix.dateTime.day , fix.dateTime.hours );
      localTime = getLocalTime( fix.dateTime.hours , fix.dateTime.minutes , fix.dateTime.seconds );

      //Serial.println("Date:"+ localDate );
      Serial.println("Time:" + localTime );
    }

    if (fix.valid.location) {
      latitude = fix.latitudeL() / 10000000.0f;
      longitude = fix.longitudeL() / 10000000.0f ;

      Serial.print( "location : " );
      Serial.print( latitude );
      Serial.print( ',' );
      Serial.println( longitude );
    }

    if ( fix.valid.altitude ) {
      altitude = fix.altitude();
      Serial.print( "Height : " );
      Serial.println( altitude );
    }

    if ( fix.valid.speed ) {
      speed = fix.speed_kph();
      Serial.print( "Speed(km/h) : " );
      Serial.println( speed );
    }

    if ( fix.valid.heading ) {
      heading = fix.heading();
      Serial.print( "Heading : " );
      Serial.println( heading );
    }

    //TODO : log to SD card

    Serial.println( "===============================================" );

  }
  LCDdisplay();
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


static void LCDdisplay() {
  switch (pageNo) {
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
  }
}

void setup()
{
  pinMode(switchPin, INPUT);
  digitalWrite(switchPin, HIGH);

  lcd.begin(16, 2);
  lcd.print("GPS Wating...");
  DEBUG_PORT.begin(9600);
  gpsPort.begin( 9600 );
  lcd.clear();
  LCDdisplay();
}

void loop()
{
  currstate = digitalRead(switchPin);

  if ((currstate != prevstate) && (currstate == HIGH)) {
    pageNo += 1;
    if (pageNo >= 4) {
      pageNo = 0;
    }
    
    lcd.clear();
    LCDdisplay();
  
  }

  if( (millis() % 1000 == 0) && (pageNo == 3) ){
    lcd.clear();
    LCDdisplay();
  }

  GPSloop();
  prevstate = currstate;
}
