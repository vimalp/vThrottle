// 
// **************************************************************************
//
// dcc_funcs.h
//
// defines common turnout index related literals.
// This file is shared beteeen throttle functions and ui_event.c
//
// **************************************************************************
//

#pragma once
#include <Arduino.h>

//----------------------------------------------------------------
// Throttle defines
//----------------------------------------------------------------

#define NUM_THROTTLES        3
#define MAX_NUM_FUNCS       16
#define NUM_TURNOUTS         5
#define NUM_BLOCK_SENSORS   10
#define NUM_SIGNAL_HEADS    10


//----------------------------------------------------------------
// Turnout defines
//----------------------------------------------------------------

// Following ids are defined for turnouts in CommandStation's myautomatioh.h
// Define macros for controlling single coil turnouts using DCC decoders
#define DCC_TL0     200
#define DCC_TL2     202
#define DCC_TR0     203
#define DCC_TR2     205
#define DCC_XC      225

// local indices for turnouts
#define TURNOUT_TL0     0
#define TURNOUT_TR0     1
#define TURNOUT_TL2     2
#define TURNOUT_TR2     3
#define TURNOUT_XC      4
#define TURNOUT_NONE    NUM_TURNOUTS

// contains dcc addresses of turnouts
extern int16_t  turnoutList[NUM_TURNOUTS];

//----------------------------------------------------------------
// block occupancy sensor list
// The DCC Addresses should match the ones in Command Station
//----------------------------------------------------------------
#define TOWN_LOOP_BLK0  0
#define TOWN_LOOP_BLK1  1
#define TOWN_LOOP_BLK2  2
#define OUT1_LOOP_BLK3  3
#define OUT1_LOOP_BLK4  4
#define OUT1_LOOP_BLK5  5
#define OUT2_LOOP_BLK6  6
#define OUT2_LOOP_BLK7  7
#define OUT2_LOOP_BLK8  8
#define SIDEING_BLK9    9
#define BLOCK_NONE      NUM_BLOCK_SENSORS

#define DCC_BLK0      100
#define DCC_BLK1      101
#define DCC_BLK2      102
#define DCC_BLK3      103
#define DCC_BLK4      104
#define DCC_BLK5      105
#define DCC_BLK6      106
#define DCC_BLK7      107
#define DCC_BLK8      108
#define DCC_BLK9      109

// constains dcc addresses (as above) of sensors

extern int16_t  sensorList[NUM_BLOCK_SENSORS];

//----------------------------------------------------------------
// DCC Addresses for signals. THis must match values in myAutomation.h
//----------------------------------------------------------------
#define SIGNAL_BASE_ADDR 400

// local indices for turnouts
enum SignalIds_e 
{
  SIGNAL_SHL0C  = 400,
  SIGNAL_SHL1TC = 401,
  SIGNAL_SHL1TD = 402,
  SIGNAL_SHL2C  = 403,
  SIGNAL_SHL2D  = 404,
  SIGNAL_SHR0TC = 405,
  SIGNAL_SHR0TD = 406,
  SIGNAL_SHR1C  = 407,
  SIGNAL_SHR2TC = 408,
  SIGNAL_SHR2TD = 409
};


//----------------------------------------------------------------
//
//----------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif


//--------------------------------------------------------
// These functions acceess the UI elements inside ui_events.c
// Hence they need C calling conventions.
//--------------------------------------------------------
extern void c_serial_print(const char* message);

extern void clearLocoList(int thr_idx); 
extern void setDccLocoSpeed(int thr_idx, int speed_val, int dir);
extern void setDccHorn(int thr_idx, int val);
extern void setDccTurnout(int turnout_idx, int val);
extern void sendDccExCmd(const char* cmd_str);

extern void setLocoList(int thr_idx, int loco_idx, const char* name, uint32_t addr);
extern void selectLoco(int thr_idx, int loco_idx);
extern void assignLocoToThrottle(int thr_idx, int loco_idx);
extern void setLocoStartBlock(uint loco_idx, uint start_blk_idx);
extern void setDccFunc(int thr_idx, int func_num, int val);
extern void gotoSleep();
extern void setSignalAspect(enum SignalIds_e signal_id, uint32_t aspect_col);
extern void setLayoutBlockHighlight(uint16_t sensor_idx, bool active);

#ifdef __cplusplus
}
#endif