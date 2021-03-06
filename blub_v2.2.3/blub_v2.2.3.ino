// Libraries
#include "Timer.h"
#include <LiquidCrystal_I2C.h>

// Define pins to be used
// -> OUT pins (lighting)
const int YELLOW1 = 3;  // DGTL PWM OUT
const int YELLOW2 = 5;  // DGTL PWM OUT
const int ORANGE = 9;   // DGTL PWM OUT
const int BLUE = 10;    // DGTL PWM OUT
const int WHITE = 6;   // DGTL PWM OUT
const int RED = 11;   // DGTL PWM OUT
const int GREEN = 12;   // DGTL PWM OUT

// -> IN pins (inputs)
const int cloudPin = 12;// DGTL OUT  
const int soundPin = 0; // ANLG IN
// Informational pins
const short W = 7;      // DGTL IN
const short T = 4;      // DGTL IN
const short W1 = 2;      // DGTL IN
const short W2 = 13;      // DGTL IN

// State Machine -- States
short state = 0;            // To keep the track of the state 
const short OFF = 0;        // default State
const short DAY = 1;        // Day State
const short DAYUP = 8;      // Day -- Waking up State
const short DAYON = 9;      // Day -- On state
const short DAYSTAT = 2;    // Day -- Show status State
const short NIGHT = 4;      // Night State
const short NIGHTAWAKE = 5; // Night -- Awake State
const short NIGHTDOWN = 6;  // Night -- Falling Sleep State
const short NIGHTSLEEP = 7; // Night -- Sleep State

// Informational States -- Weather and Traffic
// Weather
short weather = 0;
const short SWEAT = 21;     // Weather -- Sweaty weather
const short UMBRELLA = 22;  // Weather -- Rainy weather
const short COLD = 23;      // Weather -- Cold weather
const short PERFECT = 24;   // Weather -- Amazing weather
// Traffic
short traffic = 0;
const short GOOD = 31;      // Night -- Sleep State
const short DELAYS = 32;    // Night -- Sleep State
const short PATIENT = 33;   // Night -- Sleep State
const short NIGHTMARE = 34; // Night -- Sleep State

// Timers
Timer tAwake;             // Keeps control of awake Time on night state
Timer tSleep;             // Keeps dimm down time
Timer tWakeup;            // Triggers Day State
Timer tDebug;             // Print debugging info into Serial


// Set LCD’s address to be 0x20. Let it display 2 lines with 16 characters per line
LiquidCrystal_I2C lcd(0x20,16,2); 

// Normal flow
int time_alarm;

// Debug
String toPrint = "State: ";

// Auxiliaries
boolean alternate ;
boolean down ;
short bright_yell1 = 0;
short bright_yell2 = 0;
short bright_orange = 0;
short bright_blue = 0;
short bright_white = 0;
short bright_green = 0;
short brightness = 0;
int t_every;


