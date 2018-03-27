/* Sensors and Interupts

    Sketch designed to learn how to work with sensor calls and interupts to allow for a user to 



// Declare Dependencies
#include <DallasTemperature.h>;

//

void setup() {

  Button dbButton = new Button(3);
}

void loop() {}

string writeLine(string[] text) {}

*/

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


// constants won't change. They're used here to set pin numbers:
const int ledPin = 13;      // the number of the LED pin

// Construct Objects
Button b(3, INPUT);

int ledState = HIGH;

void setup() {
  Serial.begin(9600);

  
  
  // pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

}

void loop() {

  if (b.debounce(digitalRead(b.buttonPin))){
    ledState = !ledState;
  }

  digitalWrite(ledPin, ledState);

}
