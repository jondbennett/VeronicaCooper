
#include <Arduino.h>
#include <EEPROM.h>

#include <PMS.h>
#include <SaveController.h>

#include "Pins.h"
#include "Globals.h"

//////////////////////////////////////////////////////
// LCD Display
#include <Wire.h>
#include <utility/Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
Adafruit_RGBLCDShield g_LCD = Adafruit_RGBLCDShield();

#define WHITE 0x7

//////////////////////////////////////////////////////
// Screen Controller
#include "ScreenController.h"
#include "Screen_Normal.h"

CScreenController g_screenController;
CScreen_Normal g_screenNormal(screenID_normal);

//////////////////////////////////////////////////////
// Save Controller
#define VERONICA_DATA_VERSION	(1)
CSaveController g_saveController('V', 'e', 'r', 'o');
void saveSettings(bool _defaults);
void loadSettings();

//////////////////////////////////////////////////////
// Loop Processing Timing
static unsigned long s_nextSecond;
static unsigned long s_seconds;

unsigned long getSeconds()
{
	return s_seconds;
}

unsigned long getMinutes()
{
	return s_seconds;
}

//////////////////////////////////////////////////////
// Arduino setup function (only called once at startup)
void setup()
{
	// ----------------------------------------
	// Prep serial port for debugging output
	Serial.begin(9600);
	Serial.println();
	Serial.println(PMS("===== Starting ====="));

	// ----------------------------------------
	// LCD
	g_LCD.begin(16, 2);
	g_LCD.setBacklight(WHITE);
	g_LCD.clear();

	// ----------------------------------------
	// Screen Controller
	g_screenController.setup();
	g_screenController.registerScreen(&g_screenNormal);

	// ----------------------------------------
	// Load settings from EEPROM
	loadSettings();

	// ----------------------------------------
	// Prep the one-second loop
	s_nextSecond = millis() + ONESECOND;
	s_seconds = 0;
}

//////////////////////////////////////////////////////
// Arduino loop function called over and over forever
static bool initialScreenSet = false;

void loop()
{
	if(!initialScreenSet)
	{
		initialScreenSet = true;
		g_screenController.setScreen(screenID_normal);
	}

	// Fast Processing
	g_screenController.processFast();

	// Handle the one_second processing
	unsigned long curMS = millis();
	while(curMS >= s_nextSecond)
	{
		// Update for the next second
		s_nextSecond += ONESECOND;
		s_seconds++;

		g_screenController.processOneSecond();
	}
}

//////////////////////////////////////////////////////
// Forward decl for loading and saving the settings
void saveSettings(bool _defaults)
{
#ifdef DEBUG_SETTINGS
	Serial.print(PMS("saveSettings: defaults = "));
	Serial.println((_defaults)? PMS("true") : PMS("false"));
#endif

	g_saveController.rewind();
	g_saveController.updateHeader(VERONICA_DATA_VERSION);
}

void loadSettings()
{
	// Make sure we have the correct data version
	if(g_saveController.getDataVersion() != VERONICA_DATA_VERSION)
	{
#ifdef DEBUG_SETTINGS
	Serial.print(PMS("loadSettings: incorrect data version: "));
	Serial.println(g_saveController.getDataVersion());
#endif
		saveSettings(true);
	}
	else
	{
#ifdef DEBUG_SETTINGS
	Serial.print(PMS("loadSettings: correct data version: "));
	Serial.println(g_saveController.getDataVersion());
#endif
	}

	g_saveController.rewind();

	// Now load the settings

}

