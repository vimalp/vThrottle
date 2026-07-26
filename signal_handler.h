//*******************************************************************
// 
// signal_handler.h
//
// Model railroad signal control based on turnout, sensor and other
// signal states. 
// 
// THe signal handler class is instantiated per signal head.
// It defines how that signal head is set based on the turnout and
// 2 block sensors.
//
// The turnout control can be defined as active Closed or active Thrown.
// The signal head is set to aspect RED if turnout active is false.
//
// The block sensor, blk1, is the block immediately following the signal.
// if blk1 is occupied the signal is set to RED. 
//
// the block sensor, blk2 is the second block following the signal
// If blk2 is active, then signal is set to YELLOW.
//
// Authos: Vimal Parikh
// 
//*******************************************************************

#pragma once
#include "vThrottle.h"

//-------------------------------------------
enum SignalAspect_e 
{
  ASPECT_DARK = 0,
  ASPECT_RED,
  ASPECT_YELLOW,
  ASPECT_GREEN,
  NUM_ASPECTS
};

//--------------------------------------------------
// class to handle turnout value for signal update
//-------------------------------------------------
class TurnoutInput {
public:
  TurnoutInput(uint16_t t_id, bool active_closed) : _tbase(Turnout::getById(t_id)), _active_closed(active_closed) { }
  ~TurnoutInput() {}

  bool getActive()
  {
    bool isThrown = _tbase->getThrown();
    return (_active_closed ? !isThrown : isThrown);
  }

private:
  Turnout*  _tbase;   // pointer to a DCCEXProtocol Turnout object
  bool      _active_closed;
};

//--------------------------------------------------
// class to handle sensor state for signal update
//--------------------------------------------------
class SensorInput {
public:
  SensorInput(uint16_t s_id) : _tbase(Sensor::getById(s_id)) {}
  ~SensorInput() {}

  // the block sensors are active high on this layout. 
  // DCCEXProtocol returns active low.
  bool getActive()  { return !_tbase->getActive(); }

private:
  Sensor*  _tbase;   // pointer to a DCCEXProtocol Turnout object
};

//--------------------------------------------------
// Signal Handler class
// Implements signal interlock rules and sets signal 
// aspects on the layout
//--------------------------------------------------

class SignalHandler {
public:
  SignalHandler(SignalIds_e signal_id, TurnoutInput* tinput, SensorInput* blk1, SensorInput* blk2) :
               _signalId(signal_id), _tinput(tinput), _blk1(blk1), _blk2(blk2), _curAspect(ASPECT_DARK) {}
  ~SignalHandler() {}

  void updateSignal();

  // for debugging
  const char* print() 
  {
    snprintf(_debugStr, sizeof(_debugStr), "%d: turn=%x, blk1=%x, blk2=%x\n", _signalId,
            _tinput, _blk1, _blk2);
    return _debugStr;
  }

private:
  SignalIds_e       _signalId;
  TurnoutInput*     _tinput;
  SensorInput*      _blk1;
  SensorInput*      _blk2;
  SignalAspect_e    _curAspect;
  static char       _debugStr[50];
  static uint32_t   _aspectColors[NUM_ASPECTS];

  void setAspect(SignalAspect_e aspect); 
};

//--------------------------------------------------
// externals
extern void init_signal_handler();
extern void update_signals();

