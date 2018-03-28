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

    TempProbe(int pinNumber, Scale scale = Fahrenheit):
      tempWire(pinNumber),
      sensors(&tempWire)
    {
      sensors.begin();
      tempData.currentScale = scale;
    }

    void pollTemp() {
      sensors.requestTemperatures();
    }

    float getTempValue() {
      switch(tempData.currentScale){
        case Celsius:
          return sensors.getTempCByIndex(0);
          break;
        case Fahrenheit:
          return sensors.getTempFByIndex(0);
          break;
        case Kelvin:
          return ((sensors.getTempFByIndex(0) + 459.67f)*(5.0f/9.0f)); 
          break;
      }
    }

    String getTempString() {
      String output = "";
      switch(tempData.currentScale){
        case Celsius:
          output += sensors.getTempCByIndex(0);
          output += " \u00B0C";
          return output; 
          break;
        case Fahrenheit:
          output += sensors.getTempFByIndex(0);
          output += " \u00B0F"; 
          return output;
          break;
        case Kelvin:
          float kSensors = ((sensors.getTempFByIndex(0) + 459.67f)*(5.0f/9.0f)); 
          output += kSensors;
          output += " K";
          return output;
          break;
      }
    } 

  private:
};

// Construct Objects
Button b(2, INPUT);
TempProbe t(3);

const byte ledPin = 13;
const byte interruptPin = 2;

volatile int count = 0;
volatile unsigned long last_interupt_time = 0;
volatile byte ledState = LOW;

void setup() {
  Serial.begin(9600);
  
  pinMode(13, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(b.buttonPin), ISR_Button, HIGH);

}

void loop() {
  digitalWrite(13,ledState);

  Serial.print("Getting Temp.....");
  t.pollTemp();
  Serial.println("DONE");

  Serial.print("Temperature: "); Serial.println(t.getTempString());
  Serial.println("===============================");
  
}


void ISR_Button() {
  unsigned long interuptTime = millis();
  if (interuptTime - last_interupt_time > 200){
    Serial.println("Button Pressed");
    
    ledState = !ledState;
      
    Serial.println(count);
    count++;
    last_interupt_time = interuptTime;
  }
  
}