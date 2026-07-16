//******************************************************************
//
// Model Rail Throttle controller 
// Uses esp32-s3 based lcd panel as control display
// Uses dcc-ex protocol to communicate with command station
// Uses wifi to connect to command station.
// Uses Squareline Studio to manage and design the UI
//
//  Vimal Parikh
//. June 2026
//
//******************************************************************

#include "vThrottle.h"

#include "pins_config.h"
#include "LovyanGFX_Driver.h"
#include <lvgl.h>
#include <stdbool.h>
#include "src/ui/ui.h"
#include "delegate.h"

//----------------------------------------------------------
// Global variables
//----------------------------------------------------------

LGFX gfx;

/* Change to your screen resolution */
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;
static lv_color_t *buf1;
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;       

uint16_t touch_x, touch_y;

WiFiClient    client;
DCCEXProtocol dccexProtocol;
MyDelegate    dccexDelegate;

// Loco Roster List. Saves pointer to Loco Object from DCCEXProtocol
Loco*         locoList[NUM_THROTTLES] ;

// Turnout list. Saves turnout DCC id (i.e address)
int16_t       turnoutList[NUM_TURNOUTS];

// block occupancy sensor list
int16_t       sensorList[NUM_BLOCK_SENSORS];


//--------------------------------------------------------
// wifi config
//--------------------------------------------------------
const char *ssid = "vTrainWifi";        // WiFi SSID name here
const char *password = "RunMyTrains";          // WiFi password here
IPAddress serverAddress(192, 168, 4, 1);    // IP address of your EX-CommandStation
const int serverPort = 2560;                      // Network port of your EX-CommandStation

//----------------------------------------------------------
// display functions
//----------------------------------------------------------

//  Display refresh
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  if (gfx.getStartCount() > 0) {
    gfx.endWrite();
  }
  gfx.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::rgb565_t *)&color_p->full);

  lv_disp_flush_ready(disp);  //	Tell lvgl that the refresh is complete
}

//----------------------------------------------------------
//  Read touch
//----------------------------------------------------------

void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
  data->state = LV_INDEV_STATE_REL;// The state of data existence when releasing the finger
  bool touched = gfx.getTouch( &touch_x, &touch_y );
  if (touched)
  {
    data->state = LV_INDEV_STATE_PR;

    //  Set coordinates
    data->point.x = touch_x;
    data->point.y = touch_y;
    //DEBUG_PRINTF("Screen Touched! X: %d, Y: %d\n", data->point.x, data->point.y);
  }
}

//----------------------------------------------------------
// I2C functions
//----------------------------------------------------------

bool i2cScanForAddress(uint8_t address) {
  Wire.beginTransmission(address);
  return (Wire.endTransmission() == 0);
}

// Wrapper function for sending I2C commands
void sendI2CCommand(uint8_t addr, uint8_t command) {
  uint8_t error;
  // Start sending commands to the specified address
  Wire.beginTransmission(addr);
  // Send command
  Wire.write(command);
  //  End transmission and return status
  error = Wire.endTransmission();

  if (error == 0) {
    DEBUG_PRINTF("command 0x%x sent\n", command);
  } else {
    DEBUG_PRINTF("Command sent error, error code: %d\n", error);
  }
}

//----------------------------------------------------------
// Main Setup 
//----------------------------------------------------------

