#include <WiFi.h>
#include <DHT.h>
#include <ThingSpeak.h>
#include <LiquidCrystal_I2C.h>

#define RELAY_FAN_PIN 33   // GPIO pin connected to relay (adjust as needed for ESP32)
#define DHTPIN 27           // GPIO pin connected to DHT sensor (adjust as needed for ESP32)
#define DHTTYPE DHT11      // DHT11 sensor type

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int TEMP_THRESHOLD_UPPER = 26; // Upper threshold of temperature

DHT dht(DHTPIN, DHTTYPE);

float temp;    // Temperature in Celsius

// Wi-Fi settings
const char* ssid = "netLabs!UG"; // Replace with your Wi-Fi SSID
const char* password = "netLabs!UGPA55"; // Replace with your Wi-Fi password

// ThingSpeak settings
unsigned long myChannelNumber = 2619528; // Replace with your ThingSpeak Channel ID
const char * myWriteAPIKey = "LRYM2G8MTW2YNQ17"; // Replace with your ThingSpeak Write API Key
WiFiClient client;

void setup() {
  Serial.begin(115200); // Initialize serial
  dht.begin();          // Initialize the DHT sensor
  pinMode(RELAY_FAN_PIN, OUTPUT); // Initialize relay pin as an output

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);

  lcd.init(); // Initialize the LCD
 lcd.backlight(); // Turn on the backlight if your LCD has one

  // Print the message on the LCD
    lcd.print("State-of-the-fan");

}

 


void loop() {
  // Wait a few seconds between measurements
  delay(2000);

  // Clear the LCD to avoid clutter
    lcd.clear();

  float hum = dht.readHumidity();
  temp = dht.readTemperature();

  // Print temp and humidity values to serial monitor
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print(" %, Temp: ");
  Serial.print(temp);
  Serial.println(" Celsius");

  // LCD 1: Display temperature, humidity, and fan
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temp);
    lcd.print("C");
    lcd.setCursor(0, 1);
     // Write the number of seconds elapsed
    lcd.print(millis() / 1000);
    lcd.print(" SECONDS");
    
  if (isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
  } else if (temp > TEMP_THRESHOLD_UPPER) {
    Serial.println("Temperature is above 26°C. Turning on the fan.");
    digitalWrite(RELAY_FAN_PIN, HIGH); // Turn on the fan
  } else {
    Serial.println("Temperature is below 26°C. Turning off the fan.");
    digitalWrite(RELAY_FAN_PIN, LOW); // Turn off the fan
  }

  // Send data to ThingSpeak
  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, hum);
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Data sent to ThingSpeak successfully!");
  } else {
    Serial.println("Problem sending data. HTTP error code: " + String(x));
  }
}

