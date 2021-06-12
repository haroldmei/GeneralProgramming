; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=COptionsForFiles
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "eraser.h"
LastPage=0

ClassCount=10
Class1=CByteEdit
Class2=CCustomMethodEdit
Class3=CEraserApp
Class4=CErrorDialog
Class5=COptionsForFiles
Class6=COptionsForFreeSpace
Class7=COptionsDlg
Class8=CResultsDialog

ResourceCount=7
Resource1=IDD_DIALOG_PASSEDIT (English (U.S.))
Resource2=IDD_DIALOG_RESULT_NEW (English (U.S.))
Resource3=IDD_DIALOG_METHODEDIT (English (U.S.))
Resource4=IDD_PAGE_FREESPACE (English (U.S.))
Resource5=IDD_DIALOG_RESULT_FAILURE (English (U.S.))
Class9=CPassEditDlg
Resource6=IDD_PAGE_FILES (English (U.S.))
Class10=CReportDialog
Resource7=IDD_DIALOG_REPORT (English (U.S.))

[CLS:CByteEdit]
Type=0
BaseClass=CEdit
HeaderFile=ByteEdit.h
ImplementationFile=ByteEdit.cpp
LastObject=CByteEdit
Filter=W
VirtualFilter=WC

[CLS:CCustomMethodEdit]
Type=0
BaseClass=CDialog
HeaderFile=CustomMethodEdit.h
ImplementationFile=CustomMethodEdit.cpp
LastObject=CCustomMethodEdit
Filter=D
VirtualFilter=dWC

[CLS:CEraserApp]
Type=0
BaseClass=CWinApp
HeaderFile=Eraser.h
ImplementationFile=Eraser.cpp
Filter=N
VirtualFilter=AC
LastObject=CEraserApp

[CLS:CErrorDialog]
Type=0
BaseClass=CDialog
HeaderFile=ErrorDialog.h
ImplementationFile=ErrorDialog.cpp

[CLS:COptionsForFiles]
Type=0
BaseClass=CPropertyPage
HeaderFile=OptionPages.h
ImplementationFile=OptionPages.cpp
Filter=D
VirtualFilter=idWC
LastObject=COptionsForFiles

[CLS:COptionsForFreeSpace]
Type=0
BaseClass=CPropertyPage
HeaderFile=OptionPages.h
ImplementationFile=OptionPages.cpp
LastObject=COptionsForFreeSpace
Filter=D
VirtualFilter=idWC

[CLS:COptionsDlg]
Type=0
BaseClass=CPropertySheet
HeaderFile=OptionsDlg.h
ImplementationFile=OptionsDlg.cpp

[CLS:CResultsDialog]
Type=0
BaseClass=CDialog
HeaderFile=ResultsDialog.h
ImplementationFile=ResultsDialog.cpp

[DLG:IDD_DIALOG_METHODEDIT]
Type=1
Class=CCustomMethodEdit

[DLG:IDD_PAGE_FILES]
Type=1
Class=COptionsForFiles

[DLG:IDD_PAGE_FREESPACE]
Type=1
Class=COptionsForFreeSpace

[DLG:IDD_DIALOG_RESULT_NEW (English (U.S.))]
Type=1
Class=?
ControlCount=17
Control1=IDOK,button,1342242817
Control2=IDC_STATIC_STATUS_MESSAGE,static,1342308352
Control3=IDC_STATIC_STATUS_ICON,static,1342177283
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC_WIPEDTOTAL,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC_TIMETOTAL,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC_SPEED,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC_TOTALSIZE,static,1342308352
Control14=IDC_STATIC_TOTALTIPS,static,1342308352
Control15=IDC_STATIC_STATUS_DATA,static,1342308352
Control16=IDC_STATIC_STATUS_DATA_ICON,static,1342177283
Control17=IDC_STATIC,static,1342308352

[DLG:IDD_DIALOG_RESULT_FAILURE (English (U.S.))]
Type=1
Class=?
ControlCount=17
Control1=IDOK,button,1342242817
Control2=IDC_STATIC_STATUS_MESSAGE,static,1342308352
Control3=IDC_STATIC_STATUS_ICON,static,1342177283
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC_WIPEDTOTAL,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC_TIMETOTAL,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC_SPEED,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC_TOTALSIZE,static,1342308352
Control14=IDC_STATIC_TOTALTIPS,static,1342308352
Control15=IDC_STATIC_STATUS_DATA,static,1342308352
Control16=IDC_EDIT_ERRORS,edit,1353779204
Control17=IDC_STATIC_STATUS_DATA_ICON,static,1342177283

