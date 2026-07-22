//-----------------------------------------------------------------
//
// signal_interlock.cpp
//
// This module uses DigitalTown.uk's interlock++ library to set signal aspects
// based on interlock between other signals, turnout states and block occupancy
//
// Vimal Parikh
// July 2026
//
//-----------------------------------------------------------------

#include "vThrottle.h"

//--------------------------------------------------------
// main instace of interlock class
//--------------------------------------------------------
static interlock mySignalBox;  //create an instance of the class

void init_signal_interlock()
{
    mySignalBox.init();  //This functions sets up the interlock system

  // define all turnouts to the interlock system
  for (int t = 0; t < NUM_TURNOUTS; ++t) {
      mySignalBox.loadInput(turnoutList[t], TURNOUT);
  }

  // define all block occupancy  sensors
  for (int s = 0; s < NUM_BLOCK_SENSORS; ++s) {
      mySignalBox.loadInput(sensorList[s], BLOCK);
  }

  // define all signals
  for (int s = 0; s < NUM_SIGNAL_HEADS; ++s) {
    mySignalBox.loadInput(signalHeads[s].dccAddr, NUM_SIGNAL_ASPECTS);
  }

  // load rules for setting signals and turnouts
  // XXX TBD: make rules from a table
  mySignalBox.loadRule(signalHeads[SIGNAL_SHL0C].dccAddr, SIGGO, turnoutList[TURNOUT_TL0], TURNOUTCLOSED);
  mySignalBox.loadRule(signalHeads[SIGNAL_SHL0C].dccAddr, SIGGO, sensorList[TOWN_LOOP_BLK1], BLOCKCLEAR);

  mySignalBox.loadRule(signalHeads[SIGNAL_SHL1TC].dccAddr, SIGGO, turnoutList[TURNOUT_TL0], TURNOUTCLOSED);
  mySignalBox.loadRule(signalHeads[SIGNAL_SHL1TC].dccAddr, SIGGO, sensorList[OUT1_LOOP_BLK4], BLOCKCLEAR);

  mySignalBox.loadRule(signalHeads[SIGNAL_SHL1TD].dccAddr, SIGGO, turnoutList[TURNOUT_TL0], TURNOUTTHROWN);
  mySignalBox.loadRule(signalHeads[SIGNAL_SHL1TD].dccAddr, SIGGO, sensorList[TOWN_LOOP_BLK1], BLOCKCLEAR);
  
  mySignalBox.loadRule(signalHeads[SIGNAL_SHL2C].dccAddr, SIGGO, sensorList[OUT2_LOOP_BLK7], BLOCKCLEAR);
  
  mySignalBox.loadRule(signalHeads[SIGNAL_SHL1TD].dccAddr, SIGGO, turnoutList[TURNOUT_TL2], TURNOUTTHROWN);
  mySignalBox.loadRule(signalHeads[SIGNAL_SHL2D].dccAddr, SIGGO, sensorList[OUT2_LOOP_BLK7], BLOCKCLEAR);

  // update all turnout states
  for (int tidx = 0; tidx < NUM_TURNOUTS; ++tidx) {
    interlock_update_turnout(turnoutList[tidx], false);
  }

  // update state of all sensors
  for (int sidx = 0; sidx < NUM_BLOCK_SENSORS; ++sidx) {
    uint16_t sensor_addr = sensorList[sidx];
    bool active = dccexProtocol.getSensorById(sensor_addr)->getActive();
    interlock_update_sensor(sensor_addr, active);
  }
}


void run_signal_interlock()
{
  mySignalBox.runInterlock();
}

//--------------------------------------------------------
// received a block detector sensor update from layout
// sensor index is internal index to sensor list
//--------------------------------------------------------

void interlock_update_sensor(uint16_t sensor_addr, bool active)
{
  mySignalBox.checkInterlocks(sensor_addr, active);
}

//--------------------------------------------------------
// received a turnout update from UI.
// turnout index is internal index to turnout list
//--------------------------------------------------------

void interlock_update_turnout(uint16_t turnout_addr, bool thrown)
{
  mySignalBox.checkInterlocks(turnout_addr, thrown);
}

//--------------------------------------------------------
// This function is called in response to updates to 
// sensor or turnouts
//--------------------------------------------------------

void intUpdateLayout(uint16_t item_id, uint8_t instruction)
{
  DEBUG_PRINTF("Interlock: updateLayout: id = %d, val=%d\n", item_id, instruction);

  // check signal heads to set
  for (int sidx=0; sidx < NUM_SIGNAL_HEADS; ++sidx) {
    if (item_id != signalHeads[sidx].dccAddr) continue;

    setSignalAspect(sidx, instruction);
  }
}

//--------------------------------------------------------
// This function is used for screens for displaying 
// error messages as well as successes. 
// It could also be used for logging error. 
// This function is called every time a rule is processed 
// successfully or if a rule has been broken.
//--------------------------------------------------------
void intCheckResults(uint8_t interlockState, uint16_t itemID, uint8_t instruction, 
                     String itemName, uint16_t checkItemID, uint8_t checkInstruction,
                     String checkItemName)
{
    if (interlockState == 0) return;    // success. 

    DEBUG_PRINTF("interlockCheck: FAILED\n");
    DEBUG_PRINTF("   %s %d initiated check with value of %d\n", 
                  itemName.c_str(), itemID, instruction);
    DEBUG_PRINTF("   %s %d failed the test with value of %d\n", 
                  checkItemName.c_str(), checkItemID, checkInstruction);          
}

