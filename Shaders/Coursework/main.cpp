// Main.cpp
#include "../DXFramework/System.h"
#include "AppCoursework.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	App* app = new App();
	System* m_System;

	// Create the system object.
	m_System = new System(app);

	// Initialize and run the system object.
	m_System->Run();

	// Shutdown and release the system object.
	delete m_System;
	m_System = 0;

	return 0;
}