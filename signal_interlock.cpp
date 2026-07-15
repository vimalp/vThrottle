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
      mySignalBox.loadInput(turnoutList[t], POINT);
  }

  // define all block occupancy  sensors

  // define all signals
  for (int s = 0; s < NUM_SIGNAL_HEADS; ++s) {
    mySignalBox.loadInput(signalHeads[s].dccAddr, NUM_SIGNAL_ASPECTS);
  }

  // load rules for setting signals and turnouts
  
}
