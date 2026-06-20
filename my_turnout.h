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
#define NUM_TURNOUTS    5
// Following ids are defined for turnouts in CommandStation's myautomatioh.h
// Define macros for controlling single coil turnouts using DCC decoders
#define DCC_TL0     200
#define DCC_TL1     201     // ganged with TL0
#define DCC_TL2     202
#define DCC_TR0     203
#define DCC_TR1     204     // gangaed with TR0
#define DCC_TR2     205
#define DCC_TR3     206     // ganged with TR3
#define DCC_XC      225

// local indices for turnouts
#define TURNOUT_TL0     0
#define TURNOUT_TR0     1
#define TURNOUT_TL2     2
#define TURNOUT_TR2     3
#define TURNOUT_XC      4

#ifdef __cplusplus
extern "C" {
#endif

extern void setDccLocoSpeed(int thr_idx, int speed_val, int dir);
extern void setDccHorn(int thr_idx, int val);
extern void setDccTurnout(int turnout_idx, int val);

#ifdef __cplusplus
}
#endif