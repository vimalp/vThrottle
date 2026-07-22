/* interlockV0_6
  09/08/2025

  */

#include "Arduino.h"
#include "interlock.h"


// //core loop that drives the system
void interlock::runInterlock(void) {
  uint8_t _ilockError = 0;
  uint16_t q = 0;
  uint16_t a = 0;
  uint16_t myItemID = 0;
  uint16_t myItemArrayPos = 0;  //position in the inputs array
  uint16_t myCheckItemID = 0;
  uint8_t myItemStatus = 0;
  uint8_t myCheckItemStatus = 0;
  uint8_t myCheckRuleState = 0;
  //uint16_t myCheckItemArrayPos = 0;  //position in the inputs array

  monitorSignals();      //check if any monitored signals need updating
  monitorDuelSignals();  //0.6 addition check if any duel monitor signals need updating
  _currentMillis = millis();

  //_itemTypeStateArray
  //fifo buffer

  if (_currentMillis - _fifoMillis >= INTPROCSPEED) {
    if (_interlockFifoReadPos != _interlockFifoWritePos) {
      _fifoMillis = _currentMillis;

      //get the item to be cheched aganst and it's state
      myItemID = _interlockItemFifo[_interlockFifoReadPos];
      myItemStatus = _interlockInstFifo[_interlockFifoReadPos];



      for (a = 0; a < _numItemsUsed; a++) {
        if (myItemID == _itemIDArray[a]) {
          myItemArrayPos = a;
          // Serial.print("myItemArrayPos: ");
          // Serial.println(myItemArrayPos);
          a = _numItemsUsed;  //found info to exit for()
        }
      }

      //Above deals with the item now to work through the rules

      for (q = 0; q < _intNumInterlockRulesUsed; q++) {
        // Serial.print("q:");
        // Serial.println(q);
        if (myItemID == _interlockRulesItems[q][0]) {                //Is this a rule for this item
          if (myItemStatus == bitRead(_interlockRulesDirs[q], 0)) {  //now check if it's for this direction/state

            myCheckRuleState = bitRead(_interlockRulesDirs[q], 7);  //The state the item needs to be
            myCheckItemID = _interlockRulesItems[q][1];             //Get the check item ID
            myCheckItemStatus = getItemStatus(myCheckItemID);       //get the current status of the item
#if defined(RULECHECKDIAGNOSTICS_ON)
            Serial.println("  ");
            Serial.println("Rule Num ");
            Serial.println(q);
            Serial.print("ItemID ");
            Serial.println(myItemID);
            Serial.print("ItemStatus ");
            Serial.println(myItemStatus);
            Serial.print("CheckItemID ");
            Serial.println(myCheckItemID);
            Serial.print("CheckRuleState ");
            Serial.println(myCheckRuleState);
            Serial.print("CheckItemStatus ");
            Serial.println(myCheckItemStatus);
            Serial.println("  ");
#endif
            //0.6 mod...points with multi aspect signals means state may be greater than 1...causing block signalling false errors
            if(myCheckRuleState == 0){//if it's supposed to be Zero
              if (myCheckItemStatus > 0) {  //Interlock conflict
                _ilockError = 1;                            //Error found
                q = _intNumInterlockRulesUsed;              //get out of the for() loop, conflict found
              }  
            }else{//So if it's not supposed to be Zero
              if (myCheckItemStatus < 1) {  //Interlock conflict
                _ilockError = 1;                            //Error found
                q = _intNumInterlockRulesUsed;              //get out of the for() loop, conflict found
              } 
            }
            // if (myCheckRuleState != myCheckItemStatus) {  //Interlock conflict
            //   _ilockError = 1;                            //Error found
            //   q = _intNumInterlockRulesUsed;              //get out of the for() loop, conflict found
            // }
          }
        }
      }
      // //Serial.println(getItemTypeString(myItemID));
      if (_ilockError == 1) {
        intErrorTone();
        intCheckResults(_ilockError, myItemID, myItemStatus, getItemTypeString(myItemID), myCheckItemID, myCheckItemStatus, getItemTypeString(myCheckItemID));
      } else {
        intCheckResults(_ilockError, myItemID, myItemStatus, getItemTypeString(myItemID), 0, 0, " ");
      }
      if (_modeState == 0) {                                    //Training
        _itemTypeStateArray[myItemArrayPos][2] = myItemStatus;  //update status
        intUpdateLayout(myItemID, myItemStatus);                // data sent to custom function for processing
      } else {
        if (_ilockError == 0) {
          _itemTypeStateArray[myItemArrayPos][2] = myItemStatus;  //update status
          intUpdateLayout(myItemID, myItemStatus);
        }
      }
       _ilockError = 0;
      _interlockFifoReadPos++;
    }
  }
}

