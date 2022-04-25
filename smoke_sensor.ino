#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(D6, D5, D1, D2, D3, D4);
//#include <ArduinoJson.h>
#include <DHT.h>



#define HOSTIFTTT "maker.ifttt.com"
#define EVENTO "FireAlert"
#define IFTTTKEY "c8KXioEFtzMHhxKNOJY3lN"
WiFiClient client;

const char* ssid = "IOT31";
const char* password = "password";

#define DHTPIN 13 //d7  
int buzzer =  15; //d8
int smokeA0 = A0; //A0
int sensorThres = 10;

#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors
DHT dht(DHTPIN, DHTTYPE);
void setup() {
  lcd.begin(16, 2);                 // Initialize 16x2 LCD Display
  lcd.clear();
  lcd.setCursor(0, 0);

  dht.begin();

  Serial.begin(115200);
  delay(4);
  pinMode(buzzer, OUTPUT);
  pinMode(smokeA0, INPUT);
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  //  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  


  //Send Webook to IFTTT
  //  send_webhook(IFTTTEVENT,IFTTTKEY)
}

bool check = true;

int timeSinceLastRead = 0;
void loop() {

  //DHT CODE
  // Report every 2 seconds.
  if (timeSinceLastRead > 200) {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(t);
    lcd.setCursor(0, 1);
    lcd.print("H:");
    lcd.print(h);
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      timeSinceLastRead = 0;
      return;
    }

    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");
    Serial.print(hif);
    Serial.println(" *F");

    timeSinceLastRead = 0;
  }
  delay(100);
  timeSinceLastRead += 100;
  //END

  //Smoke Sensor code
  int analogSensor = analogRead(smokeA0);
  Serial.print("Pin A0: ");
  Serial.println(analogSensor);
  digitalWrite(buzzer, LOW) ;
  // END
  // Checks if it has reached the threshold value
  if (analogSensor > sensorThres)
  {
   digitalWrite(buzzer, HIGH) ;
    if (check == true) {

      check = false;
      if (client.connected())
      {
        client.stop();
      }

      client.flush();

      if (client.connect(HOSTIFTTT, 80)) {

        lcd.setCursor(0, 0);
        lcd.print("Somke Detected: ");
        lcd.setCursor(0,1);
        lcd.print("           ");
     
        // build the HTTP request
        String toSend = "GET /trigger/";
        toSend += EVENTO;
        toSend += "/with/key/";
        toSend += IFTTTKEY;
        toSend += "?value1=";
        toSend += "Softwarica";
        toSend += " HTTP/1.1\r\n";
        toSend += "Host: ";
        toSend += HOSTIFTTT;
        toSend += "\r\n";
        toSend += "Connection: close\r\n\r\n";
        client.print(toSend);
        delay(5000);
        lcd.clear();
      }
    }
  }
  else {
    Serial.print("No smoke detected");

    check = true;
  }


  delay(5000);
}
