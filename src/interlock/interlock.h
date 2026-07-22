/* interlockV0_6
  09/08/2025

  */
#ifndef interlock_h  //header guard to stop it being imported twice
#define interlock_h

#include "Arduino.h"
#include "interlock_setup.h"  //configuration file

class interlock {
public:
  interlock();
  void init(); 
  void runInterlock(void);
  
  void loadInput(uint16_t  itemID, uint8_t inputTypeID);//Inserts input types into system
  void loadSignal(uint16_t itemID, uint8_t aspects); //Inserts signals and number of aspects
  void loadRule(uint16_t  itemID, uint8_t itemStatus, uint16_t  checkItemID, uint8_t checkItemStatus);//Inserts rules into system
  void loadSignalMonitors(uint16_t  itemID, uint16_t monitorID);//Inserts signal monitor rules
  //0.6 addition
  void loadSignalDuelMonitors(uint16_t  itemID, uint16_t monitor1ID, uint16_t monitor2ID);//Inserts duel signal monitor rules
  void listInputTypes(void);
  void listInputs(void);
  void listRules(void);
  void listMonitors(void);
  //0.6 addition
  void listDuelMonitors(void);
  void setModestate(uint8_t mode);

  void checkInterlocks(uint16_t  itemID, uint8_t checkItemStatus); // Item interlock to test
  //0.6 made public
  byte getItemStatus(uint16_t itemID);//get the items current position status from array


private:
  unsigned long _currentMillis;
  uint8_t _modeState;
  char errorstring[8] = "Error: ";
  char colonstring[4] = " : ";
  
 //Fixed input types 
  const uint8_t _numInputTypes = 8;  //Number of inputTypes in system
  char myItemNamesArray[8][9] = {
    "Signal",   //0
    "Point",    //1
    "Turnout",  //2
    "Block",    //3
    "Crossing", //4
    "Other1",   //5
    "Other2",   //6
    "Other3"    //7
  }; 

  //Number of inputs INTNUMINPUTS
  uint16_t _numItemsUsed;
  uint16_t _itemIDArray[INTNUMINPUTS];
  uint8_t _itemTypeStateArray[INTNUMINPUTS][3];//Type in first element, number of states (signal heads), state in 3rd
  
  // //Load rules... 2 arrays to reduce SRAM
  
  uint16_t _intNumInterlockRulesUsed;
  uint16_t _interlockRulesItems[INTNUMINTERLOCKRULES][2];//itemID  item to be checked against
  uint8_t _interlockRulesDirs[INTNUMINTERLOCKRULES];//Bit 0 will hold itemID state, bit 7 will hold checkItemID rule state

  // //FIFO
  uint16_t _interlockItemFifo[256];  //Fifobuffer item store
  uint8_t _interlockInstFifo[256];  //Fifobuffer instruction store
  uint8_t _interlockFifoReadPos;
  uint8_t _interlockFifoWritePos;
  unsigned long _fifoMillis;

  uint8_t getSignalNumAspects(uint16_t itemID);//gets the number of aspects for a signal
  uint8_t getItemType(uint16_t itemID); //gets the inputTypeDI
  String getItemTypeString(uint16_t itemID); //Gets the item type of an item
  //byte getItemStatus(uint16_t itemID);//get the items current position status from array

  void setItemStatus(uint16_t itemID, uint8_t itemState);

  //Signal Monitors
  uint16_t _numMonitorsUsed;
  uint16_t _monitorRulesItems[INTMONITORBLOCKSIGNALS][2];
  void monitorSignals();

  //Duel signal monitors 0.6 INTDUELMONITORBLOCKSIGNALS
  uint16_t _numDuelMonitorsUsed;
  uint16_t _monitorDuelRulesItems[INTDUELMONITORBLOCKSIGNALS][3];
  void monitorDuelSignals();
};

#if defined(__cplusplus)
extern "C" {
#endif

  
  //These are user functions that they may or may not use

  extern void interlockModeState(uint8_t modeState) __attribute__((weak));//Sends mode to layout 0 = Training, 1 = interlock
  extern void intErrorTone(void) __attribute__((weak));//call the errorTome function to allow users to add tone
  extern void intUpdateLayout(uint16_t itemID, uint8_t instruction) __attribute__((weak));//sends instrcution to control layout
  extern void intCheckResults(uint8_t interlockState,uint16_t itemID,uint8_t instruction,String itemName,uint16_t checkItemID,uint8_t checkInstruction,String checkItemName) __attribute__((weak));//Sends mode to layout 0 = Training, 1 = interlock

  
#if defined(__cplusplus)
}
#endif


#endif