//0.6 addition
void interlock::monitorDuelSignals() {
  uint16_t impactedSignalID = 0;
  uint8_t impactedSignalState = 0;
  uint8_t impactedSignalAspects = 0;
  uint8_t newImpactedSignalState = 0;
  uint16_t monitoredSignal1ID = 0;
  uint16_t monitoredSignal2ID = 0;
  uint8_t monitoredSignalState = 0;
  uint8_t monitoredSignal1State = 0;
  uint8_t monitoredSignal2State = 0;
  uint8_t updatedSignals = 0;
  for (uint16_t q = 0; q < _numDuelMonitorsUsed; q++) {  //work through all the rules and see if anything is to have signal value increased
    impactedSignalID = _monitorDuelRulesItems[q][0];
    impactedSignalState = getItemStatus(impactedSignalID);
    if (impactedSignalState > 0) {  //only update signals that are NOT red/Stop
      impactedSignalAspects = getSignalNumAspects(impactedSignalID);
      monitoredSignal1ID = _monitorDuelRulesItems[q][1];
      monitoredSignal2ID = _monitorDuelRulesItems[q][2];
      monitoredSignal1State = getItemStatus(monitoredSignal1ID);
      monitoredSignal2State = getItemStatus(monitoredSignal2ID);
      monitoredSignalState = monitoredSignal1State;
      if(monitoredSignal2State > monitoredSignalState){
        monitoredSignalState = monitoredSignal2State;  
      }
      if (impactedSignalState != (monitoredSignalState + 1)) {  //signal should always be one state higher than the next

        newImpactedSignalState = monitoredSignalState + 1;

        if (newImpactedSignalState > (impactedSignalAspects - 1)) {
          newImpactedSignalState = impactedSignalAspects - 1;
        }

        if (newImpactedSignalState != impactedSignalState) {  //check the value is smaller than number of aspects 5 aspect max value is 4 (red = 0)
          // Serial.print("isID: ");
          // Serial.print(impactedSignalID);
          // Serial.print(" s: ");
          // Serial.print(impactedSignalState);
          // Serial.print(" Asp: ");
          // Serial.print(impactedSignalAspects);
          // Serial.print(" mID: ");
          // Serial.print(monitoredSignalID);
          // Serial.print(" mState: ");
          // Serial.println(monitoredSignalState);
          //  Serial.print(" impactedSignalState: ");
          //  Serial.println(impactedSignalState);
          setItemStatus(impactedSignalID, newImpactedSignalState);
          intUpdateLayout(impactedSignalID, newImpactedSignalState);
          updatedSignals++;
        }
      }
    }
  }

  if (updatedSignals > 0) {

    Serial.print("Duel updatedSignals: ");
    Serial.println(updatedSignals);
#if defined(SIGNALMONITORDIAGNOSTICS_ON)
    listInputs();
#endif
  }
}


