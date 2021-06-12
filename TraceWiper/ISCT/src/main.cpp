///////////////////////////////////////
// main.cpp

#include "stdafx.h"
#include "ContainerApp.h"
//#include "..\..\eraserlib\eraserdll.h"

HINSTANCE g_hInstance;

//extern int Client (int argc, char *argv[]);
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    (void)hPrevInstance;
    (void)nShowCmd;
    (void)lpCmdLine;
    // Start Win32++
    CContainerApp theApp;

//    eraserInit();
    //Client(0,0);
    g_hInstance = hInstance;

    // Run the application
    theApp.Run();
    
//    eraserEnd();
    
    return 0;
}


