# BLUB -- Your Bright Companion

## DESCRIPTION

(Note: The proposed code in this repo is just a prototype of the final product that is described below)

Blub is a lighting fixture compatible with a regular lamp. It's geometry is not much different than a regular bulb.

However, through its built-in microphone and interacting with other smart devices, such as home automation systems or smart phones, it provides additional services when going to bed or when waking up.
It comprises two parts: the lighting module, and the information ring.

The lighting module is made of led's of different hues which, when combined, emulate the sun hues of the sunset or the sunrise.
The information ring is made of RGB led's that, combined, cast an additional layer of light for some seconds to subtly provide some useful information.

### How it works

#### Sleep Time

You can set up on your Blub App the time you typically go to bed. Around that time, Blub will be on Sleep Mode. This means, every time you turn it on at sleep time, a countdown will start. For 5 min it will light at full brightness, then start dimming down for 5 more minutes if it does not feel you are awake, until it turns off completely. These default times can be adjusted on the Blub App as well.

If any interaction of being active happens, i.e. using your phone or talking, the countdown will start over again at the brightness status of the time it was triggered.

#### Wake Up Time

Blub will interact with your smart devices, for example, to start dimming up some time before your clock alarm rings, to help transition to real life. You can set it up to start dimming up earlier if the traffic that day is expected to be bad or if heavy rains can make you get late to work.

#### Useful Data

Through Blub you can get useful information in a quick and intuitive manner to help you leave to work in the morning as efficiently as possible.

By saying "Traffic" the information ring will cast some color patterns for some seconds to show the status of traffic on your daily commute, as is set up on your phone.

In the same way, by saying "Weather" it will tell you what to wear at home and on your destination by means of colors and lighting patterns.


## PROTOTYPE

As a prototype it doesnt have all the functionality of the final product but shows instead the application and the different interfaces that can be used to interact with Blub.

This prototype follows a static path through the proposed State Machine so it traverses all the possible states.
It interacts with the information sent through the network with different commands in order to move through the Informational Statuses (Weather and Traffic).


### Components:

- Arduino UNO
- I/O Expansion Shield
- Particle Core
- LCD 16x2
- Prototyping Board
- LEDs and Resistors (see full component list under the resources folder)


### Files

- blub_v2.2.3/blub_v2.2.3.ino: The code for the arduino board
- blub_remote_controller_v1.0: The code for the Particle Core board
- resources: The fritzing diagrams (breadboard and schematics) together with the component list can be found in this folder


### Instructions

1) Following the Diagrams in resources folder, set up the circuit and connections with the Arduino UNO board and the Particle Core
2) Load the blub_v2.2.3/blub_v2.2.3.ino into the Arduino board
3) Load the blub_remote_controller_v1.0 into the Particle Core board
4) The code in the arduino follows a static path inside the State machine:
4.1) Starts in the Night mode completely lighted up with the Night Color pattern *stays there for 10 seconds)
4.2) Moves into the Dim Down phase until its completely off (10 seconds)
4.3) Goes through the Sleeping phase (10 seconds)
4.4) Starts the Wake Up phase, dimming up using the Day Color pattern
4.5) When is completely lighted up it goes into the Day state waiting for the commands from the cloud. Each commands will enable a different color pattern with a different blinking pattern for 10 seconds

### Commands

We use curl to send the commands to the Particle Core that will enble different combinations of bits to let Arduino Know which Status (Weather/Traffic) should show:

curl https://api.particle.io/v1/devices/54ff6b066672524831201167/led -d access_token=9323312f9bb37ed7dcab841689e0facbf708a64c -d params={COMMAND}

Where COMMAND can take any of the following values:

- For Weather Statuses:
  - perfect
  - sweat
  - umbrella
  - cold
- For Traffic Statuses:
  - good
  - delays
  - patient
  - nightmare
  
Arduino will change the light/blink pattern based on the command sent
