#include "Keypad.h"
#include "Adafruit_GPS.h"
#include "Adafruit_Sensor.h"
#include "math.h"
#include "MPU9250.h"

 #define mySerial Serial1
Adafruit_GPS GPS(&mySerial);
// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO  true
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
  // in case you are not using the interrupt above, you'll
    // need to 'hand query' the GPS, not suggested :(
    if (! usingInterrupt) {
        char c = GPS.read();    // read data from the GPS in the 'main loop'
        if (GPSECHO)            // if you want to debug, this is a good time to do it!
            if (c) Serial.print(c);
    }
    // if a sentence is received, we can check the checksum, parse it...
    if (GPS.newNMEAreceived()) {
        //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
        if (!GPS.parse(GPS.lastNMEA()))   {
          // this also sets the newNMEAreceived() flag to false
          if (millis() - timer > 10000) {
            // Spark.publish("GPS", "{ last: \""+String(GPS.lastNMEA())+"\"}", 60, PRIVATE );
            // Spark.publish("GPS", "{ error: \"failed to parse\"}", 60, PRIVATE );
            
            //get location here
            //mySerial.println(GPS.read());
            Serial.print("Location: ");
            
            mySerial.print(GPS.latitude, 4); 
            //mySerial.print(GPS.lat);
            //mySerial.println(String(GPS.lat) + String(GPS.latitude));
            //mySerial.println(String(GPS.lon) + String(GPS.longitude));
          }
          return;  // we can fail to parse a sentence in which case we should just wait for another
        }
    }
    // if millis() or timer wraps around, we'll just reset it
    if (timer > millis())  timer = millis();  
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
