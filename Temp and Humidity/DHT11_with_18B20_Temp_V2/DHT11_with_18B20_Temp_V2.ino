/*-----( Import needed libraries )-----*/
#include <dht.h>
#include <LiquidCrystal.h>
#include <DallasTemperature.h> // this header already includes the OneWire header


/*-----( Declare Constants and Pin Numbers )-----*/
#define DHTPIN A0
#define ONE_WIRE_BUS_PIN 12

/*-----( Declare objects )-----*/
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS_PIN);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

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

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

dht DHT;


void setup() // setup the system
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  pinMode(10, OUTPUT); //RED
  pinMode(9, OUTPUT);  //YELLOW
  pinMode(8, OUTPUT);  //GREEN
  
  digitalWrite(10, LOW);
  digitalWrite(9, LOW);
  digitalWrite(8, LOW);
  Serial.begin(9600); // start serial port and comms
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);

  sensors.begin(); //-( Start up the DallasTemperature library )
}

void loop()
{
  delay(2000);
  sensors.requestTemperatures(); // Send the command to get temperatures
  double celsius(sensors.getTempCByIndex(0));
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.setCursor(2, 0);
  lcd.print(celsius);
  lcd.print((char)223);
  lcd.print("C ");
  
  DHT.read11(DHTPIN);
  lcd.setCursor(10, 0);
  lcd.print("RH:");
  lcd.setCursor(13, 0);
  lcd.print(DHT.humidity, 0);
  lcd.print("%");

  double DP = (dewPoint(celsius, DHT.humidity));
  lcd.setCursor(0, 1);
  lcd.print("D:");
  lcd.setCursor(2, 1);
  lcd.print(DP);
  lcd.print((char)223);
  lcd.print("C ");

  double Hx = (humidex(celsius, DP));
  lcd.setCursor(10, 1);
  lcd.print("Hx:");
  lcd.print(Hx, 0);

/*Range of humidex: Degree of comfort
20 to 29: Little to no discomfort
30 to 39: Some discomfort
40 to 45: Great discomfort; avoid exertion
Above 45: Dangerous; heat stroke quite possible*/


if (Hx < 32)
{
  digitalWrite(10, LOW);
  digitalWrite(9, LOW);
  digitalWrite(8, HIGH);
}
else if (Hx >32 and Hx <40)
{
  digitalWrite(10, LOW);
  digitalWrite(9, HIGH);
  digitalWrite(8, LOW);
}
else
{
  digitalWrite(10, HIGH);
  digitalWrite(9, LOW);
  digitalWrite(8, LOW);
}

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


