
//////////////////////////////////////////////////////
// Timing related stuff
extern unsigned long getSeconds();	// Seconds since last reset
#define ONESECOND	(1000)

//////////////////////////////////////////////////////
// LCD Display
#include <Adafruit_RGBLCDShield.h>
extern Adafruit_RGBLCDShield g_LCD;

//////////////////////////////////////////////////////
// Screen Controller
#include "ScreenController.h"
extern CScreenController g_screenController;

//////////////////////////////////////////////////////
// Various screen ids
enum ScreenIDsE
{
	screenID_normal = 0,
};

// Debug stuff
#define DEBUG_SETTINGS (1)
//#define DEBUG_SCREEN_CONTROLLER (1)
#define DEBUG_SCREEN_NORMAL	(1)
