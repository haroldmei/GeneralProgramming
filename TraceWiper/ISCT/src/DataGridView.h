/****************************************************************************\
*            
*     FILE:     DataGridView.h
*
*     PURPOSE:  ListView based Datagrid with Editable subItems
*
*     COMMENTS:
*               This source is distributed in the hope that it will be useful,
*               but WITHOUT ANY WARRANTY; without even the implied warranty of
*               MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*     Copyright 2007 David MacDermot.
*
* History:
*                Aug '07 - Created
*                July '09 - Added Unicode support
*                           Improved behavior of edit box during scrolling
*                           Improved redraw of grid lines
*                           Changed Get(Set)WindowLong to Get(Set)WindowLongPtr for 64bit compatability
*
\****************************************************************************/

/****************************************************************************/
// Messages

#define NELEMS(a)  (sizeof(a) / sizeof((a)[0]))


#define DGVM_GETLISTVIEWCONTROL WM_USER + 0x01
#define DGVM_GETEDITCONTROL WM_USER + 0x02
#define DGVM_GETCOLUMNCOUNT WM_USER + 0x03
#define DGVM_GETEDITORBACKCLR WM_USER + 0x04
#define DGVM_SETEDITORBACKCLR WM_USER + 0x05
#define DGVM_GETEDITORTEXTCLR WM_USER + 0x06
#define DGVM_SETEDITORTEXTCLR WM_USER + 0x07
#define DGVM_RESIZEABLEHEADER WM_USER + 0x08
#define DGVM_GETTXTCLR WM_USER + 0x09
#define DGVM_SETTXTCLR WM_USER + 0x0A
#define DGVM_GETALTBACKCLR WM_USER + 0x0B
#define DGVM_SETALTBACKCLR WM_USER + 0x0C
#define DGVM_GETALTTXTCLR WM_USER + 0x0D
#define DGVM_SETALTTXTCLR WM_USER + 0x0E
#define DGVM_ROWHEADERS WM_USER + 0x0F
#define DGVM_SETROWHEIGHT WM_USER + 0x10
#define DGVM_EXTENDLASTCOLUMN WM_USER + 0x11

/****************************************************************************/
// Macroes

#define DataGridView_AddColumn(hGrid,nCol,iWidth,szColText){ \
	LVCOLUMN lv_c; lv_c.mask=LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM; \
	lv_c.cx=iWidth; lv_c.pszText=(szColText); \
	ListView_InsertColumn(hGrid,nCol,&lv_c);}

#define DataGridView_AddColumns(hGrid,aryColTxt,iColCount) \
	for(int Col = 0; Col<iColCount;Col++) \
		DataGridView_AddColumn((hGrid),Col,lstrlen(aryColTxt[Col])*16,aryColTxt[Col]);

#define DataGridView_AddRow(hGrid,aryItemTxt,iColCount) \
	for(int Col = 0; Col<iColCount;){\
		LV_ITEM Lv_i; \
		Lv_i.mask=LVIF_TEXT; \
		if(0==Col) { \
			Lv_i.iItem=ListView_GetItemCount(hGrid); \
			Lv_i.iSubItem=Col; Lv_i.pszText=aryItemTxt[Col++]; \
	 		ListView_InsertItem(hGrid,&Lv_i);} \
		Lv_i.iSubItem=Col; \
		Lv_i.pszText=aryItemTxt[Col++]; \
		ListView_SetItem(hGrid,&Lv_i);}

#define DataGridView_GetEditorBkColor(hwnd)  (COLORREF)SNDMSG((hwnd),DGVM_GETEDITORBACKCLR,0,0L)
#define DataGridView_SetEditorBkColor(hwnd,clrBk)  (BOOL)SNDMSG((hwnd),DGVM_SETEDITORBACKCLR,0,(LPARAM)(COLORREF)(clrBk))
#define DataGridView_GetEditorTxtColor(hwnd)  (COLORREF)SNDMSG((hwnd),DGVM_GETEDITORTEXTCLR,0,0L)
#define DataGridView_SetEditorTxtColor(hwnd,clrTxt)  (BOOL)SNDMSG((hwnd),DGVM_SETEDITORTEXTCLR,0,(LPARAM)(COLORREF)(clrTxt))
#define DataGridView_GetListViewControl(hwnd)  (HWND)SNDMSG((hwnd),DGVM_GETLISTVIEWCONTROL,0,0L)
#define DataGridView_GetEditControl(hwnd)  (HWND)SNDMSG((hwnd),DGVM_GETEDITCONTROL,0,0L)
#define DataGridView_GetColumnCount(hwnd)  (INT)SNDMSG((hwnd),DGVM_GETCOLUMNCOUNT,0,0L)
#define DataGridView_GetRowCount(hwnd)  ListView_GetItemCount((hwnd))
#define DataGridView_SetResizableHeader(hwnd,fResizable)  (BOOL)SNDMSG((hwnd),DGVM_RESIZEABLEHEADER,(WPARAM)(fResizable),(LPARAM)0L)
#define DataGridView_GetBkColor(hwnd)  ListView_GetBkColor(hwnd)
#define DataGridView_SetBkColor(hwnd,clrBk)  ListView_SetBkColor(hwnd,clrBk)
#define DataGridView_GetTextColor(hwnd)  ListView_GetTextColor(hwnd)
#define DataGridView_SetTextColor(hwnd,clrTx)  ListView_SetTextColor(hwnd,clrTx)
#define DataGridView_GetAltBkColor(hwnd)  (COLORREF)SNDMSG((hwnd),DGVM_GETALTBACKCLR,0,0L)
#define DataGridView_SetAltBkColor(hwnd,clrBk,fPaintByRow)  (BOOL)SNDMSG((hwnd),DGVM_SETALTBACKCLR,(WPARAM)(fPaintByRow),(LPARAM)(COLORREF)(clrBk))
#define DataGridView_GetAltTextColor(hwnd)  (COLORREF)SNDMSG((hwnd),DGVM_GETALTTXTCLR,0,0L)
#define DataGridView_SetAltTextColor(hwnd,clrTx)  (BOOL)SNDMSG((hwnd),DGVM_SETALTTXTCLR,0,(LPARAM)(COLORREF)(clrTx))
#define DataGridView_DisplayRowHeaders(hwnd,fShow)  (BOOL)SNDMSG((hwnd),DGVM_ROWHEADERS,(WPARAM)(fShow),0L)
#define DataGridView_SetRowHeight(hwnd,iHeight)  (BOOL)SNDMSG((hwnd),DGVM_SETROWHEIGHT,(WPARAM)(iHeight),0L)
#define DataGridView_ExtendLastColumn(hwnd,fExtend)  (BOOL)SNDMSG((hwnd),DGVM_EXTENDLASTCOLUMN,(WPARAM)(fExtend),0L)

/****************************************************************************/
// Exported function prototypes

BOOL InitDataGridView(HINSTANCE hInstance);
int AddItem(HWND hList1,LPCTSTR szText, int nImage);
void SetSubItem(HWND hList1, int nItem, int nSubItem, LPCTSTR szText);
LPWSTR string2WCHAR(const char* srcStr);
void TC2C(const PTCHAR tc, char* pBuf) ;
void DisplaySetting(HWND hwnd);
void PreviousResult(HWND hwnd);
BOOL Grid_Validate(TCHAR* lBuf);
BOOL DClick_Validate(HWND hwnd);
void* Get_DNode(HWND hwnd, int iItem);
