/* Sensors and Interupts

    Sketch designed to learn how to work with sensor calls and interupts to allow for a user to 


*/

// Declare Dependencies
#include <DallasTemperature.h>
#include <OneWire.h>

enum Scale { Celsius, Fahrenheit, Kelvin };

class Button {
    public : 
      int buttonPin;
      int buttonState;
      
      int lastButtonState = LOW;
      int lastButtonChange = 0;
      
      unsigned long lastDebounceTime = 0;

      Button(int pinNumber, uint8_t buttonMode=OUTPUT) {
        buttonPin = pinNumber;
        pinMode(pinNumber, buttonMode);
      }

      bool isPristine() {
        return lastButtonChange == 0;
      }

      bool debounce(int reading) {
        
        if (reading != lastButtonState) {
          // reset the debouncing timer
          lastDebounceTime = millis();
        }
        
        if ((millis() - lastDebounceTime) > debounceDelay) {
          // whatever the reading is at, it's been there for longer than the debounce
          // delay, so take it as the actual current state:        

          // if the button state has changed:
          if (reading != buttonState) {
            buttonState = reading;
            lastButtonChange = millis();
            // only toggle the LED if the new button state is HIGH
            return buttonState == HIGH;            
          } else {
            return false;
          }
        }
        lastButtonState = reading;
      }

    private:
      unsigned long debounceDelay = 100;

};

class TempProbe {
  public:
    DallasTemperature sensors;
    OneWire tempWire; 

    int lastTempRequest = 0;

    struct temp {
      Scale currentScale = Fahrenheit;
      float currentTemp = 0;
      float avgTemp = 0;
      int tempReadCount = 0;
    } tempData;

    TempProbe(int pinNumber):
      tempWire(pinNumber),
      sensors(&tempWire)
    {
      sensors.begin();
    }

    void pollTemp() {
      sensors.requestTemperatures();
    }

    float getTempValue() {
      switch(currentScale){
        case Celsius:
          return sensors.getTempCByIndex(0);
          break;
        case Fahrenheit:
          return sensors.getTempFByIndex(0);
          break;
        case Kelvin:
          float kSensors = (sensors.getTempFByIndex(0) + 459.67f)*(5/9); 
          break;
      }
    }

    String getTempString() {
      String output = "";
      switch(currentScale){
        case Celsius:
          output += sensors.getTempCByIndex(0);
          output += " \u00B0C"; 
          break;
        case Fahrenheit:
          output += sensors.getTempFByIndex(0);
          output += " \u00B0F"; 
          break;
        case Kelvin:
          float kSensors = (sensors.getTempFByIndex(0) + 459.67f)*(5/9); 
          output += kSensors;
          output += " K";
          break;
      }
    } 

  private:
};

// constants won't change. They're used here to set pin numbers:
const int ledPin = 13;      // the number of the LED pin

// Construct Objects
Button b(3, INPUT);
TempProbe t(2);

#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// App Variables
int ledState = HIGH;


void setup() {
  Serial.begin(9600);

  sensors.begin();
  
  // pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

}

void loop() {
  
  int startTime = millis();

  Serial.print("Requesting temperature....");

  t.pollTemp();
  Serial.println("DONE");

  int endTime = millis();

  String tempData = "Temperature: ";
  tempData += t.sensors.getTempFByIndex(0);
  tempData += " \u00B0F";
  Serial.println(tempData);

  Serial.print("Start Time: "); Serial.println(startTime);
  Serial.print("End Time: "); Serial.println(endTime);
  Serial.print("Scan Time: "); Serial.print(endTime - startTime); Serial.println(" ms");
  //Serial.print("Temperature: "); Serial.print(t.sensors.getTempFByIndex(0)); Serial.println(" \u00B0F");
  Serial.println("============================"); Serial.println("");
  
}
