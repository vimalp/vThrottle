// 
// **************************************************************************
//
// util.ino
//
// misc utility functions for dccex_throttle
//
// Vimal Parikh 
// April, 2026
//
//
// **************************************************************************
//
#include "esp32_tft_throttle.h"


//------------------------------------------------------------------
// display track power status as LED
//------------------------------------------------------------------

void updateTrackPower(TrackPower trk_pwr)
{
#if 0
  if (trk_pwr == TrackPower::PowerOn) {
    digitalWrite(TRK_PWR_LED_R, HIGH);
    digitalWrite(TRK_PWR_LED_Y, LOW);
  }
  else if (trk_pwr == TrackPower::PowerUnknown) {
    digitalWrite(TRK_PWR_LED_R, LOW);
    digitalWrite(TRK_PWR_LED_Y, HIGH);
  }
  else {
    digitalWrite(TRK_PWR_LED_R, LOW);
    digitalWrite(TRK_PWR_LED_Y, LOW);
  }
#endif
}

//------------------------------------------------------------------
// Update roster list. Update throttles with locos
//------------------------------------------------------------------

void updateRoster()
{
  // update the loco list on dropdown menu 
  int loco_idx = 0;
  for (Loco *loco = dccexProtocol.roster->getFirst(); loco; loco = loco->getNext()) {
    int locoAddr = loco->getAddress();
    const char *name = loco->getName();
    DEBUG_PRINTF("Roster Loco %d: %s\n", locoAddr, name);
    
    for (int thr_idx=0; thr_idx < NUM_THROTTLES; ++thr_idx) {
      setLocoList(thr_idx, loco_idx, name, locoAddr);
    }
    loco_idx++;
  }

  // assign one loco from the list to each throttle
  loco_idx = 0;
  for (Loco *loco = dccexProtocol.roster->getFirst(); loco; loco = loco->getNext()) {
    int locoAddr = loco->getAddress();
    const char *name = loco->getName();

    // check if the loco address is previously stored in the preferences
    uint32_t tidx = myPrefs.getUInt(name, 0);
    if (tidx > 0 && tidx < NUM_THROTTLES) {
      locoList[tidx] = loco; 
    }
    else {
      // assign an empty throttle slot to the loco
      tidx = 0;
      for (; tidx < NUM_THROTTLES; ++tidx) {
        if (!locoList[tidx]) break;
      }
      if (tidx < NUM_THROTTLES) {
        locoList[tidx] = loco;
      }
    }
    DEBUG_PRINTF("Throttle[%d] -> loco %d\n", tidx, locoAddr);
    selectLoco(tidx, loco_idx);
    loco_idx++;
  }
}

void assignLocoToThrottle(int thr_idx, int loco_idx)
{
  if (thr_idx >= NUM_THROTTLES || loco_idx >= dccexProtocol.getRosterCount())
    return;

  Loco* locop = nullptr;
  int lidx = 0;
  for (locop = dccexProtocol.roster->getFirst(); locop; locop = locop->getNext()) {
      if (lidx == loco_idx) break;
      lidx++;
  }
  DEBUG_PRINTF("Setting new loco 0x%x, for throttle %d\n", thr_idx, locop);
  locoList[thr_idx] = locop;
}

//----------------------------------------------------------------------------
// Scan the loco roster and collect all addresses
//----------------------------------------------------------------------------

void getAvailLocoAddresses(uint32_t& num_avail, uint32_t* avail_loco_addresses)
{
  num_avail = 0;
  for (Loco *loco = dccexProtocol.roster->getFirst(); loco; loco = loco->getNext()) {
    avail_loco_addresses[num_avail] = loco->getAddress();
    num_avail++;
  }
}

//----------------------------------------------------------------------------
// Scan the turnout list and collect all turnout pointers
//----------------------------------------------------------------------------

void updateTurnouts()
{
  for (Turnout *tout = dccexProtocol.turnouts->getFirst(); tout; tout = tout->getNext()) {
    int tid = tout->getId();
    switch (tid) {
      case DCC_TL0: { turnoutList[TURNOUT_TL0].dccId = DCC_TL0; turnoutList[TURNOUT_TL0].turnoutp = tout; }  break;
      case DCC_TR0: { turnoutList[TURNOUT_TR0].dccId = DCC_TR0; turnoutList[TURNOUT_TR0].turnoutp = tout; }  break;
      case DCC_TL2: { turnoutList[TURNOUT_TL2].dccId = DCC_TL2; turnoutList[TURNOUT_TL2].turnoutp = tout; }  break;
      case DCC_TR2: { turnoutList[TURNOUT_TR2].dccId = DCC_TR2; turnoutList[TURNOUT_TR2].turnoutp = tout; }  break;
      case DCC_XC:  { turnoutList[TURNOUT_XC].dccId = DCC_XC; turnoutList[TURNOUT_XC].turnoutp = tout;  }break;
      default:
        Serial.printf("Error: Received unknown turnout -> %d\n", tid);
        break;
    }
  }
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void setDccLocoSpeed(int thr_idx, int speed_val, int dir)
{
  Direction   dccDir = dir ? Direction::Reverse : Direction::Forward;

  if (thr_idx >= 0 && thr_idx < NUM_THROTTLES && locoList[thr_idx])
    dccexProtocol.setThrottle(locoList[thr_idx], speed_val, dccDir);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void setDccHorn(int thr_idx, int val)
{
  if (thr_idx >= 0 && thr_idx < NUM_THROTTLES && locoList[thr_idx]) {
    if (val)
        dccexProtocol.functionOn(locoList[thr_idx], THR_HORN_TO_DCC_FN);
      else
        dccexProtocol.functionOff(locoList[thr_idx], THR_HORN_TO_DCC_FN);
  }
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void setDccTurnout(int turn_idx, int val)
{
  char  cmdStr[20];
  if (turn_idx >= 0 && turn_idx < NUM_TURNOUTS && turnoutList[turn_idx].turnoutp) {
    snprintf(cmdStr, sizeof(cmdStr), "<T %d %d>", turnoutList[turn_idx].dccId, val);
    dccexProtocol.sendCommand(cmdStr);
  }
}

//----------------------------------------------------------------------------
// Turn off esp32 and put it to sleep.
// Set all DCC throttles to 0 to stop all trains
//----------------------------------------------------------------------------

void gotoSleep()
{
  DEBUG_PRINTF("Going to Sleep.. zzzz..");
  for (int thr_idx=0; thr_idx < NUM_THROTTLES; ++thr_idx) {
    setDccLocoSpeed(thr_idx, 0, Direction::Forward);
  }

  esp_deep_sleep_start();
}