void interlock::monitorSignals() {
  uint16_t impactedSignalID = 0;
  uint8_t impactedSignalState = 0;
  uint8_t newImpactedSignalState = 0;
  uint8_t impactedSignalAspects = 0;
  uint16_t monitoredSignalID = 0;
  uint8_t monitoredSignalState = 0;
  uint8_t updatedSignals = 0;
  for (uint16_t q = 0; q < _numMonitorsUsed; q++) {  //work through all the rules and see if anything is to have signal value increased
    impactedSignalID = _monitorRulesItems[q][0];
    impactedSignalState = getItemStatus(impactedSignalID);
    if (impactedSignalState > 0) {  //only update signals that are NOT red/Stop
      monitoredSignalID = _monitorRulesItems[q][1];
      monitoredSignalState = getItemStatus(monitoredSignalID);
      impactedSignalAspects = getSignalNumAspects(impactedSignalID);
#if defined(SIGNALMONITORDIAGNOSTICS_ON)
      delay(500);  //just to make text readable
      Serial.print("isID: ");
      Serial.print(impactedSignalID);
      Serial.print(" s: ");
      Serial.print(impactedSignalState);
      Serial.print(" Asp: ");
      Serial.print(impactedSignalAspects);
      Serial.print(" mID: ");
      Serial.print(monitoredSignalID);
      Serial.print(" mState: ");
      Serial.println(monitoredSignalState);
#endif
      //if(impactedSignalState != (monitoredSignalState + 1) && impactedSignalState < impactedSignalAspects){//signal should always be one state higher than the next
      if (impactedSignalState != (monitoredSignalState + 1)) {  //signal should always be one state higher than the next

        newImpactedSignalState = monitoredSignalState + 1;

        if (newImpactedSignalState > (impactedSignalAspects - 1)) {
          newImpactedSignalState = impactedSignalAspects - 1;
        }

        if (newImpactedSignalState != impactedSignalState) {  //check the value is smaller than number of aspects 5 aspect max value is 4 (red = 0)
          // Serial.print("isID: ");
          // Serial.print(impactedSignalID);
          // Serial.print(" s: ");
          // Serial.print(impactedSignalState);
          // Serial.print(" Asp: ");
          // Serial.print(impactedSignalAspects);
          // Serial.print(" mID: ");
          // Serial.print(monitoredSignalID);
          // Serial.print(" mState: ");
          // Serial.println(monitoredSignalState);
          //  Serial.print(" impactedSignalState: ");
          //  Serial.println(impactedSignalState);
          setItemStatus(impactedSignalID, newImpactedSignalState);
          intUpdateLayout(impactedSignalID, newImpactedSignalState);
          updatedSignals++;
        }
      }
    }
  }

  if (updatedSignals > 0) {

    Serial.print("updatedSignals: ");
    Serial.println(updatedSignals);
#if defined(SIGNALMONITORDIAGNOSTICS_ON)
    listInputs();
#endif
  }
}



uint8_t interlock::getSignalNumAspects(uint16_t itemID) {
  byte aspects = 0;
  uint16_t q = 0;
  for (q = 0; q < _numItemsUsed; q++) {
    if (itemID == _itemIDArray[q]) {
      aspects = _itemTypeStateArray[q][1];  //I've got the number of aspects
      // Serial.println(_itemTypeStateArray[q][0]);
      // Serial.println(_itemTypeStateArray[q][1]);
      // Serial.println(_itemTypeStateArray[q][2]);
      // listInputs();
      q = _numItemsUsed;
    }
  }
  return aspects;
}

void interlock::setItemStatus(uint16_t itemID, uint8_t itemState) {
  uint16_t q = 0;
  for (q = 0; q < _numItemsUsed; q++) {
    if (itemID == _itemIDArray[q]) {
      _itemTypeStateArray[q][2] = itemState;  //set the state
      q = _numItemsUsed;
    }
  }
}

byte interlock::getItemStatus(uint16_t itemID) {
  byte itemStatus = 0;
  uint16_t q = 0;
  for (q = 0; q < _numItemsUsed; q++) {
    if (itemID == _itemIDArray[q]) {
      itemStatus = _itemTypeStateArray[q][2];  //I've got the status
      q = _numItemsUsed;
    }
  }
  return itemStatus;
}

uint8_t interlock::getItemType(uint16_t itemID) {
  uint8_t typeID = 255;
  uint16_t q = 0;
  for (q = 0; q < _numItemsUsed; q++) {
    if (itemID == _itemIDArray[q]) {
      typeID = _itemTypeStateArray[q][0];  //I've got the type ID
      q = _numItemsUsed;
    }
  }
  return typeID;
}


String interlock::getItemTypeString(uint16_t itemID) {
  String myString = "12345678";
  uint16_t q = 0;
  //uint16_t itemTypeID = 0;
  // Serial.print("i: ");
  // Serial.println(itemID);
  for (q = 0; q < _numItemsUsed; q++) {
    if (itemID == _itemIDArray[q]) {
      //itemTypeID = _itemTypeStateArray[q][0];  //I've got the type ID
      myString = myItemNamesArray[_itemTypeStateArray[q][0]];
      q = _numItemsUsed;
    }
  }
  // Serial.print("it: ");
  // Serial.println(itemTypeID);
  // Serial.println(myString);
  return myString;
}

//Inserts a move to test into the fifo buffer
void interlock::checkInterlocks(uint16_t itemID, uint8_t checkItemStatus) {
  _interlockItemFifo[_interlockFifoWritePos] = itemID;
  _interlockInstFifo[_interlockFifoWritePos] = checkItemStatus;
  //Serial.println(_interlockFifoWritePos);
  _interlockFifoWritePos++;
}



