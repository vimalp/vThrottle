// **************************************************************************
//
// autorun.h
//
// Defines and methods for autorun of trains
//
// **************************************************************************
//

#pragma once
#include "dcc_funcs.h"

//-----------------------------------------------------------
// define block connectivity 
// each block is connected to another block either
// directly or via a turnout
// if connected via turnout, then it can be connected
// to two blocks (for closed and thrown turnout)
//-----------------------------------------------------------

typedef struct {
  uint    turnoutIdx;  // use TURNOUT_NONE if not connected to turnout
  uint    blkIdxClosed; // block connectee if turnout is closed or none
  uint    blkIdxThrown; // block connected if turnout is thrown
} block_connect_entry_t;

extern block_connect_entry_t   blockLayout[NUM_BLOCK_SENSORS];
extern int locoInBlock[NUM_BLOCK_SENSORS];
