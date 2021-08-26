
#include <WiFiNINA.h>
#include <SPI.h>
#include "Keypad.h"
#include "Adafruit_GPS.h"
#include "math.h"
#include "MPU9250.h"
#include "arduino_secrets.h"

#define mySerial Serial1
Adafruit_GPS GPS(&mySerial);
// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO  false
// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
byte bufferSize = 64;
byte bufferIndex = 0;
char buffer[65];
char c;
double compass = 0;
float pi = 3.14159265359;
uint32_t timer;

 const byte ROWS = 4;
 const byte COLS = 4;
 //define symbols on keypad
 char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2};  //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); 

//magenotometer values
MPU9250 mpu;

//http client setup
int status = WL_IDLE_STATUS;
IPAddress server(192,168,1,184); // server address always use the actual IP not localhost :P
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
WiFiClient client;

void setup() {
  //configure and test vibration
  //configure and test IMU and accelerometer
   Wire.begin();
   if (!mpu.setup(0x68)) {  // change to your own address
        while (1) {
            Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
            delay(5000);
        }
    }
  //configure GPS
  GPS.begin(9600);
  mySerial.begin(9600);
  Serial.begin(115200);
    // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
    // print it out we don't suggest using anything higher than 1 Hz
    
    // Request updates on antenna status, comment out to keep quiet
    GPS.sendCommand(PGCMD_NOANTENNA);
    delay(1000);
    // Ask for firmware version
    //mySerial.println(PMTK_Q_RELEASE);
    timer = millis();
  //swtch on screen
  //configure LED
  //configure and test WiFi
    while (status != WL_CONNECTED) {
    //print connection attempt
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
   Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    IPAddress ip = WiFi.localIP();
    IPAddress gateway = WiFi.gatewayIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

}

void loop() {
  //keypad
   char customKey = customKeypad.getKey(); 
  if (customKey){
    Serial.println(customKey);
  }
  //fetch location data
  getLocation();
  getIMUData();
}
void getLocation(){
   // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    Serial.print(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer
    Serial.print("\nTime: ");
    if (GPS.hour < 10) { Serial.print('0'); }
    Serial.print(GPS.hour, DEC); Serial.print(':');
    if (GPS.minute < 10) { Serial.print('0'); }
    Serial.print(GPS.minute, DEC); Serial.print(':');
    if (GPS.seconds < 10) { Serial.print('0'); }
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    if (GPS.milliseconds < 10) {
      Serial.print("00");
    } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
      Serial.print("0");
    }
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
      syncData(GPS.longitude, GPS.latitude, 5);
    }
  }
}
void getIMUData(){
    // Calculate the angle of the vector y,x
     if (mpu.update()) {
      compass = (atan2(mpu.getMagY(), mpu.getMagX()) * 180) / PI;
      // Normalize to 0-360
      if (compass < 0) {
          compass += 360;
      }
      //uncomment to debug
      //Serial.println(compass);
     }
}
void controlLED(){}

void keyPadControl(){
  // put your main code here, to run repeatedly:
 
}
void screenControl(){}
void controlVibration(){}
/*sends location cordinates, range and other data to ONLINE*/
void syncData(float longitude, float latitude, float range){
  Serial.println("Generate JSON from values");
  String postData = "{\"longitude\": "+String(longitude)+", \"latitude\": "+String(latitude)+", \"range\": "+String(range)+"}";
  Serial.println(postData);
  
  if(client.connect(server, 3000)){
    client.println("POST /location-details HTTP/1.1");
    client.println("Host: 127.0.0.1");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(postData.length());
    client.println();
    client.print(postData);
   }
   if(client.connected()){
    client.stop(); 
   }
   delay(3000);
   Serial.println();
   Serial.println("closing connection");
}
