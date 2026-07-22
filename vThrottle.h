// 
// **************************************************************************
//
// vThrottle.h
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

#include "src/interlock/interlock.h"
#include "signal_interlock.h"
#include "dcc_funcs.h"


#define _DEBUG_   1
#define DEBUG_PRINTF      if (_DEBUG_) Serial.printf

// assert and block execution.
#define ASSERT_PRINTF(expression) \
    if (!(expression)) { \
      Serial.print(F("Assertion Failed: (")); \
      Serial.print(F(#expression)); \
      Serial.print(F("), file ")); \
      Serial.print(__FILE__); \
      Serial.print(F(", line ")); \
      Serial.println(__LINE__); \
      while(1); \
    }

//--------------------------------------------------------
// I2C Addresses
//--------------------------------------------------------

#define I2C_ADDR_BACKLIGHT  0x30
#define I2C_ADDR_TOUCHPAD   0x5D

extern DCCEXProtocol  dccexProtocol;

//-----------------------------------------
//-----------------------------------------

extern Loco*    locoList[NUM_THROTTLES];

//-----------------------------------------
//-----------------------------------------


//-----------------------------------------
// DCC Loco functions
#define THR_HORN_TO_DCC_FN    2     // DCC horn function
#define THR_SOUND_TO_DCC_FN   8     // DCC master sound enable

//-----------------------------------------
// extern functions
extern void display_dbg_msg(const char* msg);

extern void updateTrackPower(TrackPower trk_pwr);
extern void updateRoster();
extern void updateSensors();
extern uint16_t get_sensor_index_from_addr(uint16_t sensor_addr);
extern void getAvailLocoAddresses(uint32_t& num_avail, uint32_t* avail_loco_addresses);
extern void updateTurnouts();
extern void updateLocoBlock(uint new_block_index);
extern void show_wifi_status();
extern void sendI2CCommand(uint8_t addr, uint8_t command);
