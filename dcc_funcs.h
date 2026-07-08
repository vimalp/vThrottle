// 
// **************************************************************************
//
// my_tuenout.h
//
// defines common turnout index related literals.
// This file is shared beteeen throttle functions and ui_event.c
//
// **************************************************************************
//

#pragma once

//----------------------------------------------------------------
// Turnout defines
//----------------------------------------------------------------

#define NUM_TURNOUTS    5
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

//----------------------------------------------------------------
// Signal Head defines
//----------------------------------------------------------------
#define NUM_SIGNAL_HEADS    10
#define NUM_SIGNAL_ASPECTS  4   

// local indices for turnouts
#define SIGNAL_SHL0C     0
#define SIGNAL_SHL1TC    1
#define SIGNAL_SHL1TD    2
#define SIGNAL_SHL2C     3
#define SIGNAL_SHL2D     4

#define SIGNAL_SHR0TC    5
#define SIGNAL_SHR0TD    6
#define SIGNAL_SHR1C     7
#define SIGNAL_SHR2TC    8
#define SIGNAL_SHR2TD    9

typedef struct {
  uint16_t    dccAddr;
  uint8_t     aspect;     // current aspect 
} signalHead_t;

//----------------------------------------------------------------
//
//----------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_THROTTLES     3
#define MAX_NUM_FUNCS     16
extern uint8_t curFuncVals[NUM_THROTTLES][MAX_NUM_FUNCS];

extern signalHead_t signalHeads[NUM_SIGNAL_HEADS];
extern uint32_t AspectCols[NUM_SIGNAL_ASPECTS];

//--------------------------------------------------------
// debug print macros
//--------------------------------------------------------
extern void c_serial_print(const char* message);

extern void clearLocoList(int thr_idx); 
extern void setDccLocoSpeed(int thr_idx, int speed_val, int dir);
extern void setDccHorn(int thr_idx, int val);
extern void setDccTurnout(int turnout_idx, int val);
extern void setDccSignal(int signal_idx, uint8_t aspect_val);
extern void sendDccExCmd(const char* cmd_str);

extern void setLocoList(int thr_idx, int loco_idx, const char* name, uint32_t addr);
extern void selectLoco(int thr_idx, int loco_idx);
extern void assignLocoToThrottle(int thr_idx, int loco_idx);
extern void setDccFunc(int thr_idx, int func_num, int val);
extern void gotoSleep();
extern void setSignalAspect(uint8_t signal_idx, uint8_t aspect_val);

#ifdef __cplusplus
}
#endif