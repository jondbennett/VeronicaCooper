////////////////////////////////////////////////////////////
// Screen Controller
////////////////////////////////////////////////////////////
#include <Arduino.h>

#include <Adafruit_RGBLCDShield.h>

#include <PMS.h>

#include "Pins.h"
#include "Globals.h"

#include "ScreenController.h"

////////////////////////////////////////////////////////////
// This object simply dispatches it's process calls
// to the appropriate screen object
////////////////////////////////////////////////////////////
#define CScreenController_Invalid_ScreenID (-1)

CScreenController::CScreenController()
{
}

CScreenController::~CScreenController()
{
}

void CScreenController::setup()
{
#ifdef DEBUG_SCREEN_CONTROLLER
	Serial.println(PMS("CScreenController::setup()"));
#endif

	m_screenID = CScreenController_Invalid_ScreenID;
	for(int _ = 0; _ < CScreenController_MAX_SCREENS; _++)
		m_screenList[_] = 0;

	m_buttonController.setup();
}

void CScreenController::registerScreen(CScreen_Base *_screen)
{
#ifdef DEBUG_SCREEN_CONTROLLER
	Serial.print(PMS("CScreenController::registerScreen: "));
	Serial.println(_screen->getID());
#endif

	for(int _ = 0; _ < CScreenController_MAX_SCREENS; _++)
		if(m_screenList[_] == 0)
			m_screenList[_] = _screen;
}

CScreen_Base *CScreenController::findScreen()
{
	if(m_screenID < 0)
		return 0;

	for(int _ = 0; _ < CScreenController_MAX_SCREENS; _++)
		if(m_screenList[_] && m_screenList[_]->getID() == m_screenID)
			return m_screenList[_];

	return 0;
}

void CScreenController::setScreen(int _screen)
{
	// Don't keep resetting the screen
	if(m_screenID == _screen)
		return;
	m_screenID = _screen;

#ifdef DEBUG_SCREEN_CONTROLLER
	Serial.print(PMS("CScreenController::setScreen: "));
	Serial.println(_screen);
#endif

	// And do an immediate update
	if(findScreen())
		findScreen()->init();

	m_buttonController.maskButtonsUntilClear();
}

void CScreenController::processFast()
{
	m_buttonController.processFast();

	CScreen_Base *screen = findScreen();
	if(screen)
		screen->buttonCheck(m_buttonController);
}

void CScreenController::processOneSecond()
{
	CScreen_Base *screen = findScreen();
	if(screen)
	{
#ifdef DEBUG_SCREEN_CONTROLLER
	Serial.print(PMS("CScreenController::processOneSecond: "));
	Serial.println(screen->getID());
#endif
		screen->processOneSecond();
	}
}

////////////////////////////////////////////////////////////
// Button Controller
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Read the buttons from the keyboard and monitor how
// long each button has been pressed.
//
// NOTE: This object uses the buttons build into the
// LCD Shield
////////////////////////////////////////////////////////////
static int RawReadButtons()
{
	uint8_t buttons = g_LCD.readButtons();

	if(buttons & BUTTON_UP) return BC_BUTTON_UP;
	if(buttons & BUTTON_DOWN) return BC_BUTTON_DOWN;
	if(buttons & BUTTON_LEFT) return BC_BUTTON_LEFT;
	if(buttons & BUTTON_RIGHT) return BC_BUTTON_RIGHT;
	if(buttons & BUTTON_SELECT) return BC_BUTTON_SELECT;

	return BC_BUTTON_NONE;
}

CButtonController::CButtonController()
{
	// Clear all of the on times
	for(int iButtonIndex = 0; iButtonIndex < BC_NBUTTONS; ++iButtonIndex)
		m_buttons[iButtonIndex] = 0;

	// Clear the readings
	m_buttonRead1 = BC_BUTTON_NONE;
	m_buttonRead2 = BC_BUTTON_NONE;

	// Set our state to the start
	m_state = Button_State_1;

	m_maskButtonsUntilClear = false;
}

CButtonController::~CButtonController()
{
}

void CButtonController::setup()
{
}

void CButtonController::processFast()
{
unsigned long uMillies = millis();
int rawButtons = BC_BUTTON_NONE;

	// Read the raw button values in such a way
	// that the buttons on the shield, and the digital
	// input buttons can both work
	rawButtons = RawReadButtons();

	// Sanity
	if((rawButtons < 0) || (rawButtons >= BC_NBUTTONS))
		rawButtons = BC_BUTTON_NONE;

	// is this the first read of this cycle?
	if(m_state == Button_State_1)
	{
		m_buttonRead1 = rawButtons;
		m_buttonRead2 = 0;

		m_debounceTime = uMillies + BC_DEBOUNCE_DELAY;
		m_state = Button_State_2;
		return;
	}

	// Second read of the cycle
	if(m_state == Button_State_2)
	{
		// Have we waited long enough?
		if(uMillies < m_debounceTime)
			return;

		// Read them again
		m_buttonRead2 = rawButtons;
		m_state = Button_State_1;
	}

	if((m_buttonRead1 > BC_BUTTON_NONE) &&
		(m_buttonRead2 > BC_BUTTON_NONE) &&
		(m_buttonRead1 == m_buttonRead2))
	{
		// A button is pressed, so sets it "on time". DO NOT reset it
		if(m_buttons[m_buttonRead2] == 0)
			m_buttons[m_buttonRead2] = uMillies;
	}
	else
	{
		// Nothing pressed so clear all of the on times
		for(int iButtonIndex = 0; iButtonIndex < BC_NBUTTONS; ++iButtonIndex)
			m_buttons[iButtonIndex] = 0;

		if(m_maskButtonsUntilClear)
		{
//			Serial.PMSPRINTLN("Unmasking Buttons");
			m_maskButtonsUntilClear = false;
		}
	}
}

// Return ms since button pressed - 0 = not pressed
unsigned long CButtonController::getButton(int _iButtonID)
{

	if(m_maskButtonsUntilClear)
		return 0;

	if((_iButtonID >= 0) && (_iButtonID < BC_NBUTTONS))
	{
		if(m_buttons[_iButtonID] > 0)
			return millis() - m_buttons[_iButtonID];
		else
			return 0;
	}

	return 0;
}

void CButtonController::maskButtonsUntilClear()
{
	m_maskButtonsUntilClear = true;
//	Serial.PMSPRINTLN("Masking buttons until clear");
}
