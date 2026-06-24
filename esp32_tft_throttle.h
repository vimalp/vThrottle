// 
// **************************************************************************
//
// dccex_throttle.h
//
// Common include file for dccex_throttle source
//
// **************************************************************************
//

#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include <DCCEXProtocol.h>
#include <WiFi.h>
#include "my_turnout.h"


#define _DEBUG_   1
#define DEBUG_PRINTF      if (_DEBUG_) Serial.printf

//--------------------------------------------------------
// I2C Addresses
//--------------------------------------------------------

#define I2C_ADDR_BACKLIGHT  0x30
#define I2C_ADDR_TOUCHPAD   0x5D

extern DCCEXProtocol  dccexProtocol;

//-----------------------------------------
//-----------------------------------------

#define NUM_THROTTLES   3
extern Loco*    locoList[NUM_THROTTLES];

//-----------------------------------------
//-----------------------------------------

extern Turnout*   turnoutList[NUM_TURNOUTS];

//-----------------------------------------
// DCC Loco functions
#define THR_HORN_TO_DCC_FN    2     // DCC horn function
#define THR_SOUND_TO_DCC_FN   8     // DCC master sound enable

//-----------------------------------------
// extern functions
extern void updateTrackPower(TrackPower trk_pwr);
extern void updateRoster();
extern void getAvailLocoAddresses(uint32_t& num_avail, uint32_t* avail_loco_addresses);
extern void updateTurnouts();
extern void show_wifi_status();
