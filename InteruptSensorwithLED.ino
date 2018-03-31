/* Sensors and Interupts

    Sketch designed to learn how to work with sensor calls and interupts to allow for a user to 


*/

// Declare Dependencies
#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <LiquidCrystal.h>
#include <Keypad.h>

enum Scale { Celsius, Fahrenheit, Kelvin };

enum State { Menu, Setup, Time, Temp, Brew, BrewSetup };

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

      float newVal = getTempValue();
      tempData.currentTemp = newVal;
      tempData.avgTemp += newVal;
      tempData.tempReadCount++;
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

    void setScale(Scale scale) {
      tempData.currentScale = scale;
    }

};

class Line {
  public: 
    String text = "";
    int startPosition = 0;

    int lineSize = 16;

    Line(String newLine) {
      text = newLine;
    }

    int getSize() {
      return text.length();
    }

    String scrollText() {
      char charBuf[text.length()];
      text.toCharArray(charBuf, text.length()+1);
      int txtLen = strlen(charBuf);

      String output = "";

      if ((txtLen - startPosition) > lineSize){
        for(int j = startPosition; j < txtLen; j++){
          output += charBuf[j];
        }

        startPosition++;
      } else {
        startPosition = 0;
      }

      Serial.print("Text: "); Serial.println(text);
      Serial.print("Text Length:"); Serial.println(txtLen);
      Serial.println(output);
  
      return output;
    }

    
};

class MenuLine: public Line {
  char selector = '> ';
  bool isSelected = false;

  MenuLine(String str){
    text = str;
    lineSize = 13;
  }

  String drawLine(){
    String output = "";
    output += selector;
  }

};

class DisplayLine: public Line {

  DisplayLine(String str){
    text = str;

  }
};



// Construct Objects
Button b(2, INPUT);
TempProbe t(7);

LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

Line testLine("This is a test line that is too long for me to see");

// Interupt Info
const byte interruptPin = 2;
volatile unsigned long last_interupt_time = 0;

// App Variables
  int count = 0;

  // Serial Port
  bool serialTransmit = true; 

  // Keypad
  const byte ROWS = 4; //four rows
  const byte COLS = 4; //four columns
  char keys[ROWS][COLS] = {
      {'1','2','3','A'},
      {'4','5','6','B'},
      {'7','8','9','C'},
      {'*','0','#','D'}
  };

  byte rowPins[ROWS] = {17, 16, 15 ,14}; //connect to the row pinouts of the keypad
  byte colPins[COLS] = {6, 5, 4, 3}; //connect to the column pinouts of the keypad

  Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() {
  // Setup Serial Port
  Serial.begin(9600);

  lcd.begin(16,2);
  lcd.print("Booting....");

  attachInterrupt(digitalPinToInterrupt(b.buttonPin), ISR_Button, HIGH);
  keypad.addEventListener(keypadEvent); 
}

void loop() {
  if(count == 0){
    lcd.clear();
    lcd.print("   Brew Buddy");
  }

  char key = keypad.getKey();

  if (testLine.getSize() > 16){
    lcd.setCursor(0,0);
    lcd.print(testLine.scrollText());
  } else {
    lcd.setCursor(0,0);
    lcd.print(testLine.text);
  }

    //scrollInFromRight(0,"Settings");

  if ( count - (millis()/1000) > 1) {
    // Serial.print("Polling Themometer....");
    // t.pollTemp();
    // Serial.println("DONE");

    // lcd.setCursor(0,0);
    // lcd.print("Current Temp: ");
    // lcd.setCursor(0,1);
    // lcd.print(t.getTempValue());
  }  

  delay(350);
  Serial.println(count);
  count++;
}


void ISR_Button() {
  unsigned long interuptTime = millis();
  if (interuptTime - last_interupt_time > 200){
      

    last_interupt_time = interuptTime;
  }
  
}

// Taking care of some special events.
void keypadEvent(KeypadEvent key){
    switch (keypad.getState()){
    case PRESSED:
        if (key == '#') {
            
        }
        break;

    case RELEASED:
        if (key == '*') {
            
        }
        break;

    case HOLD:
        if (key == '*') {
            
        }
        break;
    }
}