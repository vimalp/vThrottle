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
      DEBUG_PRINTF("\nReceived Track Power: %d\n", state);
      updateTrackPower(state);
    }

    void receivedRosterList() override {
      DEBUG_PRINTF("\nReceived Roster\n");
      updateRoster();
    }

    void receivedTurnoutList() override {
      DEBUG_PRINTF("\nReceived Turnout List\n");
      updateTurnouts();
    }

    void receivedSensorList() override {
      int sensorCnt = dccexProtocol.getSensorCount();
      DEBUG_PRINTF("\nReceived Sensor List: Number of sensors=%d\n", sensorCnt);
      if (sensorCnt < NUM_BLOCK_SENSORS) {
        DEBUG_PRINTF(".. Waiting more more sensors...\n");
        return;
      }
      updateSensors();
    }

   void receivedSensorState(int sensor_addr, bool active) 
   {
      DEBUG_PRINTF("\nReceived Sensor State: addr=%d, val=%d\n", sensor_addr, active);
      uint16_t sensor_idx = get_sensor_index_from_addr(sensor_addr);
      setLayoutBlockHighlight(sensor_idx, active);
      interlock_update_sensor(sensor_idx);

   }


    void receivedMessage(const char *message) {
      DEBUG_PRINTF("Received Message: %s\n", message);
    }

};