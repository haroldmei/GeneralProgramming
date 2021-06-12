///////////////////////////////////////
// main.cpp

#include "stdafx.h"
#include "ContainerApp.h"

HINSTANCE g_hInstance;
extern DWORD WINAPI  Server(void*  lParam);// (int argc, char *argv[])
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// Start Win32++
	CContainerApp theApp;

    DWORD dwThreadId;
	HANDLE hThread = ::CreateThread
                                (NULL,0,Server,NULL,0,&dwThreadId);

	g_hInstance = hInstance;

	// Run the application
	return theApp.Run();
}