void setup() {
  // put your setup code here, to run once:

  pinMode(cloudPin, INPUT);
  Serial.begin(9600); // You can uncomment this for monitoring

  // Set OUTPUT mode for lighting LEDs
  pinMode(YELLOW1, OUTPUT);
  pinMode(YELLOW2, OUTPUT);
  pinMode(ORANGE, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(WHITE, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);

  // Set INPUT mode for informational pins
  // These are connected to the Particle Core
  pinMode(W, INPUT);
  pinMode(T, INPUT);
  pinMode(W1, INPUT);
  pinMode(W2, INPUT);

  // Set the initial brightness of all the LEDs in Digital PWM ports
  analogWrite(YELLOW1, bright_yell1);
  analogWrite(YELLOW2, bright_yell2);
  analogWrite(ORANGE, bright_orange);
  analogWrite(BLUE, bright_blue);
  analogWrite(WHITE, bright_white);
  analogWrite(GREEN, bright_green);

  //Set up the auxiliary LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 1);
  
  alternate = false;

  /* Set the alarm time. 
     This will be a configurable time in the real application */
  time_alarm = 10000 ;

}

/* 
NAME: setInitState()
IN:   NONE
OUT:  sm_state
DESC: This function will be called at every loop and will determine 
      the State Machine status based on the available different 
      inputs
*/
int setInitState(boolean cloudState, int soundState) {

  int sm_state = 0;

  // The starting point hardcoded for the prototype
  if (cloudState) {

    if (soundState > 100) {
      sm_state = NIGHTAWAKE;
    } else {
      sm_state = NIGHTDOWN;
    }
    
  } else {
    sm_state = DAY;
  }

  return sm_state;

}


void upBrightness() {

  // First check yell1 status and act when it completely
  //  switches off
  if (bright_yell1 >= 255) {

    bright_yell1 = 255;
    analogWrite(YELLOW1, bright_yell1);
    
    // Second check yell2 status until completely off
    if (bright_yell2 >= 255) {

      bright_yell2 = 255;
      analogWrite(YELLOW2, bright_yell2);
      
      // Last check orange status until off
      if (bright_white >= 255) {

        bright_white = 255;
        analogWrite(WHITE, bright_white);
        
        state = DAYON;
        return;  
        
      } else {

        analogWrite(WHITE, bright_white);  
        bright_white = bright_white + 100;
      
      } // if white
    
    } else {

      analogWrite(YELLOW2, bright_yell2);
      bright_yell2 = bright_yell2 + 100;
        
    } // if yell2
    
  } else {

    analogWrite(YELLOW1, bright_yell1);
    bright_yell1 = bright_yell1 + 100;
        
  } // if yell1


  
}


void downBrightness() {

  
  // First check yell1 status and act when it completely
  //  switches off
  if (bright_yell1 <= 0) {

    bright_yell1 = 0;
    analogWrite(YELLOW1, bright_yell1);
    
    // Second check yell2 status until completely off
    if (bright_yell2 <= 0) {

      bright_yell2 = 0;
      analogWrite(YELLOW2, bright_yell2);
      
      // Last check orange status until off
      if (bright_orange <= 0) {

        bright_orange = 0;
        analogWrite(ORANGE, bright_orange);
        
        state = NIGHTSLEEP;
        return;  
        
      } else {

        analogWrite(ORANGE, bright_orange);        
        bright_orange = bright_orange - 100;

      } // if orange
    
    } else {

      analogWrite(YELLOW2, bright_yell2);
      bright_yell2 = bright_yell2 - 100;
      
    } // if yell2
    
  } else {

    analogWrite(YELLOW1, bright_yell1);
    bright_yell1 = bright_yell1 - 100;
    
  } // if yell1
    
}


void setNightdown() {

  Serial.print("Setting Night Down State");
  state = NIGHTDOWN;
  
  return;
}


void setWakeup() {

  Serial.print("Setting Day State");
  state = DAYUP;

  return;
  
}


void setDay() {

  Serial.println("Going back to Day Mode");
  state = DAY;
}


// We will use this function to select the weather status to be shown
//  based on the bits received from Particle Core
short whichWeather() {

  short result;

  // If pin W1 is set to 1 we have bad weather (COLD or UMBRELLA)

  Serial.println("Weather Selection (W1 and W2):");
  Serial.println(digitalRead(W1));
  Serial.println(digitalRead(W2));
  
  if(digitalRead(W1)) {
    if(digitalRead(W2)) {
      result = COLD;
    } else {
      result = UMBRELLA;
    }
  // If W1 is not set we have good weather (SWEAT or PERFECT)
  } else {
    if(digitalRead(W2)) {
      result = SWEAT ;
    } else {
      result = PERFECT;
    }
  }

  Serial.println("Weather Selected is:");
  Serial.println(result);

  return result;
}

// We will use this function to select the traffic status to be shown
//  based on the bits received from Particle Core
short whichTraffic() {
  
  short result;

  
  Serial.println("Traffic selection (W1 and W2):");
  Serial.println(digitalRead(W1));
  Serial.println(digitalRead(W2));

  // If pin T1 is set to 1 we have high traffic (NIGHTMARE or PATIENT)
  if(digitalRead(W1)) {
    if(digitalRead(W2)) {
      result = NIGHTMARE;
    } else {
      result = PATIENT;
    }
  // If T1 is not set we have low traffic (DELAYS or GOOD)
  } else {
    if(digitalRead(W2)) {
      result = DELAYS ;
    } else {
      result = GOOD;
    }
  }

  Serial.println("Traffic Selected is:");
  Serial.println(result);


  return result;
}


/* The following collection fo functions enable the different
  blinking modes for the Information Statuses: Alternate, blink, 
  glow, dimup...*/

  // DIMUP blinking pattern
short setBrightnessDimup() {

  brightness = brightness + 20;

  if (brightness > 255) {
    brightness = 255;
    // Stop "every timer"
    tDebug.stop(t_every);
    // Keep color pattern for 5s (t2)
    tAwake.after(5*1000,setDay);
    // And return to DAY STAT waiting state
  }

  return brightness ;
}


// Alternate blinking pattern
short setBrightnessAlt() {

  if (brightness == 255) {
    brightness = 40 ;
  } else {
    brightness = 255;
  }

  return brightness;
  
}


// Glowing blinking pattern
short setBrightnessGlow() {

  if (brightness > 255) {
    down = 1;
  } else if (brightness < 0) {
    down = 0;
  }

  if(down) {
    brightness = brightness - 25;
  } else {
    brightness = brightness + 25;
  }
  
  return brightness;
  
}


// Standard blinking pattern
short setBrightnessBlink() {

  if (brightness == 255) {
    brightness = 0;
  } else {
    brightness = 255;
  }

  return brightness;
    
}


/* The following functions take care of setting up the 
  color pattern for each of the Informational Statuses*/

// WEATHER STATUSES
// Set color pattern for the Sweat weather status
void setSweat() {

  brightness = setBrightnessGlow();

  Serial.println("Brightness in SWEAT is:");
  Serial.println(brightness);
  
  analogWrite(YELLOW1, 0);
  analogWrite(YELLOW2, 0);
  analogWrite(WHITE, 0);
  analogWrite(ORANGE, brightness);
  analogWrite(BLUE, 0);
  digitalWrite(GREEN, LOW);
  analogWrite(RED, 0);

  state = DAYSTAT;

  return;
  
}


// Set color pattern for the Umbrella weather status
void setUmbrella() {

  brightness = setBrightnessAlt();
  
  Serial.println("Brightness in UMBRELLA is:");
  Serial.println(brightness);
  
  analogWrite(YELLOW1, 0);
  analogWrite(YELLOW2, 0);
  analogWrite(WHITE, 100);
  analogWrite(ORANGE, 0);
  analogWrite(BLUE, brightness);
  digitalWrite(GREEN, LOW);
  analogWrite(RED, 0);

  // Move to waiting DAY state, listening to input from Particle
  state = DAYSTAT;


  return;
  
}


// Set color pattern for the Cold weather status
void setCold() {

  brightness = setBrightnessGlow();

  Serial.println("Brightness in COLD is:");
  Serial.println(brightness);
  
          analogWrite(YELLOW1, 0);
          analogWrite(YELLOW2, 0);
          analogWrite(WHITE, 60);
          analogWrite(ORANGE, 0);
          analogWrite(BLUE, 30);
          digitalWrite(GREEN, LOW);
          analogWrite(RED, brightness);


  // Move to waiting DAY state, listening to input from Particle
  state = DAYSTAT;

  return;
}


// Set color pattern for the Perfect weather status
void setPerfect() {

  brightness = setBrightnessDimup();

  Serial.println("Brightness in PERFECT is:");
  Serial.println(brightness);
  
          analogWrite(YELLOW1, 0);
          analogWrite(YELLOW2, 0);
          analogWrite(WHITE, 0);
          analogWrite(ORANGE, 100);
          analogWrite(BLUE, brightness);
          digitalWrite(GREEN, LOW);
          analogWrite(RED, brightness);
  
  // Move to waiting DAY state, listening to input from Particle
  state = DAYSTAT;

  return ;
}


// TRAFFIC STATUSES
// Set color pattern for the Nightmare traffic status
void setNightmare() {

  brightness = setBrightnessBlink();
  
  
  analogWrite(YELLOW1, 0);
  analogWrite(YELLOW2, 0);
  analogWrite(RED, brightness);
  analogWrite(WHITE, 0);
  analogWrite(ORANGE, 0);
  analogWrite(BLUE, 0);
  digitalWrite(GREEN, LOW);

  state = DAYSTAT;

  return ;
}



/* The following function takes care of setting up the timers
  and calling the specific function for defining the blinking
  pattern for each of the information statuses regarding
  Weather*/
void selectWeather(short weath) {

      switch(weath) {

        case SWEAT:
          // Glow Orange to represent "you will sweat"

          // Set color pattern
          brightness = 0;
          setSweat();
          // Start "every 100ms" timer (t1)
          t_every = tDebug.every(100,setSweat);
          // Start "after 5s" timer (t2)
          tAwake.after(10*1000,setDay);
          
          break;

        case UMBRELLA:
          // Fast blue blink to represent "bring your umbrella"

          // Set color pattern
          brightness = 0;
          setUmbrella();
          // Start "every 100ms" timer (t1)
          t_every = tDebug.every(100,setUmbrella);
          // Start "after 5s" timer (t2)
          tAwake.after(10*1000,setDay);
          
          break;

        case COLD:
          // Twist the outer ring of Blub to represent cold weather

          // Set color pattern
          brightness = 0;
          setCold();
          // Start "every 100ms" timer (t1)
          t_every = tDebug.every(150,setCold);
          //Serial.println("The value of t_every is:");
          //Serial.println(t_every);
          // Start "after 5s" timer (t2)
          tAwake.after(10*1000,setDay);
          

          break;

        case PERFECT:
          // Dim up dark pink to represent a perfect day

          // Set color pattern
          brightness = 0;
          setPerfect();
          // Start "every 100ms" timer (t1)
          t_every = tDebug.every(200,setPerfect);
          
          
          break;
      }      
 
} // selectWeather



/* The following function takes care of setting up the timers
  and calling the specific function for defining the blinking
  pattern for each of the information statuses regarding
  Traffic*/
void selectTraffic(short traf) {


      switch(traf) {

        case GOOD:
          // Fixed green to represent fluid traffic

          analogWrite(YELLOW1, 0);
          analogWrite(YELLOW2, 0);
          analogWrite(RED, 0);
          analogWrite(WHITE, 0);
          analogWrite(ORANGE, 0);
          analogWrite(BLUE, 0);
          digitalWrite(GREEN, HIGH);

          break;

        case DELAYS:
          // Fixed Green/Yellow to represent low delays

          analogWrite(YELLOW1, 120);
          analogWrite(YELLOW2, 120);
          analogWrite(RED, 0);
          analogWrite(WHITE, 0);
          analogWrite(ORANGE, 0);
          analogWrite(BLUE, 0);
          digitalWrite(GREEN, HIGH);
          
          break;

        case PATIENT:
          // Fixed Orange to represent "patience needed, expect delays"

          analogWrite(YELLOW1, 120);
          analogWrite(YELLOW2, 120);
          analogWrite(RED, 150);
          analogWrite(WHITE, 0);
          analogWrite(ORANGE, 100);
          analogWrite(BLUE, 0);
          digitalWrite(GREEN, LOW);

          break;

        case NIGHTMARE:
          // Blinking Red to reprensent "you wont make it on time, look for alternatives"

         // Set color pattern
          brightness = 0;
          setNightmare();
          // Start "every 1s" timer (t1)
          t_every = tDebug.every(1000,setNightmare);
          // Start "after 5s" timer (t2)
          tAwake.after(10*1000,setDay);

          break;
      }
}



// Main loop where the State Machine of Blub is called on each iteration
void loop() {
  // put your main code here, to run repeatedly:

  // soundState and cloudState are used when interacting with the microphone
  //  and the cloud. Not used in this version of the prototype
  
  //boolean cloudState = digitalRead(cloudPin); // Read the signal received from cloud
  boolean cloudState;
  //int soundState = analogRead(soundPin); // Read sound sensor’s value
  int soundState;

  int downEvent ;
  int upEvent ;


  // To switch ON the lamp in the first iteration
  if (state == OFF) {
    cloudState=true;
    soundState=101;
    state = setInitState(cloudState, soundState) ;
  }

  // Update the timers
  tDebug.update();
  tSleep.update();
  tAwake.update();
  tWakeup.update();
  

  // Main state machine keeping the different states Blub goes through
  //  In this prototype Blub goes through all the phases, first covering
  //  the diming down (night) and dimming up (day) phases and after that 
  //  staying in ON mode waiting for the different signal to show the 
  //  Information Statuses (Traffic and Weather)
  switch (state) {

    case NIGHT:     // Transition State 
      // wait for timers to expire
      break;
      
    case NIGHTAWAKE:
      // Keep Light on
      // Set pattern night-awake
      bright_yell1 = 255;
      bright_yell2 = 255;
      bright_orange = 255;

      // Switch on BLUB
      analogWrite(YELLOW1, bright_yell1);
      analogWrite(YELLOW2, bright_yell1);
      analogWrite(ORANGE, bright_orange);
      
      // Start Timer tAwake
      tAwake.after(10*1000,setNightdown);

      // Go to NIGHT waiting state
      state = NIGHT;
      
      break;
      
    case NIGHTDOWN:
      // Start Sleepdown timer tSleep
      // Dim down every 5 seconds
      downEvent = tSleep.every(2*1000,downBrightness);

      // Go to NIGHT waiting state
      state = NIGHT;
      
      break;
      
    case NIGHTSLEEP:
      
      // Stop tSleep timer
      tSleep.stop(downEvent);

      // Start alarm to wake up (tWakeup timer)
      Serial.print("Setting timer for waking up!");
      tWakeup.after(time_alarm,setWakeup);

      // Go to NIGHT waiting state
      state = NIGHT;
      
      break;

    case DAYUP:

      // Start waking up timer
      // Dim up every 5 seconds
      upEvent = tWakeup.every(2*1000, upBrightness);

      // Move to waiting state DAY
      state = DAY ; 

      break;
      
    case DAYSTAT:
      // Waiting state. We move into this state when we are waiting for 
      //  a timer to expire and we dont have anythign else to do
      
      break;
      
    case DAY:

      // DEBUG infor on Serial port
      Serial.println("DAY mode");

      //Auxiliary LCD shows the status of the Informational PINs
      //  sent by Particle Core through the local network/internet
      
      lcd.setCursor(0,0);
      lcd.print("                ");
      lcd.setCursor(0,0);
      lcd.print("W: ");
      lcd.setCursor(3,0);
      lcd.print(digitalRead(W));
      lcd.setCursor(6,0);
      lcd.print("T: ");
      lcd.setCursor(9,0);
      lcd.print(digitalRead(T));

      lcd.setCursor(0,1);
      lcd.print("W1: ");
      lcd.setCursor(3,1);
      lcd.print(digitalRead(W1));
      lcd.setCursor(6,1);
      lcd.print("W2: ");
      lcd.setCursor(9,1);
      lcd.print(digitalRead(W2));

      
      // Stop the every timer set for the blinking mode
      tDebug.stop(t_every);

      // If Particle Core enables the Weather bit (particle sends the
      //  pulse to indicate a weather status is ready to be read) then
      //  we prepare Blub to show the corresponding weather status
      if (digitalRead(W)) {

        // Select which weather status to show based on the 
        //  Informational bits received from Particle Core
        weather = whichWeather();  

        // Call the function to set up the light pattern of the
        //  weather status to show
        selectWeather(weather);
        Serial.println("Selected weather:");
        Serial.println(weather);

        // Wait for some time to read the pulse sent by particle
        delay(1000);

        // Go to the waiting state DAY until timer expires
        state = DAY;

      // If on the contrary Particle Core enables the Traffic bit 
      //  (particle sends the
      //  pulse to indicate a traffic status is ready to be read) then
      //  we prepare Blub to show the corresponding traffic status status
      } else if (digitalRead(T)) {
        
        // Select which traffic status to show based on the 
        //  Informational bits received from Particle 
        traffic = whichTraffic();

        // Call the function to set up the light pattern of the
        //  specific traffic status to be set up   
        selectTraffic(traffic);
        Serial.println("Slected traffic:");
        Serial.println(traffic);

        // Wait for some time to read the pulse sent by particle
        delay(1000);

        // Go to the waiting state DAY until timer expires
        state = DAY;
      } 
     
      break;

    case DAYON:

      Serial.print("DAYON") ;
      // Stop tWakeup timer
      tWakeup.stop(upEvent);

      // Go to the waiting state DAY until timer expires
      state = DAY;

      // Switch on the white light for the standar ON mode for the day
      analogWrite(YELLOW1, 0);
      analogWrite(YELLOW2, 0);
      analogWrite(WHITE, 255);
      analogWrite(ORANGE, 0);
      analogWrite(BLUE, 0);

      break;
      
    case OFF:

      // Switch off all the lights
      analogWrite(YELLOW1, 0);
      analogWrite(YELLOW2, 0);
      analogWrite(WHITE, 0);
      analogWrite(ORANGE, 0);
      analogWrite(BLUE, 0);
      analogWrite(RED, 0);
      digitalWrite(GREEN, LOW);
      
      break;
      
    default:
      
      break;
  }

}
