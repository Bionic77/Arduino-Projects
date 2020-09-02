/*-----( Import needed libraries )-----*/
#include <dht.h>
#include <LiquidCrystal.h>
#include <DallasTemperature.h> // this library version already includes the OneWire library


/*-----( Declare Constants and Pin Numbers )-----*/
#define DHTPIN A5
#define ONE_WIRE_BUS_PIN 5

/*-----( Declare objects )-----*/
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS_PIN);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// Setup an instance for the DHT sensor
dht DHT;

/*
  LiquidCrystal(rs, enable, d4, d5, d6, d7)
  LiquidCrystal(rs, rw, enable, d4, d5, d6, d7)
  LiquidCrystal(rs, enable, d0, d1, d2, d3, d4, d5, d6, d7)
  LiquidCrystal(rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7)

  Parameters

  rs:     Arduino pin that is connected to the RS pin on the LCD
  rw:     Arduino pin that is connected to the RW pin on the LCD (optional)
  enable: Arduino pin that is connected to the enable pin on the LCD
  d0, d1, d2, d3, d4, d5, d6, d7:
  Arduino pins that are connected to the corresponding data pins on the LCD.
  d0, d1, d2, and d3 are optional;
  if omitted,
  the LCD will be controlled using only the four data lines (d4, d5, d6, d7). */

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);




void setup() // setup the system

{
  pinMode(4, OUTPUT); //RED LED
  pinMode(3, OUTPUT);  //YELLOW LED
  pinMode(2, OUTPUT);  //GREEN LED

  digitalWrite(4, LOW);
  digitalWrite(3, LOW);
  digitalWrite(2, LOW);

  lcd.begin(20, 4);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Room Temp:");
  lcd.setCursor(16, 0);
  lcd.print((char)223);
  lcd.print("C ");
  lcd.setCursor(0, 1);
  lcd.print("Humidity :");
  lcd.setCursor(13, 1);
  lcd.print("%");
  lcd.setCursor(0, 2);
  lcd.print("DP:");
  lcd.setCursor(8, 2);
  lcd.print((char)223);
  lcd.print("C ");
  lcd.setCursor(11, 2);
  lcd.print("HI:");
  lcd.setCursor(0, 3);
  lcd.print("Tank Temp:");
  lcd.setCursor(16, 3);
  lcd.print((char)223);
  lcd.print("C ");

  sensors.begin(); //Start up the DallasTemperature library 
  delay(1000); //Let sensors initialize

 Serial.begin(9600); // start serial port and comms
}

void loop()
{
  sensors.requestTemperatures(); // Send the command to get temperatures
  double celsius1(sensors.getTempCByIndex(0)); // index 0 is first sensor on the OneWire bus - this case Room Sensor
  lcd.setCursor(11, 0);
  lcd.print(celsius1);

  double celsius2(sensors.getTempCByIndex(1)); // index 1 is the second sensor on the OneWire bus - this case Fish Tank Sensor
  lcd.setCursor(11, 3);
  lcd.print(celsius2);

  DHT.read11(DHTPIN); // Get Humidity data from DHT11
  lcd.setCursor(11, 1);
  lcd.print(DHT.humidity, 0);

  double DP = (dewPoint(celsius1, DHT.humidity)); // dewpoint calculation calling on the function below
  lcd.setCursor(3, 2);
  lcd.print(DP);

  double HI = (humidex(celsius1, DP)); // humidity index (humidex) calculation calling on the function below
  lcd.setCursor(14, 2);
  lcd.print(HI, 1);

  /*Range of humidex: Degree of comfort
    20 to 29: Little to no discomfort
    30 to 39: Some discomfort
    40 to 45: Great discomfort; avoid exertion
    Above 45: Dangerous; heat stroke quite possible*/


  if (HI < 30)
  {
    digitalWrite(4, LOW);
    digitalWrite(3, LOW);
    digitalWrite(2, HIGH);
  }
  else if (HI > 30.01 and HI < 40)
  {
    digitalWrite(4, LOW);
    digitalWrite(3, HIGH);
    digitalWrite(2, HIGH);
  }
    else if (HI > 40.01 and HI < 45)
  {
    digitalWrite(4, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(2, LOW);
  }
  else
  {
    digitalWrite(4, HIGH);
    digitalWrite(3, LOW);
    digitalWrite(2, LOW);
  }

// below statements for data check on serial port of laptop. can be commented out if not needed
/* */
  Serial.print(celsius1);
  Serial.print("   ");
  Serial.print(DHT.humidity);
  Serial.print("   ");
  Serial.print(DHT.temperature);
  Serial.print("   ");
  Serial.print(DP);
  Serial.print("   ");
  Serial.print(HI);
  Serial.print("   ");
  Serial.print(celsius2);
  Serial.println();
/* */

// delay to gather data every five seconds.
  
  delay(5000);
}


// dewPoint function NOAA
// reference: http://wahiduddin.net/calc/density_algorithms.htm

double dewPoint(double tempC, double Humidity)
{
  double A0 = 373.15 / (273.15 + tempC);
  double SUM = -7.90298 * (A0 - 1);
  SUM += 5.02808 * log10(A0);
  SUM += -1.3816e-7 * (pow(10, (11.344 * (1 - 1 / A0))) - 1) ;
  SUM += 8.1328e-3 * (pow(10, (-3.49149 * (A0 - 1))) - 1) ;
  SUM += log10(1013.246);
  double VP = pow(10, SUM - 3) * Humidity;
  double T = log(VP / 0.61078); // temp var
  return (241.88 * T) / (17.558 - T);
}

/*
  // delta max = 0.6544 wrt dewPoint()
  // 5x faster than dewPoint()
  // reference: http://en.wikipedia.org/wiki/Dew_point

  double dewPointFast(double tempC, double Humidity)
  {
  double a = 17.271;
  double b = 237.7;
  double temp = (a * tempC) / (b + tempC) + log(Humidity * 0.01);
  double Td = (b * temp) / (a - temp);
  return Td;
  } */

double humidex(double tempC, double DewPoint)
{
  double e = 5417.7530 * ((1 / 273.16) - (1 / (273.16 + DewPoint)));
  double h = tempC + 0.5555 * ( 6.11 *  exp (e) - 10);
  return h;
}


