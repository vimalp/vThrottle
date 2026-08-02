// 
// **************************************************************************
//
// dcc_funcs.ino
//
// misc utility functions for dccex_throttle
//
// Vimal Parikh 
// April, 2026
//
//
// **************************************************************************
//
#include "vThrottle.h"
#include "signal_handler.h"


//------------------------------------------------------------------
// Print function for ui_events.c
//------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

void c_serial_print(const char* message) 
{
    if (_DEBUG_)
      Serial.println(message); // C++ object called safely inside a wrapper
}

#ifdef __cplusplus
}
#endif

//------------------------------------------------------------------
// Show wifi status
//------------------------------------------------------------------

void show_wifi_status()
{
  // check if displayinh is needed
  int32_t rssi = WiFi.RSSI();

  int signalPct = 0;
  if (rssi < -100)        signalPct = 0;
  else if (rssi >= - 50)  signalPct = 100;
  else                    signalPct = 2 * (rssi + 100);

  lv_bar_set_value(ui_WifiLevel, signalPct, LV_ANIM_ON);
}

//------------------------------------------------------------------
// display_dbg_msg: Print a debug message in startup screen's 
// text area
// Since this function is mainly called from setup(),
// lv_timer_handler needs to be called to draw the text. 
//------------------------------------------------------------------

void display_dbg_msg(const char* msg)
{
  lv_textarea_add_text(ui_StartupMsgArea, msg);
  lv_timer_handler();
}
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
  // clear all old lists
  for (int thr_idx = 0; thr_idx < NUM_THROTTLES; ++thr_idx) {
      clearLocoList(thr_idx);       
  }

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

    // assign an empty throttle slot to the loco
    int tidx = 0;
    for (; tidx < NUM_THROTTLES; ++tidx) {
      if (!locoList[tidx]) break;
    }
    if (tidx < NUM_THROTTLES) {
      locoList[tidx] = loco;
      DEBUG_PRINTF("Throttle[%d] -> loco %d\n", tidx, locoAddr);
      selectLoco(tidx, loco_idx);
    }
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
    int dcc_addr = tout->getId();
    switch (dcc_addr) {
      case DCC_TL0: { turnoutList[TURNOUT_TL0] = dcc_addr; }  break;
      case DCC_TR0: { turnoutList[TURNOUT_TR0] = dcc_addr; }  break;
      case DCC_TL2: { turnoutList[TURNOUT_TL2] = dcc_addr; }  break;
      case DCC_TR2: { turnoutList[TURNOUT_TR2] = dcc_addr; }  break;
      case DCC_XC:  { turnoutList[TURNOUT_XC]  = dcc_addr; }  break;
      default:
        Serial.printf("Error: Received unknown turnout -> %d\n", dcc_addr);
        break;
    }
  }
}

//----------------------------------------------------------------------------
// Update sensor list with sensor's dcc address
// Also a check on DCC_BLK* values matching the command stations' addresses
//----------------------------------------------------------------------------
uint16_t get_sensor_index_from_addr(uint16_t sensor_addr)
{
  switch (sensor_addr) {
    case  DCC_BLK0:  return 0; 
    case  DCC_BLK1:  return 1;
    case  DCC_BLK2:  return 2;
    case  DCC_BLK3:  return 3;
    case  DCC_BLK4:  return 4;
    case  DCC_BLK5:  return 5;
    case  DCC_BLK6:  return 6; 
    case  DCC_BLK7:  return 7; 
    case  DCC_BLK8:  return 8;
    case  DCC_BLK9:  return 9;
    default:
      //DEBUG_PRINTF("Error: get_sensor_index_from_addr: unknown sensor address = %d\n", sensor_addr);
      break;
  }
  return NUM_BLOCK_SENSORS;
}



void updateSensors() 
{
  int s_idx = 0;
  for (Sensor *s = dccexProtocol.sensors->getFirst(); s; s = s->getNext()) {
    int s_addr = s->getId();
    int s_idx = get_sensor_index_from_addr(s_addr);

    if (s_idx < NUM_BLOCK_SENSORS) {
      sensorList[s_idx] = s_addr;   
      DEBUG_PRINTF("Got Sensor %d: add = %d\n", s_idx, s_addr);
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
   setDccFunc(thr_idx, THR_HORN_TO_DCC_FN, val);
 }

void setDccFunc(int thr_idx, int func_num, int val)
{
  if (thr_idx >= 0 && thr_idx < NUM_THROTTLES && locoList[thr_idx]) {
    if (val)
        dccexProtocol.functionOn(locoList[thr_idx], func_num);
      else
        dccexProtocol.functionOff(locoList[thr_idx], func_num);
  }
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void setDccTurnout(int turn_idx, int val)
{
  char  cmdStr[20];
  if (turn_idx >= 0 && turn_idx < NUM_TURNOUTS && (turnoutList[turn_idx] > 0)) {
    DEBUG_PRINTF("setDCCTurnout[%d] = %x\n", turn_idx, turnoutList[turn_idx]);
    if (val) 
      dccexProtocol.throwTurnout(turnoutList[turn_idx]);
    else
      dccexProtocol.closeTurnout(turnoutList[turn_idx]);
  }
}

//----------------------------------------------------------------------------
// Send a raw command packet to DCC-EX
//----------------------------------------------------------------------------

void startDccRoute(int thr_index, int route_index)
{
  int ridx = 0;
	Route* route; 
	for (route = dccexProtocol.routes->getFirst(); route; route = route->getNext()) {
		if (ridx++ == route_index) break;
	}
  int route_id = route->getId();
  int loco_addr  = locoList[thr_index]->getAddress();
  	
  dccexProtocol.handOffLoco(loco_addr, route_id);
}

void stopDccRoute(int route_index)
{
  char cmdStr[40];
  int ridx = 0;
	Route* route; 
	for (route = dccexProtocol.routes->getFirst(); route; route = route->getNext()) {
		if (ridx++ == route_index) break;
	}
  int route_id = route->getId();
  snprintf(cmdStr, sizeof(cmdStr), "/ KILL %d", route_id);
  sendDccExCmd(cmdStr);
}

//----------------------------------------------------------------------------
// Send a raw command packet to DCC-EX
//----------------------------------------------------------------------------

void sendDccExCmd(const char* cmd_str)
{
  DEBUG_PRINTF("SendDccCmd: %s\n", cmd_str);
  dccexProtocol.sendCommand(cmd_str);
}

//----------------------------------------------------------------------------
// Reset throttle and restar
//----------------------------------------------------------------------------

void reset()
{
  esp_restart();  
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

  // reset input touch pad 
  lv_indev_t * indev = NULL;
  for(indev = lv_indev_get_next(NULL); indev != NULL; indev = lv_indev_get_next(indev)) {
    // Check type, e.g., LV_INDEV_TYPE_POINTER, LV_INDEV_TYPE_KEYPAD, etc.
    if(lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER) {
        lv_indev_reset(indev, NULL);
        break;
    }
  }
  // Turn off backlight
  sendI2CCommand(I2C_ADDR_BACKLIGHT, 245);  // 0 is the brightest backlight.    / 245 backlight off   

  //esp_sleep_enable_ext0_wakeup(TOUCH_GT911_INT, LOW);
  esp_deep_sleep_start();
}

