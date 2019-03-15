/*
  Test BMX055

  Test progam for Bosch BMX055, connected to Wire1 (I2C).
*/

#include <Wire.h>
#include <senseBoxIO.h>
#include <SD.h>
#include <SPI.h>
#include <SenseBoxMCU.h>
#include "DS1307.h"
#include "Adafruit_MQTT.h"        // Adafruit.io MQTT library
#include "Adafruit_MQTT_Client.h" // Adafruit.io MQTT library

DS1307 clock;//define a object of DS1307 class

float accRange = 20.0/2048.0; // depends on range set
float accelZ;

float XA, YA, ZA; // accelerometer axis
Bee* b = new Bee();

// Adafruit MQTT

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "KARLKARL"
#define AIO_KEY         "32c0fe2c3a614ff08157e08e3cfad7d0"
#define AIO_ZA          "Final1"

WiFiClient client;

// create the objects for Adafruit IO
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish ZA_feed = Adafruit_MQTT_Publish(&mqtt, "KARLKARL/feeds/Final1");

GPS gps;
float lat; //Geografische Breite
float lng; //Geografische Länge
float alt; //Höhe über Meeresspiegel in Metern
float speed;

void read(byte addr, byte reg, byte *data, byte len)
{
  Wire1.beginTransmission(addr); // start transmission
  Wire1.write(reg);              // write register byte
  Wire1.endTransmission();       // stop transmission
  Wire1.requestFrom(addr, len);  // request x bytes
  while(Wire1.available() == 0); // wait for data bytes
  delay(1); // wait 1ms
  for(byte i = 0; i < len; i++)
  {
    *data++ = Wire1.read();      // read data byte
  }
}

void write(byte addr, byte data1, byte data2)
{
  Wire1.beginTransmission(addr); // start transmission
  Wire1.write(data1);            // write 1st data byte
  Wire1.write(data2);            // write 2nd data byte
  Wire1.endTransmission();       // stop transmission
}

void setup(){
    b->connectToWifi("UPC4941791","k4hpxWeNtxnu");
  delay(1000);
  
  // init serial library
  Serial.begin(9600);
  //while(!Serial); // wait for serial monitor
  Serial.println("Test BMX055");


  // microSD in XBEE2 Socket
  senseBoxIO.powerXB2(false); // power off to reset microSD
  delay(250);
  senseBoxIO.powerXB2(true);  // power on

  // init card
  const int chipSelect = PIN_XB2_CS;
if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  // init I2C/Wire library
  Wire1.begin();

  // init BMX055
  write(I2C_ACCEL, 0x0F, 0x03);  // PMU_RANGE=0x03 +/-2g
  write(I2C_ACCEL, 0x10, 0x08);  // PMU_BW=0x08    7.81Hz
  write(I2C_ACCEL, 0x11, 0x00);  // PMU_LPW=0x00   normal, sleep 0.5ms
  delay(500); // wait 500ms
  gps.begin();

  clock.begin();
  //clock.fillByYMD(2013,1,19);//Jan 19,2013
  //clock.fillByHMS(15,28,30);//15:28 30"
  //clock.fillDayOfWeek(SAT);//Saturday
  //clock.setTime();//write time to the RTC chip
}

void loop()
{
  byte data[6];   // receive buffer
  lat = gps.getLatitude();
  lng = gps.getLongitude();
  alt = gps.getAltitude();
  speed = gps.getSpeed();

  clock.getTime();
  Serial.print(clock.hour, DEC);
  Serial.print(":");
  Serial.print(clock.minute, DEC);
  Serial.print(":");
  Serial.print(clock.second, DEC);
  Serial.print("  ");
  Serial.print(clock.month, DEC);
  Serial.print("/");
  Serial.print(clock.dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(clock.year+2000, DEC);
  Serial.print(" ");
  Serial.print(clock.dayOfMonth);
  Serial.print("*");
  switch (clock.dayOfWeek)// Friendly printout the weekday
  {
    case MON:
      Serial.print("MON");
      break;
    case TUE:
      Serial.print("TUE");
      break;
    case WED:
      Serial.print("WED");
      break;
    case THU:
      Serial.print("THU");
      break;
    case FRI:
      Serial.print("FRI");
      break;
    case SAT:
      Serial.print("SAT");
      break;
    case SUN:
      Serial.print("SUN");
      break;
  }
  Serial.println(" ");
  
  // read accelerometer data and convert them
  read(I2C_ACCEL, 0x02, data, 6);

  XA = (data[1]<<4) | ((data[0]&0xF0)>>4); // 12 bit
  if(XA > 2047){ XA -= 4096; }
  YA = (data[3]<<4) | ((data[2]&0xF0)>>4); // 12 bit
  if(YA > 2047){ YA -= 4096; }
  ZA = (data[5]<<4) | ((data[4]&0xF0)>>4); // 12 bit
  if(ZA > 2047){ ZA -= 4096; }

  // output data to serial monitor
  Serial.print("Accel  X: ");
  Serial.println(XA);
  Serial.print("Accel  Y: ");
  Serial.println(YA);
  Serial.print("Accel  Z: ");
  Serial.println(ZA);
  Serial.print(lat,6);
  Serial.print(F(","));
  Serial.print(lng,6);
  Serial.print(F(","));
  Serial.println(alt,1);
  Serial.print(F(","));
  Serial.println(speed,4);
  delay(100);
   
  File dataFile = SD.open("Final1.csv", FILE_WRITE);
    
  if (dataFile) {
    // print to the serial port too:
    dataFile.print(millis());
    dataFile.print(",");
    dataFile.print(XA); //dataFile.print(String(XA));
    dataFile.print(",");
    dataFile.print(YA);
    dataFile.print(",");
    dataFile.print(ZA);
    dataFile.print(",");
    dataFile.print(lat,6);
    dataFile.print(",");
    dataFile.print(lng,6);
    dataFile.print(",");
    dataFile.print(alt,1);
    dataFile.print(",");
    dataFile.print(speed,4);
    dataFile.print(",");
    dataFile.print(clock.hour, DEC);
    dataFile.print(":");
    dataFile.print(clock.minute, DEC);
    dataFile.print(":");
    dataFile.print(clock.second, DEC);
    dataFile.print("  ");
    dataFile.print(clock.month, DEC);
    dataFile.print("/");
    dataFile.print(clock.dayOfMonth, DEC);
    dataFile.print("/");
    dataFile.print(clock.year+2000, DEC);
    dataFile.println();
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

  accelZ = (float)ZA*accRange;
  delay(1000); // wait 1000ms
  
  MQTT_connect();
        ZA_feed.publish(accelZ);
        Serial.println(accelZ);
  delay(5000);
}  
// define functions
  
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println(ret);
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