[DLG:IDD_PAGE_FILES (English (U.S.))]
Type=1
Class=COptionsForFiles
ControlCount=10
Control1=IDC_LIST_METHOD,SysListView32,1342275597
Control2=IDC_BUTTON_NEW,button,1342242816
Control3=IDC_BUTTON_EDIT,button,1342242816
Control4=IDC_BUTTON_DELETE,button,1342242816
Control5=IDC_STATIC_SELECTED,static,1342308480
Control6=IDC_STATIC,static,1342308352
Control7=IDC_CHECK_FILECLUSTERTIPS,button,1342242819
Control8=IDC_STATIC,static,1342308352
Control9=IDC_CHECK_FILENAMES,button,1342242819
Control10=IDC_CHECK_ALTERNATESTREAMS,button,1342242819

[DLG:IDD_PAGE_FREESPACE (English (U.S.))]
Type=1
Class=?
ControlCount=10
Control1=IDC_LIST_METHOD,SysListView32,1342275597
Control2=IDC_BUTTON_NEW,button,1342242816
Control3=IDC_BUTTON_EDIT,button,1342242816
Control4=IDC_BUTTON_DELETE,button,1342242816
Control5=IDC_CHECK_FREESPACE,button,1342242819
Control6=IDC_CHECK_CLUSTERTIPS,button,1342242819
Control7=IDC_CHECK_DIRECTORYENTRIES,button,1342242819
Control8=IDC_STATIC_SELECTED,static,1342308480
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352

[DLG:IDD_DIALOG_METHODEDIT (English (U.S.))]
Type=1
Class=?
ControlCount=21
Control1=IDC_EDIT_DESCRIPTION,edit,1350631552
Control2=IDC_LIST_PASSES,SysListView32,1342275597
Control3=IDC_BUTTON_ADD,button,1342242816
Control4=IDC_BUTTON_DELETE,button,1342242816
Control5=IDC_BUTTON_COPY,button,1342242816
Control6=IDC_BUTTON_UP,button,1342242816
Control7=IDC_BUTTON_DOWN,button,1342242816
Control8=IDC_EDIT_BYTE1,edit,1350631552
Control9=IDC_CHECK_BYTE2,button,1342242819
Control10=IDC_EDIT_BYTE2,edit,1350631552
Control11=IDC_CHECK_BYTE3,button,1342242819
Control12=IDC_EDIT_BYTE3,edit,1350631552
Control13=IDC_RADIO_PSEUDORANDOM,button,1342177289
Control14=IDC_CHECK_SHUFFLE,button,1342242819
Control15=IDOK,button,1342242817
Control16=IDCANCEL,button,1342242816
Control17=IDC_STATIC,static,1342308352
Control18=IDC_STATIC,static,1342308352
Control19=IDC_RADIO_PATTERN,button,1342177289
Control20=IDC_STATIC,static,1342177287
Control21=IDC_STATIC_BYTE1,static,1342308352

[DLG:IDD_DIALOG_PASSEDIT (English (U.S.))]
Type=1
Class=CPassEditDlg
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDIT_PASSES,edit,1350639744
Control5=IDC_SPIN_PASSES,msctls_updown32,1342177462
Control6=IDC_STATIC,static,1342308352

[CLS:CPassEditDlg]
Type=0
HeaderFile=PassEditDlg.h
ImplementationFile=PassEditDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CPassEditDlg

[DLG:IDD_DIALOG_REPORT (English (U.S.))]
Type=1
Class=CReportDialog
ControlCount=8
Control1=IDCANCEL,button,1342242817
Control2=IDC_BUTTON_SAVEAS,button,1342242816
Control3=IDC_EDIT_STATISTICS,edit,1353779204
Control4=IDC_LIST_ERRORS,SysListView32,1350631429
Control5=IDC_STATIC,static,1342177296
Control6=IDC_STATIC_COMPLETION,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC_FAILURES_HEADER,static,1342308352

[CLS:CReportDialog]
Type=0
HeaderFile=ReportDialog.h
ImplementationFile=ReportDialog.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CReportDialog

