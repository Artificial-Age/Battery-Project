#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO


Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_ADS1115 ads1115;

const int analongInPin = A3; //this is the pin to collect the voltage from the voltage divider to measure the battery voltage = ~4.1V

int Divider = 0;

const int numReadings = 30;

int readings[numReadings];  // the readings from the analog input
int readIndex = 0;          // the index of the current reading
int total = 0;              // the running total
int average = 0;            // the average

int inputPin = A3;


// constants won't change. They're used here to set pin numbers:
const int buttonPin = 8;     // the number of the pushbutton pin
const int OLEDpin= 10;     // OLED Power control pin

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status


//Battery Gauge bitmap

const unsigned char full_battery [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 
	0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x0f, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 
	0x0f, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0x1f, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 
	0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 
	0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 
	0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 
	0xfc, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xfc, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 
	0xfc, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xfc, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 
	0xfc, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xfc, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 
	0xfc, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xfc, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 
	0xff, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 
	0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 
	0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 
	0x3f, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0x0f, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 
	0x0f, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 
	0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 
	0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


 
void setup(void)
{


   // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(OLEDpin, OUTPUT);
  digitalWrite(OLEDpin, HIGH);

  // intilize the OLED screen

  delay(250); // wait for the OLED to power up
  display.begin(i2c_Address, true); // Address 0x3C default
  
 //display.setContrast (0); // dim display

  ads1115.setGain(GAIN_SIXTEEN); // 16x gain  +/- 0.256V  1 bit = 0.125mV
  Serial.begin(9600);
  Serial.println("Hello!");
  
  Serial.println("Getting differential reading from AIN0 (P) and AIN1 (N)");
  Serial.println("ADC Range: +/- 256mv (1 bit = 3mV)");
  ads1115.begin();
 
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }



}

void loop(void)

{

   // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);  

  int16_t results;

   // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(inputPin);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  // send it to the computer as ASCII digits
  Serial.println(average);
  delay(1000);  // delay in between reads for stability

  Divider = analogRead(inputPin); // voltage from voltage divider input into analog pin 3 (A3)

  results = ads1115.readADC_Differential_0_1(); //This is the differential of pins 0 and 1 on the ads1115 analong to digital converter 

 //int numReads = 100; //number of samples      //Smoothing samle size
 //int SensorReadings = 0; //sum of sensor readings //
 //for(int k = 0; k < numReads; k++){  //Loop to make k equal to numReads
 // SensorReadings += results; // loop while SensorReadings is great
  //delay(1);

 //}
//int senseAve = SensorReadings / numReads; //Average Sensor Readings

  // int numReads2 = 10; //number of samples
 //int SensorReadings2 = 0; //sum of sensor readings 
// for(int k2 = 0; k2 < numReads2; k2++){
 // SensorReadings2 += senseAve;
// delay(1);

 //}
 
float Battery = average * 0.01532; // Declare battery voltage as voltage divider * correction calibration
unsigned long msec = millis(); // Store millis as variable to use to count
int secs = msec/1000.0; // Convert milliseconds to seconds. 1000 milliseconds = 1 second
//float senseAve2 = SensorReadings2 / numReads2; //Average of Average
float amps = results * .00528; // convert the average amp sensor reading and multiply by correction factor calibrated by bench supply
float ampSecs = amps * secs; // convert amps to amp hours 
float ampHours = ampSecs/3600;
float watts = Battery * amps;
float wattSeconds = watts*secs;
float wattHours = wattSeconds/3600;


//this section is created to show variables on the OLED screen

     // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
     buttonState = digitalRead(buttonPin);
     

    display.clearDisplay();
    display.drawBitmap(60, 0, full_battery, 64,64, 1);
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.print(Battery);
    display.print(" V");
    display.print(buttonState);
    display.setCursor(0,30);
    display.print(wattHours);
    display.print( "wh");
    display.setCursor(0,50);
    display.print(amps);
    display.print(" Amps");
    display.display(); 
 
//Serial.println(senseAve2); 

Serial.print(amps,4);

Serial.print("amps  ");

Serial.print(Divider);

Serial.print("(");Serial.print(Battery, 4);Serial.print("v");Serial.print(")  ");

Serial.print(watts);

Serial.print("w  ");

Serial.print(wattHours, 4 ); Serial.print("wh " );

Serial.println(secs);

Serial.print(buttonState);

delay(100);
 
}