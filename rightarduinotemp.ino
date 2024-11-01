#include <DHT.h>
#include <DHT_U.h>

#define RELAY_FAN_PIN 10 // Arduino pin connected to relay which connected to fan
#define DHTPIN 5           // Arduino pin connected to relay which connected to DHT sensor
#define DHTTYPE DHT11

const int TEMP_THRESHOLD_UPPER = 20; // upper threshold of temperature, change to your desire value
//const int TEMP_THRESHOLD_LOWER = 3; // lower threshold of temperature, change to your desire value
// const int RELAY_FAN_PIN = 10;
DHT dht(DHTPIN, DHTTYPE);

float temp;    // temperature in Celsius

void setup()
{
  Serial.begin(9600); // initialize serial
  dht.begin();        // initialize the sensor
  pinMode(RELAY_FAN_PIN, OUTPUT); // initialize digital pin as an output
}

void loop()
{
  // wait a few seconds between measurements.
  delay(2000);
float hum = dht.readHumidity();
  temp= dht.readTemperature();
  
  //Print temp and humidity values to serial monitor
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print(" %, Temp: ");
  Serial.print(temp);
  Serial.println(" Celsius");
  
 // temperature = dht.readTemperature();  // read temperature in Celsius
  
  if (isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");

    Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" °C");

  } 
  else if(temp < TEMP_THRESHOLD_UPPER){
      Serial.println("The fan is turned on");
     // Serial.println("Temperature is above 26°C. Turning on the fan.");
      digitalWrite(RELAY_FAN_PIN, HIGH); // turn on
    } else{
      Serial.println("The fan is turned off");
      digitalWrite(RELAY_FAN_PIN, LOW); // turn on
    }
  }