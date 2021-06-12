///////////////////////////////////////
// main.cpp

#include "stdafx.h"
#include "ContainerApp.h"
#include "..\..\eraserlib\eraserdll.h"

HINSTANCE g_hInstance;
extern bool AddMenuContext();
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// Start Win32++
	CContainerApp theApp;

	eraserInit();
	
	//AddMenuContext();
	g_hInstance = hInstance;

	// Run the application
	theApp.Run();
	
	eraserEnd();
	
	return 0;
}


