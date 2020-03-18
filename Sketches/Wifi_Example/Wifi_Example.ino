#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SimpleDHT.h>

enum State {
  scan,
  transmit,
  wait,  
};

State currentState = wait;

SimpleDHT11 dht11;
int pinDHT11 = 4;
const int pinLED = 2;

StaticJsonBuffer<200> jsonbuffer;
JsonObject& root = jsonbuffer.createObject();

TaskHandle_t DHTTask;

const char* ssid="NETGEAR44";
const char* password = "helpfulnest851";

bool success = true;
volatile bool send_state = true;

int buttonState = 0;
bool ledState = false;

const char* DataPacket[] = { "This is my message", "", "ESP32" };

byte temp = 0;
byte humid = 0;
byte rawData[40] = {0};

void GetTempReading(void * parameter){
  while(true){
    Serial.println("=================================");
    Serial.println("Getting Info from DHT11...");
    int result = dht11.read(pinDHT11, &temp, &humid, rawData);
    Serial.print("Result: ");
    Serial.println(result);

    switch(result){
    case 0:
      Serial.print("Sample RAW Bits: ");
      for (int i = 0; i < 40; i++) {
        Serial.print((int)rawData[i]);
        if (i > 0 && ((i + 1) % 4) == 0) {
          Serial.print(' ');
        }
      }
      Serial.println("");
      
      Serial.print("Sample OK: ");
      Serial.print((int)temp); Serial.print(" *C, ");
      Serial.print((int)humid); Serial.println(" %");
      break;
    case 101:
      Serial.print("\nRead DHT11 Warning: Checksum Test Failed");
      return;
    case 111:
      Serial.print("\nRead DHT11 Error: Connection Timeout - Check Sensor Connection"); 
      return;
    }

    delay(3000);
    

  }  
}

void setup() {
  Serial.begin(115200);
  delay(4000);

  pinMode(pinDHT11, INPUT);
  pinMode(pinLED, OUTPUT);
  
  Serial.println("");
  Serial.println("Booting....");
  WiFi.begin(ssid,password);

  Serial.print("Connecting to WiFi...");
  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");  
  }

  Serial.println("Connected to WiFi network");

  xTaskCreatePinnedToCore(
    GetTempReading,
    "TempCheck",
    1000,
    NULL,
    1,
    &DHTTask,
    0);

  Serial.println("Boot Complete.");
}

void loop() {
  Serial.println("===================================");
  HTTPClient http;
  http.begin("https://us-central1-my-test-project-5984d.cloudfunctions.net/RESTapi/esp32/data/");
  
  
  if (WiFi.status() == WL_CONNECTED && success && send_state){  
    http.addHeader("Content-Type", "text/plain");

    String output = "";
    
    root["os_timestamp"] = millis();
    root["Temp"] = temp;
    root["Humidity"] = humid;

    root.printTo(output);

    Serial.println("Sending Data to Firebase....");
    int httpResponseCode = http.POST(output);    


    Serial.print("Result: ");
    Serial.println(httpResponseCode);
    switch(httpResponseCode){
      case 0:
        Serial.println("Error Sending Data");
        success = false;
        break;
      case 200:
      case 304:
        Serial.println("Info Successfully Sent");
        success = true;
        break;
      case 500:
        Serial.println("Internal Server Error");
        success = false;
        send_state = false;
        break;
    }
    
  } else {
    success == true || send_state == false ? Serial.println("WiFi Not Connected") : 
      send_state == false ? Serial.println("System Idle - Data Posted") : Serial.println("Issue Sending Data - Check Logs");
  }

  Serial.println("===================================");
  http.end();
  delay(30000);
}


/*
 * HTTPClient http;
  http.begin("https://us-central1-my-test-project-5984d.cloudfunctions.net/RESTapi/esp32/messages/");
  
  
  if (WiFi.status() == WL_CONNECTED && success && send_state){

    Serial.println("Sample DHT11....");
    byte temp = 0;
    byte humid = 0;
    byte data[40] = {0};

   

  
    http.addHeader("Content-Type", "text/plain");

    String output = "";
    
    root["timestamp"] = millis();
    root["Temp"] = temp;
    root["Humidity"] = humid;

    root.printTo(output);
    
    int httpResponseCode = http.POST(output);    


    Serial.print("Result: ");
    Serial.println(httpResponseCode);
    switch(httpResponseCode){
      case 0:
        Serial.println("Error Sending Data");
        success = false;
        break;
      case 200:
      case 304:
        Serial.println("Info Successfully Sent");
        success = true;
        break;
      case 500:
        Serial.println("Internal Server Error");
        success = false;
        send_state = false;
        break;
    }
    
  } else {
    success == true || send_state == false ? Serial.println("WiFi Not Connected") : 
      send_state == false ? Serial.println("System Idle - Data Posted") : Serial.println("Issue Sending Data - Check Logs");
  }

  Serial.println("===================================");
  http.end();
  delay(10000);
 * 
 * 
 */
 
