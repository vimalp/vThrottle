//-----------------------------------------------------------------
//
// signal_interlock.h
//
// This module uses DigitalTown.uk's interlock++ library to set signal aspects
// based on interlock between other signals, turnout states and block occupancy
//
// Vimal Parikh
// July 2026
//
//-----------------------------------------------------------------
#pragma once

//-----------------------------------------------------
// Defines for different object types
//#define TSIGNAL 0 //The definition SIGNAL CANNOT be used as it is ised by the Arduino library...may need removing
//-----------------------------------------------------
#define POINT 1
#define TURNOUT 2
#define BLOCK 3
#define CROSSING 4
#define OTHER1 5  //Change definition name as required
#define OTHER2 6  //Change definition name as required
#define OTHER3 7  //Change definition name as required

//--------------------------------------------------------
// object state values
//--------------------------------------------------------
#define POINTLEFT 0  //I define left and right as viewed from the point frog, change definition names to suit
#define POINTRIGHT 1
#define TURNOUTLEFT 0  //I define left and right as viewed from the point frog, change definition names to suit
#define TURNOUTRIGHT 1
#define SIGST 0       //signal stop (red)
#define SIGGO 1       //Green/yellow/ Go
#define CROSSINGST 0  //crossing stop
#define CROSSINGGO 1  //crossing Go
#define OTHERST 0     //other stop
#define OTHERGO 1     //other Go
#define BLOCKCLEAR 0
#define BLOCKOCCUPIED 1

//--------------------------------------------------------
// exported function
//--------------------------------------------------------
extern void interlock_update_sensor(uint16_t sensor_idx);

// turnout_index: index to turnoutList
// thrown:  true if turnout was thrown, false if closed.
extern void interlock_update_turnout(uint16_t turnout_index, bool thrown);