//Serial print the entered rules
void interlock::listRules(void) {
  Serial.print(_intNumInterlockRulesUsed);
  Serial.println(" Rules");
  for (uint16_t q = 0; q < _intNumInterlockRulesUsed; q++) {
    Serial.print(_interlockRulesItems[q][0]);
    Serial.print(colonstring);
    Serial.print(bitRead(_interlockRulesDirs[q], 0));
    Serial.print(colonstring);
    Serial.print(_interlockRulesItems[q][1]);
    Serial.print(colonstring);
    Serial.print(bitRead(_interlockRulesDirs[q], 7));
    Serial.println(" ");
  }
}

//Serial print the inputs
void interlock::listInputs() {
  Serial.print(_numItemsUsed);
  Serial.println(" Inputs");
  for (uint16_t q = 0; q < _numItemsUsed; q++) {
    Serial.print("ItemID: ");
    Serial.print(_itemIDArray[q]);
    Serial.print(colonstring);
    Serial.print(_itemTypeStateArray[q][0]);
    Serial.print(colonstring);
    Serial.print(myItemNamesArray[_itemTypeStateArray[q][0]]);
    if (_itemTypeStateArray[q][0] == 0) {  //If it's a signal
      Serial.print(" Aspects: ");
      Serial.print(_itemTypeStateArray[q][1]);
    }
    Serial.print(colonstring);
    Serial.print(_itemTypeStateArray[q][2]);
    Serial.println(" ");
  }
}

//Serial prints the inputs types
void interlock::listInputTypes(void) {
  Serial.println("Input List");
  for (uint8_t q = 0; q < _numInputTypes; q++) {
    Serial.print(q);
    Serial.print(colonstring);
    Serial.println(myItemNamesArray[q]);
  }
}

//0.6 addition
//This function loads duel signal monitoring for block signalling, allows a signal to monitor 2 signals, for diverging tracks
//itemID will have it's state changed if the state of monitor1ID or Monitor2 changes... one way effect.
void interlock::loadSignalDuelMonitors(uint16_t itemID, uint16_t monitor1ID, uint16_t monitor2ID) {
  byte funcError = 0;
  uint16_t q = 0;
  // Serial.println("loadSignalDuelMonitors");
  // Serial.print(itemID);
  // Serial.print(" : ");
  // Serial.print(monitor1ID);
  // Serial.print(" : ");
  // Serial.println(monitor2ID);

  // Serial.print(getItemType(itemID));
  // Serial.print(" : ");
  // Serial.println(getItemType(monitorID));
  // Serial.println(" ");
  if (getItemType(itemID) != 0) {
    funcError = 1;
    Serial.print(errorstring);
    Serial.print(13);
    Serial.print(colonstring);
    Serial.println(itemID);
    intErrorTone();
  }
  if (getItemType(monitor1ID) != 0) {
    funcError = 1;
    Serial.print(errorstring);
    Serial.print(14);
    Serial.print(colonstring);
    Serial.println(monitor1ID);
    intErrorTone();
  }
  if (getItemType(monitor2ID) != 0) {
    funcError = 1;
    Serial.print(errorstring);
    Serial.print(15);
    Serial.print(colonstring);
    Serial.println(monitor2ID);
    intErrorTone();
  }
  if (_numDuelMonitorsUsed >= INTDUELMONITORBLOCKSIGNALS) {  //too many monitor rules for array
    funcError = 1;
    Serial.print(errorstring);
    Serial.print(16);
    Serial.print(colonstring);
    Serial.println(itemID);
    intErrorTone();
  }
  for (q = 0; q < _numDuelMonitorsUsed; q++) {
    if (_monitorDuelRulesItems[q][0] == itemID) {
      funcError = 1;
      Serial.print(errorstring);
      Serial.print(17);
      Serial.print(colonstring);
      Serial.println(itemID);
      intErrorTone();
    }
    if (_monitorDuelRulesItems[q][1] == monitor1ID) {
      funcError = 1;
      Serial.print(errorstring);
      Serial.print(18);
      Serial.print(colonstring);
      Serial.println(monitor1ID);
      intErrorTone();
    }
    if (_monitorDuelRulesItems[q][2] == monitor2ID) {
      funcError = 1;
      Serial.print(errorstring);
      Serial.print(19);
      Serial.print(colonstring);
      Serial.println(monitor2ID);
      intErrorTone();
    }
  }
  if (funcError == 0) {  //no erros insert rule
    
    _monitorDuelRulesItems[_numDuelMonitorsUsed][0] = itemID;
    _monitorDuelRulesItems[_numDuelMonitorsUsed][1] = monitor1ID;
    _monitorDuelRulesItems[_numDuelMonitorsUsed][2] = monitor2ID;
    _numDuelMonitorsUsed++;
  }
}

