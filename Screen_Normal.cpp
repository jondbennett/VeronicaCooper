////////////////////////////////////////////////////////////
// Normal Screen
////////////////////////////////////////////////////////////
#include <Arduino.h>

#include <PMS.h>

#include "Pins.h"
#include "Globals.h"

#include "ScreenController.h"
#include "Screen_Normal.h"

////////////////////////////////////////////////////////////
// Display normal temperature situation
////////////////////////////////////////////////////////////
CScreen_Normal::CScreen_Normal(int _id) : CScreen_Base(_id)
{
	m_displaySwapTime = getSeconds() + CScreen_Normal_SwapDelay;
	m_showCommsGPS = false;
}

CScreen_Normal::~CScreen_Normal()
{

}

void CScreen_Normal::init()
{
#ifdef DEBUG_SCREEN_NORMAL
	Serial.println(PMS("CScreen_Normal::init()"));
#endif

	m_displaySwapTime = getSeconds() + CScreen_Normal_SwapDelay;
	m_showCommsGPS = false;
	updateStatics();
}

void CScreen_Normal::updateStatics()
{
#ifdef DEBUG_SCREEN_NORMAL
	Serial.println(PMS("CScreen_Normal::updateStatics()"));
#endif

	if(m_showCommsGPS)
	{
		g_LCD.clear();
		g_LCD.noCursor();

		g_LCD.setCursor(0, 0);
		g_LCD.print(PMS("Comms: "));

		g_LCD.setCursor(0, 1);
		g_LCD.print(PMS("  GPS: "));
	}
	else
	{
		g_LCD.clear();
		g_LCD.noCursor();

		g_LCD.setCursor(0, 0);
		g_LCD.print(PMS(" Door: "));

		g_LCD.setCursor(0, 1);
		g_LCD.print(PMS("Light: "));
	}
}

void CScreen_Normal::updateDynamics()
{
#ifdef DEBUG_SCREEN_NORMAL
	Serial.println(PMS("CScreen_Normal::updateDynamics()"));
#endif

	if(getSeconds() >= m_displaySwapTime)
	{
		m_displaySwapTime = getSeconds() + CScreen_Normal_SwapDelay;
		m_showCommsGPS = !m_showCommsGPS;

		updateStatics();
	}

	if(m_showCommsGPS)
	{
	}
	else
	{
	}
}

void CScreen_Normal::buttonCheck(CButtonController &_buttons)
{
}

void CScreen_Normal::processOneSecond()
{
	updateDynamics();
}
