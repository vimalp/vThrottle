/* interlockV0_6
  09/08/2025

  */

//Mode Pin
#define STARTMODE 0 //0 = Learning Mode, set to 1 for full interlock mode

//Interlock System

//If you are sending batch commands you can increase the value to get a better more visable change sequence (masnual lever signals would be 4000 - 6000 in reality)
#define INTPROCSPEED 10 //Default 10ms between processing instructions. Can be reduced to 1 for faster processing or increase to 1000-2000 to see the step through of instructions.

//The following settings are about the max for the Mega2560 and may need to be reduced for your input/output processing code.
//ESP32 dev module can handle much larger numbers of items 4000+ rules is handled easily
#define INTNUMINPUTS 100 //The number of signals, turnouts, blocks, crossings, points you want to add to the system, reduce to save memory, increase if more items

#define INTNUMINTERLOCKRULES 300 //Reduce to save memory, Increase this number to add more rules (Beware large values will cause smaller boards to run out of RAM... Mega Max is about 1000 rules, ESP32 approx 15000 rules)

#define INTMONITORBLOCKSIGNALS 40 //Like rules but specific to signals. These special rules monitor a signal (next block) and change the signal state depending on the monitored signal

//0.6 Addition
#define INTDUELMONITORBLOCKSIGNALS 40 // Like signal monitors but for diverging junctions when a signal must monitor 2 signals
//Diagnostics
//This diagnostic helps to see clock signalling effects in real time allowing you to see the changes to the item states
//#define SIGNALMONITORDIAGNOSTICS_ON //Uncomment this line for Signal Monitor diagnostics, processing speed is slowed down to make results easier to read

//This diagnostic allows you to see the rules being processed... comment out after testing
#define RULECHECKDIAGNOSTICS_ON //Uncomment to see Item and it's state, the item being checked in rules, the Rule State and the actual state of the checked item