//0.6 additions
void interlock::listDuelMonitors(void) {
  Serial.println("duel monitor Rules");
  for (uint16_t q = 0; q < _numDuelMonitorsUsed; q++) {
    Serial.print(_monitorDuelRulesItems[q][0]);
    Serial.print(colonstring);
    Serial.print(_monitorDuelRulesItems[q][1]);
    Serial.print(colonstring);
    Serial.println(_monitorDuelRulesItems[q][2]);
  }
}

//This function loadssignal monitoring for block signalling
//itemID will have it's state changed if the state of monitorID changes... one way effect.
void interlock::loadSignalMonitors(uint16_t itemID, uint16_t monitorID) {
  byte funcError = 0;
  uint16_t q = 0;
  // Serial.println("loadSignalMonitors");
  // Serial.print(itemID);
  // Serial.print(" : ");
  // Serial.println(monitorID);

  // Serial.print(getItemType(itemID));
  // Serial.print(" : ");
  // Serial.println(getItemType(monitorID));
  // Serial.println(" ");
  if (getItemType(itemID) != 0) {
    funcError = 1;
    Serial.print(errorstring);
    Serial.print(8);
    Serial.print(colonstring);
    Serial.println(itemID);
    intErrorTone();
  }
  if (getItemType(monitorID) != 0) {
    funcError = 1;
    Serial.print(errorstring);
    Serial.print(9);
    Serial.print(colonstring);
    Serial.println(monitorID);
    intErrorTone();
  }
  if (_numMonitorsUsed >= INTMONITORBLOCKSIGNALS) {  //too many monitor rules for array
    funcError = 1;
    Serial.print(errorstring);
    Serial.print(10);
    Serial.print(colonstring);
    Serial.println(itemID);
    intErrorTone();
  }
  for (q = 0; q < _numMonitorsUsed; q++) {
    if (_monitorRulesItems[q][0] == itemID) {
      funcError = 1;
      Serial.print(errorstring);
      Serial.print(11);
      Serial.print(colonstring);
      Serial.println(itemID);
      intErrorTone();
    }
    //Reduced to a warning from Version 0.6 to allow for double monitors (converge)
    if (_monitorRulesItems[q][1] == monitorID) {
      //funcError = 1;
      Serial.print(errorstring);
      Serial.print(12);
      Serial.print(colonstring);
      Serial.println(monitorID);
      //intErrorTone();
    }
  }
  if (funcError == 0) {  //no erros insert rule
    _monitorRulesItems[_numMonitorsUsed][0] = itemID;
    _monitorRulesItems[_numMonitorsUsed][1] = monitorID;
    _numMonitorsUsed++;
  }
}

void interlock::listMonitors(void) {
  Serial.println("monitor Rules");
  for (uint16_t q = 0; q < _numMonitorsUsed; q++) {
    Serial.print(_monitorRulesItems[q][0]);
    Serial.print(colonstring);
    Serial.println(_monitorRulesItems[q][1]);
  }
}

void interlock::loadRule(uint16_t itemID, uint8_t itemStatus, uint16_t checkItemID, uint8_t checkItemStatus) {  //Inserts rules into system
  if (_intNumInterlockRulesUsed >= INTNUMINTERLOCKRULES) {
    Serial.print(errorstring);
    Serial.print(7);
    Serial.print(colonstring);
    Serial.println(itemID);
    intErrorTone();
  } else {
    _interlockRulesItems[_intNumInterlockRulesUsed][0] = itemID;
    _interlockRulesItems[_intNumInterlockRulesUsed][1] = checkItemID;
    if (itemStatus > 0) {
      bitSet(_interlockRulesDirs[_intNumInterlockRulesUsed], 0);  //Set bit 0
    } else {
      bitClear(_interlockRulesDirs[_intNumInterlockRulesUsed], 0);  //clear bit 0
    }
    if (checkItemStatus > 0) {
      bitSet(_interlockRulesDirs[_intNumInterlockRulesUsed], 7);  //set bit 7
    } else {
      bitClear(_interlockRulesDirs[_intNumInterlockRulesUsed], 7);  // clear bit 7
    }
    _intNumInterlockRulesUsed++;
  }
}

