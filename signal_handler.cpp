#include "dcc_funcs.h"
//*******************************************************************
// 
// signal_handler.cpp
//
// Model railroad signal control based on turnout, sensor and other
// signal states. 
// 
//
// Authos: Vimal Parikh
// 
//*******************************************************************

#include "vThrottle.h"
#include "signal_handler.h"

//--------------------------------------------------------------------------
// Creare turnout Input objects that return an active status for the turnout
//--------------------------------------------------------------------------
static TurnoutInput* tClosed[NUM_TURNOUTS];
static TurnoutInput* tThrown[NUM_TURNOUTS];
static SensorInput*  blkInputs[NUM_BLOCK_SENSORS];

static SignalHandler*  sigHandlers[NUM_SIGNAL_HEADS];

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
uint32_t SignalHandler::_aspectColors[NUM_ASPECTS] = {
  0x000000,
  0xff0000,   // red
  0xffff00,   // yellow
  0x00ff00,   // green;
};

//--------------------------------------------------------------------------
// Initialize the signal handler
// NOTE: this must be called after all turnout and sensor lists are
// recieved from DCC-EX as it depends on the dcc addresses.
//--------------------------------------------------------------------------
void init_signal_handler()
{
  // set turnout input objects
  for (int t=0; t < NUM_TURNOUTS; ++t) {
    tClosed[t] = new TurnoutInput(turnoutList[t], true);
    tThrown[t] = new TurnoutInput(turnoutList[t], false);
  }

  for (int s=0; s < NUM_BLOCK_SENSORS; ++s) {
    blkInputs[s] = new SensorInput(sensorList[s]);
  }

  //0: SIGNAL_SHL0C
  sigHandlers[0] = new SignalHandler(SIGNAL_SHL0C, tClosed[TURNOUT_TL0], blkInputs[TOWN_LOOP_BLK1], blkInputs[TOWN_LOOP_BLK2]);

  //1: SIGNAL_SHL1TC
  sigHandlers[1] = new SignalHandler(SIGNAL_SHL1TC, tClosed[TURNOUT_TL0], blkInputs[OUT1_LOOP_BLK4], blkInputs[OUT1_LOOP_BLK5]);

  //2: SIGNAL_SHL1TD
  sigHandlers[2] = new SignalHandler(SIGNAL_SHL1TD, tThrown[TURNOUT_TL0], blkInputs[TOWN_LOOP_BLK1], blkInputs[TOWN_LOOP_BLK2]);

  //3: SIGNAL_SHL2C
  sigHandlers[3] = new SignalHandler(SIGNAL_SHL2C, tClosed[TURNOUT_TL2], blkInputs[OUT2_LOOP_BLK7], blkInputs[OUT2_LOOP_BLK8]);
  
  //4: SIGNAL_SHL2D
  sigHandlers[4] = new SignalHandler(SIGNAL_SHL2D, tThrown[TURNOUT_TL2], blkInputs[OUT2_LOOP_BLK7], blkInputs[OUT2_LOOP_BLK8]);

  //5: SIGNAL_SHR0TC
  sigHandlers[5] = new SignalHandler(SIGNAL_SHR0TC, tClosed[TURNOUT_TR0], blkInputs[TOWN_LOOP_BLK0], blkInputs[TOWN_LOOP_BLK1]);

  //6: SIGNAL_SHR0TD
  sigHandlers[6] = new SignalHandler(SIGNAL_SHR0TD, tThrown[TURNOUT_TR0], blkInputs[OUT1_LOOP_BLK3], blkInputs[OUT1_LOOP_BLK4]);

  //7: SIGNAL_SHR1C
  sigHandlers[7] = new SignalHandler(SIGNAL_SHR1C, tClosed[TURNOUT_TR0], blkInputs[OUT1_LOOP_BLK3], blkInputs[OUT1_LOOP_BLK4]);

  //8: SIGNAL_SHR2TC
  sigHandlers[8] = new SignalHandler(SIGNAL_SHR2TC, tClosed[TURNOUT_TR2], blkInputs[OUT2_LOOP_BLK6], blkInputs[OUT2_LOOP_BLK7]);

  //9: SIGNAL_SHR2TD
  sigHandlers[9] = new SignalHandler(SIGNAL_SHR2TD, tThrown[TURNOUT_TR2], blkInputs[SIDEING_BLK9], blkInputs[OUT2_LOOP_BLK7]);
}

//--------------------------------------------------------------------------
// update all signals based on turnout and sensor states.
//--------------------------------------------------------------------------

void update_signals()
{
  for (int s=0; s < NUM_SIGNAL_HEADS; ++s) {
    sigHandlers[s]->updateSignal();
  }
}

//--------------------------------------------------------------------------
// Set signal head to given aspect. Update the UI and layout.
//--------------------------------------------------------------------------

void SignalHandler::updateSignal() 
  {
    bool turnPass = _tinput ? _tinput->getActive() : true;
    bool blk1Pass = _blk1 ? !_blk1->getActive() :   true;
    bool blk2Pass = _blk2 ? !_blk2->getActive() :   true;;
    SignalAspect_e  aspect = ASPECT_GREEN;

    if (!turnPass) {
      aspect = ASPECT_RED;
    }
    else {
      if (!blk1Pass) {
       aspect = ASPECT_RED;
      }
      else if (!blk2Pass) {
        aspect = ASPECT_YELLOW;
      }
    }
    // update signal if aspect has changed
    if (_curAspect != aspect) {
      DEBUG_PRINTF("updateSignal: %d: pass=(%d,%d,%d), aspect=%d\n", _signalId,
                   turnPass, blk1Pass, blk2Pass, aspect);
      setAspect(aspect);
      _curAspect = aspect;
    }
  }

  void SignalHandler::setAspect(SignalAspect_e aspect)
  {
    char  cmdStr[20];
    snprintf(cmdStr, sizeof(cmdStr), "A %d %d", _signalId, aspect);
    dccexProtocol.sendCommand(cmdStr);  
    setSignalAspect(_signalId, _aspectColors[aspect]);      
  };