void setup()
{
  char    dbgStr[80];

  Serial.begin(115200); 

  pinMode(TOUCH_GT911_INT, INPUT_PULLUP);   // touch pad interrupt pin for sleep wakeup

  // turn on the touch panel controller and backlight controller
  Wire.begin(TOUCH_GT911_SDA, TOUCH_GT911_SCL);
  delay(50);
  while (1) {
    if (i2cScanForAddress(I2C_ADDR_BACKLIGHT) && i2cScanForAddress(I2C_ADDR_TOUCHPAD)) {
      Serial.printf("The backlight controller is detected: address 0x%x\n", I2C_ADDR_BACKLIGHT);
      Serial.printf("The touch controller is detected: address 0x%x\n", I2C_ADDR_TOUCHPAD);
      break;
    } else {
      Serial.printf("No backlight controller was detected: address 0x%x\n", I2C_ADDR_BACKLIGHT);
      Serial.printf("No touch controller was detected: address 0x%x\n", I2C_ADDR_TOUCHPAD);
      //Prevent the microcontroller did not start to adjust the bright screen
      sendI2CCommand(I2C_ADDR_BACKLIGHT, 250);    // 250 : Activate touch screen
      pinMode(1, OUTPUT);
      digitalWrite(1, LOW);
      delay(120);
      pinMode(1, INPUT_PULLUP);

      delay(100);
    }
  }

  // Start sending command 0 to address 0x30
  sendI2CCommand(I2C_ADDR_BACKLIGHT, 0);  // 0 is the brightest backlight.    / 245 backlight off   (0-245)

  //-----------------------------
  // Init Display
  //-----------------------------
  gfx.init();
  gfx.initDMA();
  gfx.startWrite();
  gfx.fillScreen(TFT_BLACK);
  gfx.setFont(&fonts::Font4);

  lv_init();
  size_t buffer_size = sizeof(lv_color_t) * LCD_H_RES * LCD_V_RES;
  buf = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
  buf1 = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);

  lv_disp_draw_buf_init(&draw_buf, buf, buf1, LCD_H_RES * LCD_V_RES);

  // Initialize display
  lv_disp_drv_init(&disp_drv);
  // Change the following lines to your display resolution
  disp_drv.hor_res = LCD_H_RES;
  disp_drv.ver_res = LCD_V_RES;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // Initialize input device driver program
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  delay(100);
  gfx.fillScreen(TFT_BLACK);

  //----------------------------------------------------
  // lv_demo_widgets();// Main UI interface
  ui_init();
  DEBUG_PRINTF("Init UI\n");
  lv_timer_handler(); /* let the GUI do its work */

  //----------------------------------------------------
  // Initialize wifi 
  //----------------------------------------------------
  snprintf(dbgStr, sizeof(dbgStr), "Connecting to wifi with SSID: %s....\n", ssid);
  display_dbg_msg(dbgStr);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(10);
    lv_timer_handler();     // call this in order to handle events in ui
  }
  snprintf(dbgStr, sizeof(dbgStr), "Connected with SSID: %s\n", ssid);
  display_dbg_msg(dbgStr);
  DEBUG_PRINTF(dbgStr);

 //----------------------------------------------------
  // Initialize dcc-ex protocol 
  //----------------------------------------------------
  display_dbg_msg("Connecting to DCC-EX server...\n");

  if (!client.connect(serverAddress, serverPort)) {
    DEBUG_PRINTF("connection failed");
    while (1) {
      delay(10);    
      lv_timer_handler();     // call this in order to handle events in ui
    }
  }
  display_dbg_msg("Connected to DCC-EX server\n");

#if _DEBUG_
  dccexProtocol.setLogStream(&Serial);
#endif
  dccexProtocol.enableHeartbeat();

  // Pass the communication to wiThrottleProtocol
  dccexProtocol.connect(&client);
  dccexProtocol.setDelegate(&dccexDelegate);
  dccexProtocol.setDebug(_DEBUG_ ? true : false);
  DEBUG_PRINTF("DCC-EX connected");

   // reset the HAL (i.e. serial interface etc)
  dccexProtocol.sendCommand("D HAL RESET");
  delay(100);

  init_dcc_lists();
 
  // load main ui screen
  lv_scr_load_anim(ui_Home, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, false);

  Serial.println( "Setup done" );
}


//--------------------------------------------------------
// initialize roster and turnout lists
// update the ui widgets for loco names
//--------------------------------------------------------

void init_dcc_lists() 
{
  // Initialize obect id arrays
  for (int lidx=0; lidx < NUM_THROTTLES; ++lidx)      { locoList[lidx] = nullptr; }
  for (int tidx=0; tidx < NUM_TURNOUTS; ++tidx)       { turnoutList[tidx] = -1; }
  for (int sidx=0; sidx < NUM_BLOCK_SENSORS; ++sidx)  { sensorList[sidx] = -1; }


  // wait unti all lists are recieved.
  while (!dccexProtocol.receivedLists()) {
     // request loco roster list and turnout lists
    dccexProtocol.getLists(true, true, false, false);
    dccexProtocol.check();
    delay(10);
  }

  // reset all turnouts to closed position
  for (int tidx=0; tidx < NUM_TURNOUTS; ++tidx) {
    setDccTurnout(tidx, 0);
    delay(100);
  }

  // set initial state of all signal heads
  for (int sidx = 0; sidx < NUM_SIGNAL_HEADS; ++sidx) {
    setDccSignal(sidx, signalHeads[sidx].aspect);
    delay(100);
  }
}

//--------------------------------------------------------
//--------------------------------------------------------

void loop()
{
    // parse incoming messages
  dccexProtocol.check();

  lv_timer_handler(); /* let the GUI do its work */

  if ((millis()/32000) & 0x1) {    // show every 16 second
    show_wifi_status();
  }

  delay(1);
}
