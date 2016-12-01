////////////////////////////////////////////////////////////
// Normal Screen
////////////////////////////////////////////////////////////
#ifndef SCREEN_NORMAL_H
#define SCREEN_NORMAL_H

////////////////////////////////////////////////////////////
// Display normal comms, GPS, and door/light status
////////////////////////////////////////////////////////////
#define CScreen_Normal_SwapDelay	(5)	// seconds
class CScreen_Normal : public CScreen_Base
{
	protected:
	unsigned long m_displaySwapTime;
	bool m_showCommsGPS;

	void updateStatics();
	void updateDynamics();

	public:

	CScreen_Normal(int _id);
	virtual ~CScreen_Normal();

	void init();

	void buttonCheck(CButtonController &_buttons);
	void processOneSecond();
};

#endif
