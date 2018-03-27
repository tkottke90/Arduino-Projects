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

      unsigned long lastDebounceTime = 0;

      Button(int pinNumber) {
        buttonPin = pinNumber;
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
          
          Serial.println(reading != buttonState ? "TRUE" : "FALSE");
          if (reading != buttonState) {
            buttonState = reading;

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
const int buttonPin = 3;    // the number of the pushbutton pin
const int ledPin = 13;      // the number of the LED pin
Button b(3);

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

void setup() {
  Serial.begin(9600);

  
  
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // set initial LED state
  digitalWrite(ledPin, ledState);
}

void loop() {
  
  bool test = b.debounce(digitalRead(buttonPin));
  if (test) {
    Serial.print("LED State Change: ");
    Serial.println(ledState == 0 ? "ON" : "OFF");
    Serial.println("");
    ledState = !ledState;
  }


  // set the LED:
  digitalWrite(ledPin, ledState);

}
