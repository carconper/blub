# BLUB -- Your Bright Companion

## DESCRIPTION

(Note: The proposed code in this repo is just a prototype of the final product)

Blub is a lighting fixture compatible with a regular lamp. It's geometry is not much different than a regular bulb.

However, through its built-in microphone and interacting with other smart devices, such as home automation systems or smart phones, it provides additional services when going to bed or when waking up.
It comprises two parts: the lighting module, and the information ring



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
  - 
  
Arduino will change the light/blink pattern based on the command sent
