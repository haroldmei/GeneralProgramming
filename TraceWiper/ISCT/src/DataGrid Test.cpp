// DataGrid Test.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "DataGrid.h"
#include <stdio.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
TCHAR szTitle[MAX_LOADSTRING];                                // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];                                // The title bar text

// Foward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                DG_InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

/* Custom sort function */
int CALLBACK CompareItems(char* item1, char* item2, int column);

CDataGrid dataGrid, dataGrid1;
bool sort = true;
char buffer[1024];
int g_Message = 0;
HWND buttonRemove;
HWND buttonRemoveAll;
HWND buttonShowText;
HWND buttonAdd;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
     // TODO: Place code here.
    MSG msg;
    HACCEL hAccelTable;

    // Initialize global strings

    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!DG_InitInstance (hInstance, nCmdShow)) 
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)"test");

    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX); 

    wcex.style            = CS_DBLCLKS;
    wcex.lpfnWndProc    = (WNDPROC)WndProc;
    wcex.cbClsExtra        = 0;
    wcex.cbWndExtra        = 0;
    wcex.hInstance        = hInstance;
    wcex.hIcon            = LoadIcon(hInstance, (LPCTSTR)"aaa");
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground    = (HBRUSH)(COLOR_WINDOW);
    wcex.lpszMenuName    = (LPCSTR)"bbb";
    wcex.lpszClassName    = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, (LPCTSTR)"small");

    return RegisterClassEx(&wcex);
}

