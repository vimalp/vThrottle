# vThrottle

This project describe a throttle controller for model railrods. The throttle allows control of multiple trains on a layout. It also allows control of turnout points and other functions of a locomotive. 
The controller is implemented entirely in software and it runs on a smart LCD panel.
The controller uses DCCEXProtocol and hence only works with DCC-EX based command stations.

<img width="3145" height="1718" alt="IMG_0825" src="https://github.com/user-attachments/assets/6c7371ec-2176-4784-ad51-3705edd7f5b5" />

## Features

- Control multiple trains at the same time
- Connected to DCC command station over wifi
- Control all functions of each locomotive (headlights, horn, bells, sounds etc)
- Shows track layout and all turnout points and signals. 
- Control of all turnouts
- Send native commands directly to command station
- Dynamic assignment of a throttle control to any train on the layout


## Tech Stack

### Hardware:  
CrowPanel Advance 7inch HMI| ESP32-S3 AI-Powered IPS Touch Screen (800x480)

https://www.elecrow.com/crowpanel-advance-7-hmi-esp32-ai-display-800x480-ai-ips-touch-screen.html?srsltid=AfmBOooG08kHHJE71Ty5PXQrkTReuErOQSTNwiafLM21iU6kwAi_OGL2

### Software

* Arduino IDE
* Crowpanel libraries: https://github.com/Elecrow-RD/CrowPanel-Advance-7-HMI-ESP32-S3-AI-Powered-IPS-Touch-Screen-800x480/tree/master/example/V1.3_and_V1.4_and_V1.5/arduino/libraries
* Squareline Studio:  free version is sufficient
* Other Libraries:
   - DCCEXProtocol
   - LVGL (version 8.3.11 needed by crowpanel
   - LovyanGFX 

## Arduiono Setup
Please follow the crowpanel tutorials (https://www.elecrow.com/wiki/HMI_Display_course.html#version-13) to install all libraries needed by crowpanel
After downloading the project files and installing all libraries, configure Arduino IDE as follows;

* Tools->Board:  ESP32S3 Dev Module
* Tools->Flash Size: 16MB
* Tools->Partition Scheme:  Huge APP
* Tools->PSRAM:  OPI PSRAM

## UI Features

* **Layout Panel:**  This shows the layout of the tracks. The layout of tracks will differ for each person's own setup. The tracks are laid out using Panel widget of SQL Studio.
* **Turnouts:**      The turnout points are shown on the layout panel as square icons with tracks. Clicking on the turnout icon changes the icon image to show 'thrown' state of the tracks.
                 <img width="15" height="15" alt="turnout_tl0_0" src="https://github.com/user-attachments/assets/485d58cc-76cf-40aa-bbb5-a0ec73a6d03d" />
* **Signals:**      Signals are shown as round objects near turnout icons. The siganals are informative only and are set by the underlying interlock system (if implemented)

* **Throttle Panels:** Each throttle panel controls one locomotive on the layout. Each throttle has following controls
   - **Speed Slider:**  Controls the speed of locomotove associated with this throtlle. The speed value is also displayed on top-right side of the slider.
   - **Horn:**          Horn button is a momentary switch that will sound horn as long as it is pressed down
   - **Reverse** (<img width="20" height="20" alt="loco_fwd" src="https://github.com/user-attachments/assets/1fcc99aa-32cc-4880-8e38-d092c14275f1" />):  Controle forward/reverse direction of locomotive
   - **Loco Address:**  This dropdown menu shows which locomotive is currently associated with this throttle. Clicking on the name will bringup the complete roster of all locomotives and user can select any loco
   - **Fn Button:**     This will bringup a function panel. The function panel has a number of button associated with some of the common locomotive functions (headlight, sounds etc).

* **DCC-EX button:**  This button will bringup a keyboard to enter and send native DCC-EX commands to the command station.

## License
The software is free to download and use. 

## Disclaimer
The author assumes no liabilty on the use of software or any bugs contained therein.
The software is provided for use as-is and may contain bugs.


  





