#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <BluetoothSerial.h>
#include <ThingSpeak.h>
#include <WiFi.h>

// Pin definitions
const int lightPins[] = {4, 16};
const int PIR_PIN = 14;    // LDR sensor pin
const int ldrPin = 12;
const int pirPin = 13;    // PIR sensor pin
const int servoPin1 = 17; // Servo for the first door
const int servoPin2 = 5;  // Servo for the second door

// Threshold values
const int darkThreshold = 100;   // LDR value for darkness
const int lightThreshold = 500;  // LDR value for bright light

// Instances
LiquidCrystal_I2C lcd(0x27, 20, 4); // LCD for displaying data
Servo servo1;                       // Servo instance for the first door
Servo servo2;                       // Servo instance for the second door
BluetoothSerial SerialBT;           // Bluetooth Serial instance

// Wi-Fi settings
const char* ssid = "netLabs!UG";
const char* password = "netLabs!UGPA55";

// ThingSpeak settings
unsigned long myChannelNumber = 2636639;
const char* myWriteAPIKey = "TNPPZMPSK0Q77SML";
WiFiClient client;

void setup() {
    Serial.begin(115200);
    lcd.init();
    lcd.backlight();

    // Initialize WiFi and connect to network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi!");

    // Initialize ThingSpeak
    ThingSpeak.begin(client);

    // Attach servos to pins
    servo1.attach(servoPin1);
    servo2.attach(servoPin2);

    // Initialize light pins
    for (int i = 0; i < 13; i++) {
        pinMode(lightPins[i], OUTPUT);
    }

    pinMode(pirPin, INPUT); // PIR sensor pin

    // Initialize Bluetooth
    if (!SerialBT.begin("smart_connect_remotecontrol")) {
        Serial.println("An error occurred initializing Bluetooth");
    } else {
        Serial.println("Bluetooth Initialized. Waiting for commands...");
    }
}

void loop() {
      // Display on LCD
    lcd.clear();
    lcd.setCursor(0, 3);
    lcd.print("Smart Connect");

    AutoDoor();
    AutoLights();
    BluetoothCommands();

    // Display on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PIR:");
    lcd.print(digitalRead(pirPin) == HIGH ? "MOTION" : "NO MOTION");

    int ldrValue = analogRead(ldrPin);
    lcd.setCursor(0, 1);
    lcd.print("LDR: ");
    lcd.print(ldrValue);


    // Send data to ThingSpeak
    ThingSpeak.setField(1, digitalRead(pirPin));
    ThingSpeak.setField(2, ldrValue);
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (x == 200) {
        Serial.println("Data sent to ThingSpeak successfully!");
    } else {
        Serial.println("Problem sending data. HTTP error code: " + String(x));
    }
        delay(2000); // Short delay to stabilize sensor readings

}

void AutoDoor() {
    int pirStatus = digitalRead(pirPin);
    
    // Display AutoDoor actions on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    if (pirStatus == HIGH) {
        Serial.println("Motion Detected");
        servo1.write(180);   // Open the first door
        servo2.write(-180);   // Open the second door
        delay(3000);         // Wait for the doors to fully open
        lcd.print("Doors OPEN ");
    } else {
        Serial.println("No motion detected");
         servo1.write(0);     // Close the first door
        servo2.write(0);     // Close the second door
        lcd.print("Doors CLOSED");
    }
    delay(2000); // Brief pause to display door status
}

void AutoLights() {
    int ldrValue = analogRead(ldrPin);
    
    // Display AutoLights actions on LCD
    lcd.clear();
    lcd.setCursor(0, 1);
    if (digitalRead(PIR_PIN) == HIGH && ldrValue < darkThreshold) {
        Serial.print("LDR Value: ");
        Serial.print(ldrValue);
        Serial.println(" - Darkness");
        for (int i = 0; i < 13; i++) {
            digitalWrite(lightPins[i], HIGH); // Turn on all lights if it's dark
        }
        lcd.print("Lights ON");
    } else if (ldrValue > lightThreshold) {
        Serial.print("LDR Value: ");
        Serial.print(ldrValue);
        Serial.println(" - Bright Light");
        for (int i = 0; i < 13; i++) {
            digitalWrite(lightPins[i], LOW);  // Turn off all lights if it's bright
        }
        lcd.print("Lights OFF");
    } else {
        Serial.print("LDR Value: ");
        Serial.print(ldrValue);
        Serial.println(" - Moderate Light");
        lcd.print("Moderate Light");
    }
    delay(2000); // Allow the display to be readable
}

void BluetoothCommands() {
    if (SerialBT.available()) {
        String command = SerialBT.readString();
        command.trim(); // Remove any trailing newline characters

        // Display Bluetooth commands on LCD
        lcd.clear();
        lcd.setCursor(0, 2);
        lcd.print("BT Cmd: ");
        lcd.print(command); // Show received Bluetooth command

        if (command.startsWith("L")) {
            int lightNumber = command.substring(1).toInt();
            if (lightNumber >= 1 && lightNumber <= 13) {
                digitalWrite(lightPins[lightNumber - 1], HIGH); // Turn on the specific light
                Serial.print("Light ");
                Serial.print(lightNumber);
                Serial.println(" ON");
                lcd.setCursor(0, 2);
                lcd.print("Light ");
                lcd.print(lightNumber);
                lcd.print(" ON ");
            }
        } else if (command.startsWith("OFF")) {
            int lightNumber = command.substring(3).toInt();
            if (lightNumber >= 1 && lightNumber <= 13) {
                digitalWrite(lightPins[lightNumber - 1], LOW); // Turn off the specific light
                Serial.print("Light ");
                Serial.print(lightNumber);
                Serial.println(" OFF");
                lcd.setCursor(0, 2);
                lcd.print("Light ");
                lcd.print(lightNumber);
                lcd.print(" OFF");
            }
        } else if (command == "L") {
            for (int i = 0; i < 13; i++) {
                digitalWrite(lightPins[i], HIGH); // Turn on all lights
            }
            Serial.println("All Lights ON");
            lcd.setCursor(0, 2);
            lcd.print("All Lights ON");
        } else if (command == "0") {
            for (int i = 0; i < 13; i++) {
                digitalWrite(lightPins[i], LOW); // Turn off all lights
            }
            Serial.println("All Lights OFF");
            lcd.setCursor(0, 2);
            lcd.print("All Lights OFF");
        } else if (command == "O") {
            servo1.write(180);
            servo2.write(180);
            Serial.println("Doors OPEN");
            lcd.setCursor(0, 2);
            lcd.print("Doors OPEN");
        } else if (command == "C") {
            servo1.write(0);
            servo2.write(0);
            Serial.println("Doors CLOSED");
            lcd.setCursor(0, 2);
            lcd.print("Doors CLOSED");
        }
        delay(2000); // Allow the display to show the command for a short time
    }
}