//
//   FUNCTION: DG_InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL DG_InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND    - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY    - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    TCHAR szHello[MAX_LOADSTRING];

    switch (message) 
    {
        case WM_COMMAND:
        {
            if ( (HWND)lParam == buttonRemove )
            {
                // Remove selected item
                dataGrid.RemoveItem(dataGrid.GetSelectedRow());
                dataGrid.Update();
            }
            else if ( (HWND)lParam == buttonRemoveAll )
            {
                // Remove all items
                dataGrid.RemoveAllItems();
            }
            else if ( (HWND)lParam == buttonShowText )
            {
                // Show item text
                MessageBox( hWnd, buffer, "Info...", MB_OK );
                strcpy( buffer, "" );
            }
            else if ( (HWND)lParam == buttonAdd )
            {
                // Add new item
                char item[100];
                int row = dataGrid.GetRowNumber();
                sprintf( item, "Item%d", row );
                dataGrid.InsertItem( item, DGTA_CENTER );
                dataGrid.SetItemInfo( row, 1, "Subitem1", DGTA_RIGHT, false );
                dataGrid.SetItemInfo( row, 2, "Subitem2", DGTA_LEFT, false );
                dataGrid.SetItemInfo( row, 3, "Subitem3", DGTA_CENTER, false );
                dataGrid.SetItemInfo( row, 4, "Subitem4", DGTA_CENTER, false );
                dataGrid.Update();
            }
            else if ( (HWND)lParam == dataGrid.GetWindowHandle() )
            {
                switch (HIWORD(wParam))
                {
                    case DGM_ITEMCHANGED:
                    {
                        // Item selected
                        char msg[100];
                        sprintf( buffer, "Item changed: (%d,%d)", dataGrid.GetSelectedRow(), dataGrid.GetSelectedColumn() );
                        sprintf( msg, "%d", g_Message+1 );
                        dataGrid1.InsertItem( msg, DGTA_LEFT );
                        dataGrid1.SetItemInfo( g_Message, 1, buffer, DGTA_LEFT, true );
                        dataGrid1.Update();
                        g_Message++;
                    }
                    break;

                    case DGM_ITEMTEXTCHANGED:
                    {
                        // Item text changed
                        char msg[100];
                        sprintf( msg, "%d", g_Message+1 );
                        sprintf( buffer, "Item (%d,%d) text changed", dataGrid.GetSelectedRow(), dataGrid.GetSelectedColumn() );
                        dataGrid1.InsertItem( msg, DGTA_LEFT );
                        dataGrid1.SetItemInfo( g_Message, 1, buffer, DGTA_LEFT, true );
                        dataGrid1.Update();
                        g_Message++;
                    }
                    break;

                    case DGM_ITEMADDED:
                    {
                        if ( dataGrid1.GetWindowHandle() )
                        {
                        // Item added
                        char msg[100];
                        sprintf( msg, "%d", g_Message+1 );
                        sprintf( buffer, "Item added" );
                        dataGrid1.InsertItem( msg, DGTA_LEFT );
                        dataGrid1.SetItemInfo( g_Message, 1, buffer, DGTA_LEFT, true );
                        dataGrid1.Update();
                        g_Message++;
                        }
                    }
                    break;

                    case DGM_ITEMREMOVED:
                    {
                        // Item added
                        char msg[100];
                        sprintf( msg, "%d", g_Message+1 );
                        sprintf( buffer, "Item removed" );
                        dataGrid1.InsertItem( msg, DGTA_LEFT );
                        dataGrid1.SetItemInfo( g_Message, 1, buffer, DGTA_LEFT, true );
                        dataGrid1.Update();
                        g_Message++;
                    }
                    break;

                    case DGM_STARTSORTING:
                    {
                        // Sorting started
                        char msg[100];
                        sprintf( msg, "%d", g_Message+1 );
                        int col = dataGrid.GetResizedColumn();
                        sprintf( buffer, "Sorting started by column %d", col );
                        dataGrid1.InsertItem( msg, DGTA_LEFT );
                        dataGrid1.SetItemInfo( g_Message, 1, buffer, DGTA_LEFT, true );
                        dataGrid1.Update();
                        g_Message++;
                    }
                    break;

                    case DGM_ENDSORTING:
                    {
                        // Sorting started
                        char msg[100];
                        sprintf( msg, "%d", g_Message+1 );
                        int col = dataGrid.GetResizedColumn();
                        sprintf( buffer, "Sorting ended by column %d", col );
                        dataGrid1.InsertItem( msg, DGTA_LEFT );
                        dataGrid1.SetItemInfo( g_Message, 1, buffer, DGTA_LEFT, true );
                        dataGrid1.Update();
                        g_Message++;

                        sort = !sort;
                    }

                    case DGM_COLUMNRESIZED:
                    {
                        // Column resized
                        char msg[100];
                        sprintf( msg, "%d", g_Message+1 );
                        int col = dataGrid.GetResizedColumn();
                        sprintf( buffer, "Column %d resized...", col );
                        dataGrid1.InsertItem( msg, DGTA_LEFT );
                        dataGrid1.SetItemInfo( g_Message, 1, buffer, DGTA_LEFT, true );
                        dataGrid1.Update();
                        g_Message++;
                    }
                    break;

                    case DGM_COLUMNCLICKED:
                    {
                        // Column clicked
                        char msg[100];
                        sprintf( msg, "%d", g_Message+1 );
                        int col = dataGrid.GetResizedColumn();
                        sprintf( buffer, "Column %d clicked...", col );
                        dataGrid1.InsertItem( msg, DGTA_LEFT );
                        dataGrid1.SetItemInfo( g_Message, 1, buffer, DGTA_LEFT, true );
                        dataGrid1.Update();
                        g_Message++;
                    }
                    break;
                }
                break;
            }
            else if ( (HWND)lParam == dataGrid1.GetWindowHandle() )
            {
                switch (HIWORD(wParam))
                {
                    case DGM_ITEMADDED:
                    {
                        if ( dataGrid1.GetWindowHandle() )
                        {
                            DG_ITEMINFO dgItemInfo;
                            dgItemInfo.dgMask = DG_TEXTHIGHLIGHT;
                            dgItemInfo.dgItem = dataGrid1.GetRowNumber() - 1;
                            dgItemInfo.dgSubitem = 0;
                            dgItemInfo.dgSelected = true;
                            dataGrid1.SetItemInfo(&dgItemInfo);
                            dataGrid1.Update();
                            dataGrid1.EnsureVisible( dataGrid1.GetRowNumber(), 0 );
                            dataGrid1.Update();
                        }
                    }
                    break;
                }
            }

            wmId    = LOWORD(wParam); 
            wmEvent = HIWORD(wParam); 
            // Parse the menu selections:
            switch (wmId)
            {

                default:
                   return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_CREATE:
            {
                // Create DataGrid child window
                RECT rect = {50,50,700,300};
                dataGrid.Create( rect, hWnd, 5 );
                // Set DataGrid column info
                dataGrid.SetColumnInfo( 0, "Column0", 120, DGTA_CENTER );
                dataGrid.SetColumnInfo( 1, "Column1", 120, DGTA_CENTER );
                dataGrid.SetColumnInfo( 2, "Column2", 120, DGTA_CENTER );
                dataGrid.SetColumnInfo( 3, "Column3", 120, DGTA_CENTER );
                dataGrid.SetColumnInfo( 4, "Column4", 120, DGTA_CENTER );
                // Insert DataGrid rows
                char item[10];
                for ( int row=0; row<1000; row++ )
                {
                    sprintf( item, "Item%d", row );
                    dataGrid.InsertItem( item, DGTA_CENTER );
                    dataGrid.SetItemInfo( row, 1, "Subitem1", DGTA_RIGHT, false );
                    dataGrid.SetItemInfo( row, 2, "Subitem2", DGTA_LEFT, false );
                    dataGrid.SetItemInfo( row, 3, "Subitem3", DGTA_CENTER, false );
                    dataGrid.SetItemInfo( row, 4, "Subitem4", DGTA_CENTER, false );
                }
                // Set DataGrid attributes
                dataGrid.SetItemBgColor( 10, RGB(250,220,220) );
                dataGrid.SetItemBgColor( 12, RGB(220,250,220) );
                dataGrid.SetItemBgColor( 15, RGB(250,250,220) );
                dataGrid.SetCompareFunction((DGCOMPARE)CompareItems);
                DG_ITEMINFO dgItemInfo;
                dgItemInfo.dgMask = DG_TEXTRONLY;
                dgItemInfo.dgItem = 0;
                dgItemInfo.dgSubitem = 0;
                dgItemInfo.dgReadOnly = true;
                dataGrid.SetItemInfo(&dgItemInfo);
                dgItemInfo.dgItem = 1;
                dgItemInfo.dgSubitem = 1;
                dataGrid.SetItemInfo(&dgItemInfo);
                LOGFONT lf;
                dataGrid.GetColumnFont(&lf);
                lf.lfWeight = FW_BOLD;
                dataGrid.SetColumnFont(&lf);
                dataGrid.Update();

                // Create DataGrid child window
                RECT rect1 = {50,400,500,550};
                dataGrid1.Create( rect1, hWnd, 2 );
                // Set DataGrid column info
                dataGrid1.SetColumnInfo( 0, "No.", 50, DGTA_CENTER );
                dataGrid1.SetColumnInfo( 1, "Message posted", 300, DGTA_LEFT );
                // Set DataGrid attributes
                dataGrid1.EnableSort(FALSE);
                dataGrid1.EnableEdit(FALSE);
                dataGrid1.EnableResize(FALSE);
                dataGrid1.EnableGrid(FALSE);
                dataGrid1.SetColumnTxtColor(RGB(0,0,100));
                dataGrid1.SetRowTxtColor(RGB(100,0,0));
                dataGrid1.GetRowFont(&lf);
                lf.lfItalic = TRUE;
                dataGrid1.SetRowFont(&lf);
                dataGrid1.Update();

                // Create buttons
                buttonRemove = CreateWindowEx( 0, "BUTTON", "Remove", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                    100, 450, 100, 20, hWnd, NULL, GetModuleHandle(NULL), NULL );
                buttonRemoveAll = CreateWindowEx( 0, "BUTTON", "Remove all", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                    250, 450, 100, 20, hWnd, NULL, GetModuleHandle(NULL), NULL );
                buttonShowText = CreateWindowEx( 0, "BUTTON", "Show info", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                    400, 450, 100, 20, hWnd, NULL, GetModuleHandle(NULL), NULL );
                buttonAdd = CreateWindowEx( 0, "BUTTON", "Add", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                    550, 450, 100, 20, hWnd, NULL, GetModuleHandle(NULL), NULL );
            }
            break;
            case WM_SIZE:
            {
                // Resize DataGrid window
                dataGrid.Resize();
                dataGrid1.Resize();
            }
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
                return TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
            {
                EndDialog(hDlg, LOWORD(wParam));
                return TRUE;
            }
            break;
    }
    return FALSE;
}


int CALLBACK CompareItems(char* item1, char* item2, int column)
{
    // Return comparison result
    if ( sort )
         return strcmp( item1, item2 );
    else
         return strcmp( item2, item1 );
}