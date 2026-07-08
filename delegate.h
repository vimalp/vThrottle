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

    void receivedMessage(const char *message) {
      DEBUG_PRINTF("Received Message: %s\n", message);
    }

};