//loads signals
void interlock::loadSignal(uint16_t itemID, uint8_t aspects) {
  uint16_t q = 0;
  byte funcError = 0;
  if (aspects < 2 || aspects > 5) {  //Allows for 5 aspect signalling
    Serial.print(errorstring);
    Serial.print(2);
    Serial.print(colonstring);
    Serial.println(itemID);
    if (intErrorTone) {
      intErrorTone();
    }
  } else {
    if (_numItemsUsed >= INTNUMINPUTS) {
      Serial.print(errorstring);
      Serial.print(4);
      Serial.print(colonstring);
      Serial.println(itemID);
      if (intErrorTone) {
        intErrorTone();
      }
    } else {
      for (q = 0; q < _numItemsUsed; q++) {  //work through existing values to make sure no duplicates
        if (_itemIDArray[q] == itemID) {
          funcError = 1;
          Serial.print(errorstring);
          Serial.print(3);
          Serial.print(colonstring);
          Serial.println(itemID);
          if (intErrorTone) {
            intErrorTone();
          }
        }
      }
      if (funcError < 1) {  //no errors
        _itemIDArray[_numItemsUsed] = itemID;
        _itemTypeStateArray[_numItemsUsed][0] = 0;
        _itemTypeStateArray[_numItemsUsed][1] = aspects;  //number of states = 2 ON/OFF
        _itemTypeStateArray[_numItemsUsed][2] = 0;        //default OFF status value
        // Serial.print(_itemIDArray[_numItemsUsed]);
        // Serial.print(" : ");
        // Serial.print(_numItemsUsed);
        // Serial.print(" : ");
        // Serial.print(_itemTypeStateArray[_numItemsUsed][0]);
        // Serial.print(_itemTypeStateArray[_numItemsUsed][1]);
        // Serial.println(_itemTypeStateArray[_numItemsUsed][2]);
        _numItemsUsed++;
      }
    }
  }
}

//loads all inputs except signals
void interlock::loadInput(uint16_t itemID, uint8_t inputTypeID) {
  uint16_t q;
  byte funcError = 0;
  if (_numItemsUsed >= INTNUMINPUTS) {
    Serial.print(errorstring);
    Serial.print(6);
    Serial.print(colonstring);
    Serial.println(itemID);
    if (intErrorTone) {
      intErrorTone();
    }
  } else {
    for (q = 0; q < _numItemsUsed; q++) {  //work through existing values to make sure no duplicates
      if (_itemIDArray[q] == itemID) {
        funcError = 1;
        Serial.print(errorstring);
        Serial.print(5);
        Serial.print(colonstring);
        Serial.println(itemID);
        if (intErrorTone) {
          intErrorTone();
        }
      }
    }
    if (funcError < 1) {  //no errors
      _itemIDArray[_numItemsUsed] = itemID;
      _itemTypeStateArray[_numItemsUsed][0] = inputTypeID;
      _itemTypeStateArray[_numItemsUsed][1] = 2;  //number of states = 2 ON/OFF
      _itemTypeStateArray[_numItemsUsed][2] = 0;  //default OFF status value
      // Serial.print(_itemIDArray[_numItemsUsed]);
      // Serial.print(" : ");
      // Serial.print(_numItemsUsed);
      // Serial.print(" : ");
      // Serial.print(_itemTypeStateArray[_numItemsUsed][0]);
      // Serial.print(_itemTypeStateArray[_numItemsUsed][1]);
      // Serial.println(_itemTypeStateArray[_numItemsUsed][2]);
      _numItemsUsed++;
    }
  }
}

void interlock::setModestate(uint8_t mode) {
  if (mode < 1) {
    _modeState = 0;
  } else {
    _modeState = 1;
  }
  if (interlockModeState) {
    interlockModeState(_modeState);
  }
}

void interlock::init() {
  _modeState = STARTMODE;  //get the start up mode state
  if (interlockModeState) {
    interlockModeState(_modeState);
  }
  if (intErrorTone) {
    intErrorTone();
  }
}

interlock::interlock(){};