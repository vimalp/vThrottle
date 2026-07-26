// 
// **************************************************************************
//
// delegate.h
//
// defines the delegate class for receving data from DCC-ex command station.
// The delegate class derives from DCCExProtocolDelegate and implements all
// relevant methods. 
// The DCCEx protocol object calls the delegate methods when a new response
// is received. The delegate class handles the incoming data and lists
//
// **************************************************************************
//

#pragma once

class MyDelegate : public DCCEXProtocolDelegate 
{
  public:
    void receivedTrackPower(TrackPower state) override {
      DEBUG_PRINTF("Received Track Power: %d\n", state);
      updateTrackPower(state);
    }

    //-------------------------------------------------------
    // Roster list
    void receivedRosterList() override {
      DEBUG_PRINTF("Received Roster\n");
      updateRoster();
    }

    //-------------------------------------------------------
    // Turnout List
    void receivedTurnoutList() override {
      DEBUG_PRINTF("Received Turnout List\n");
      updateTurnouts();
    }

    void receivedTurnoutAction(int turnoutId, bool thrown)
    {
      DEBUG_PRINTF("Received Turnout Action: id=%d, thrown=%d\n", turnoutId, thrown);
      update_signals();
    }

    //-------------------------------------------------------
    // Sensor List updates
    void receivedSensorList() override {
      int sensorCnt = dccexProtocol.getSensorCount();
      DEBUG_PRINTF("\nReceived Sensor List: Number of sensors=%d\n", sensorCnt);
      if (sensorCnt < NUM_BLOCK_SENSORS) {
        DEBUG_PRINTF(".. Waiting more more sensors...\n");
        return;
      }
      updateSensors();
    }

   void receivedSensorState(int sensor_addr, bool active_low) 
   {
      // the dcc-ex always reports sensor pin low as active. 
      // But the block detector on this layout is active high. so we need to flip the sensor value.
      bool active = !active_low;
      uint16_t sensor_idx = get_sensor_index_from_addr(sensor_addr);
      DEBUG_PRINTF("\nReceived Sensor %d State: addr=%d, val=%d\n", sensor_idx, sensor_addr, active);
      if (sensor_idx < NUM_BLOCK_SENSORS) {
        updateLocoBlock(sensor_idx);
        setLayoutBlockHighlight(sensor_idx, active);
        update_signals();
      }
   }


    void receivedMessage(const char *message) {
      DEBUG_PRINTF("Received Message: %s\n", message);
    }

};