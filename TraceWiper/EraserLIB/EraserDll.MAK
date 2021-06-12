ORIGIN		= DigiMars C++
ORIGIN_VER	= Version 7.22
VERSION		= RELEASE

!IFDEF SUB_DEBUG
DEBUG		= $(SUB_DEBUG)
NDEBUG		= !$(SUB_DEBUG)
!ELSE
DEBUG		= 0
NDEBUG		= 1
!ENDIF

PROJ		= EraserDll
APPTYPE		= WIN32 DLL
PROJTYPE	= DLL

CC		= SC
CPP		= SPP
JAVAC		= sj
MAKE		= SMAKE
RC		= RCC
HC		= HC31
ASM		= SC
DISASM		= OBJ2ASM
LNK		= LINK
DLLS		= 

HEADERS		= F:\Dev\dm\mfc\include\32-bit\afxwin.h F:\Dev\dm\mfc\include\32-bit\afx.h F:\Dev\dm\mfc\include\32-bit\afxver_.h  \
		F:\Dev\dm\mfc\include\32-bit\afxv_w32.h F:\Dev\dm\mfc\include\32-bit\afxcoll.h F:\Dev\dm\mfc\include\32-bit\afxstat_.h F:\Dev\dm\mfc\include\32-bit\afxtls_.h  \
		F:\Dev\dm\mfc\include\32-bit\afxres.h F:\Dev\dm\mfc\include\32-bit\afxmsg_.h F:\Dev\dm\mfc\include\32-bit\afxdd_.h F:\Dev\dm\mfc\include\32-bit\afxext.h  \
		F:\Dev\dm\mfc\include\32-bit\afxdlgs.h F:\Dev\dm\mfc\include\32-bit\afxcmn.h F:\Dev\dm\mfc\include\32-bit\afxmt.h F:\Dev\dm\mfc\include\32-bit\afxtempl.h  \
		F:\Dev\dm\mfc\include\32-bit\afxplex_.h stdafx.h F:\Dev\dm\mfc\include\afxwin.h F:\Dev\dm\mfc\include\afx.h  \
		F:\Dev\dm\mfc\include\afxver_.h F:\Dev\dm\mfc\include\afxv_w32.h F:\Dev\dm\include\windows.h F:\Dev\dm\include\win32\scdefs.h  \
		F:\Dev\dm\include\win32\WINDOWS.H F:\Dev\dm\include\excpt.h F:\Dev\dm\include\stdarg.h F:\Dev\dm\include\windef.h  \
		F:\Dev\dm\include\win32\WINDEF.H F:\Dev\dm\include\winnt.h F:\Dev\dm\include\win32\WINNT.H F:\Dev\dm\include\ctype.h  \
		F:\Dev\dm\include\pshpack4.h F:\Dev\dm\include\win32\pshpack4.h F:\Dev\dm\include\poppack.h F:\Dev\dm\include\win32\poppack.h  \
		F:\Dev\dm\include\win32\pshpack2.h F:\Dev\dm\include\string.h F:\Dev\dm\include\winbase.h F:\Dev\dm\include\win32\WINBASE.H  \
		F:\Dev\dm\include\winerror.h F:\Dev\dm\include\win32\WINERROR.H F:\Dev\dm\include\wingdi.h F:\Dev\dm\include\win32\WINGDI.H  \
		F:\Dev\dm\include\pshpack1.h F:\Dev\dm\include\win32\pshpack1.h F:\Dev\dm\include\pshpack2.h F:\Dev\dm\include\winuser.h  \
		F:\Dev\dm\include\win32\WINUSER.H F:\Dev\dm\include\winnls.h F:\Dev\dm\include\win32\WINNLS.H F:\Dev\dm\include\wincon.h  \
		F:\Dev\dm\include\win32\WINCON.H F:\Dev\dm\include\winver.h F:\Dev\dm\include\win32\WINVER.H F:\Dev\dm\include\winreg.h  \
		F:\Dev\dm\include\win32\WINREG.H F:\Dev\dm\include\winnetwk.h F:\Dev\dm\include\win32\WINNETWK.H F:\Dev\dm\include\zmouse.h  \
		F:\Dev\dm\include\win32\zmouse.h F:\Dev\dm\include\commctrl.h F:\Dev\dm\include\win32\COMMCTRL.H F:\Dev\dm\include\prsht.h  \
		F:\Dev\dm\include\win32\PRSHT.H F:\Dev\dm\include\tchar.h F:\Dev\dm\include\mbstring.h F:\Dev\dm\include\mbctype.h  \
		F:\Dev\dm\include\stdio.h F:\Dev\dm\include\stdlib.h F:\Dev\dm\include\time.h F:\Dev\dm\include\limits.h  \
		F:\Dev\dm\include\stddef.h F:\Dev\dm\include\crtdbg.h F:\Dev\dm\mfc\include\afxcoll.h F:\Dev\dm\mfc\include\afxstat_.h  \
		F:\Dev\dm\mfc\include\afxtls_.h F:\Dev\dm\include\shellapi.h F:\Dev\dm\include\win32\shellapi.h F:\Dev\dm\mfc\include\afxres.h  \
		F:\Dev\dm\mfc\include\afxmsg_.h F:\Dev\dm\mfc\include\afxdd_.h F:\Dev\dm\mfc\include\afxext.h F:\Dev\dm\mfc\include\afxdlgs.h  \
		F:\Dev\dm\include\commdlg.h F:\Dev\dm\include\win32\COMMDLG.H F:\Dev\dm\include\richedit.h F:\Dev\dm\include\win32\RICHEDIT.H  \
		F:\Dev\dm\mfc\include\afxcmn.h F:\Dev\dm\mfc\include\afxmt.h F:\Dev\dm\mfc\include\afxtempl.h F:\Dev\dm\mfc\include\afxplex_.h  \
		F:\Dev\dm\include\new.h F:\Dev\dm\include\eh.h ..\shared\SeException.h eraser.h  \
		resource.h ByteEdit.h EraserDll.h Common.h  \
		random.h F:\Dev\dm\include\wincrypt.h F:\Dev\dm\include\win32\WINCRYPT.H F:\Dev\dm\include\tlhelp32.h  \
		F:\Dev\dm\include\win32\TLHELP32.H eraserdllinternal.h Pass.h FillMemoryWith.h  \
		Custom.h CustomMethodEdit.h ..\shared\\FlatListCtrl.h ..\shared\\FlatHeaderCtrl.h  \
		..\shared\\MemDC.h DOD.h Options.h OptionsDlg.h  \
		OptionPages.h ReportDialog.h RND.h Gutmann.h  \
		File.h NTFS.h FreeSpace.h FAT.h  \
		Stack.h ..\shared\FileHelper.h ..\shared\key.h F:\Dev\dm\include\regstr.h  \
		F:\Dev\dm\include\win32\REGSTR.H F:\Dev\dm\include\winioctl.h F:\Dev\dm\include\win32\WINIOCTL.H PassEditDlg.h  \
		Tiger.h ..\shared\UserInfo.h F:\Dev\dm\include\winperf.h F:\Dev\dm\include\win32\WINPERF.H  \
		F:\Dev\dm\include\process.h ..\shared\Utils.h ..\shared\MemDC.h ..\shared\FlatHeaderCtrl.h  \
		..\shared\FlatListCtrl.h 

DEFFILE		= Eraser.def

!IF $(DEBUG)
OUTPUTDIR	= .
CREATEOUTPUTDIR	=
TARGETDIR	= .
CREATETARGETDIR	=

SYMROOT		= 
SYMS		= 
LIBS		= KERNEL32.LIB GDI32.LIB USER32.LIB 

CFLAGS		=  -Ae -mn -C -WD -S -3 -a8 -c -gf 
LFLAGS		=  /CO /NOI /DO /DE /NOPACKF /XN /NT /ENTRY:_DllMainCRTStartup /BAS:268435456 /A:512 /IMPL:$(TARGETDIR)\$$SCW$$.LIB /RC   :Eraser.RES
DEFINES		= -D_DEBUG=1 -D_X86_=1 -D_MT=1 -D_WINDLL=1 -D_USRDLL=1 -D_MBCS=1
!ELSE
OUTPUTDIR	= h:\eraser\eraser-source\eraserdll\release
!IF EXIST (h:\eraser\eraser-source\eraserdll\release)
CREATEOUTPUTDIR	=
!ELSE
CREATEOUTPUTDIR	= if not exist $(OUTPUTDIR)\*.* md $(OUTPUTDIR)
!ENDIF
TARGETDIR	= h:\eraser\eraser-source\eraserdll\release
!IF EXIST (h:\eraser\eraser-source\eraserdll\release)
CREATETARGETDIR	=
!ELSE
CREATETARGETDIR	= if not exist $(TARGETDIR)\*.* md $(TARGETDIR)
!ENDIF

SYMROOT		= 
SYMS		= 
LIBS		= KERNEL32.LIB GDI32.LIB USER32.LIB 

CFLAGS		=  -Ae -mn -o+time -WD -3 -a8 -c 
LFLAGS		=  /M /NOI /DO /DE /DET /PACKF /XN /NT /ENTRY:_DllMainCRTStartup /BAS:268435456 /A:512 /IMPL:$(TARGETDIR)\$$SCW$$.LIB /RC   :h:\eraser\eraser-source\eraserdll\release\Eraser.RES
DEFINES		= -D_X86_=1 -D_MT=1 -D_WINDLL=1 -D_USRDLL=1 -DDMARS -D_MBCS=1
!ENDIF

HFLAGS		= $(CFLAGS) 
MFLAGS		= MASTERPROJ=$(PROJ) 
LIBFLAGS	=  /C 
RESFLAGS	=  -v -32 
DEBUGGERFLAGS	=  
AFLAGS		= $(CFLAGS) 
HELPFLAGS	= 

MODEL		= N

PAR		= PROJS BATS OBJS

RCDEFINES	= 

INCLUDES	= 

INCLUDEDOBJS	= 

OBJS		=  $(OUTPUTDIR)\ByteEdit.OBJ  $(OUTPUTDIR)\Common.OBJ  $(OUTPUTDIR)\Custom.OBJ  \
		 $(OUTPUTDIR)\CustomMethodEdit.OBJ  $(OUTPUTDIR)\DOD.OBJ  $(OUTPUTDIR)\Eraser.OBJ  $(OUTPUTDIR)\FAT.OBJ  \
		 $(OUTPUTDIR)\File.OBJ  $(OUTPUTDIR)\FillMemoryWith.OBJ  $(OUTPUTDIR)\FreeSpace.OBJ  $(OUTPUTDIR)\Gutmann.OBJ  \
		 $(OUTPUTDIR)\NTFS.OBJ  $(OUTPUTDIR)\OptionPages.OBJ  $(OUTPUTDIR)\Options.OBJ  $(OUTPUTDIR)\OptionsDlg.OBJ  \
		 $(OUTPUTDIR)\Pass.OBJ  $(OUTPUTDIR)\PassEditDlg.OBJ  $(OUTPUTDIR)\Random.OBJ  $(OUTPUTDIR)\ReportDialog.OBJ  \
		 $(OUTPUTDIR)\RND.OBJ  $(OUTPUTDIR)\Sboxes.OBJ  $(OUTPUTDIR)\StdAfx.OBJ  $(OUTPUTDIR)\Tiger.OBJ 

RCFILES		= $(OUTPUTDIR)\Eraser.RES

RESFILES	= 

HELPFILES	= 

BATS		= 

.SUFFIXES: .C .CP .CPP .CXX .CC .H .HPP .HXX .COM .EXE .DLL .LIB .RTF .DLG .ASM .RES .RC .OBJ 

.C.OBJ:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.c

.CPP.OBJ:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.cpp

.CXX.OBJ:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.cxx

.CC.OBJ:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.cc

.CP.OBJ:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.cp

.H.SYM:
	$(CC) $(HFLAGS) $(DEFINES) $(INCLUDES) -HF -o$(*B).sym $*.h

.HPP.SYM:
	$(CC) $(HFLAGS) $(DEFINES) $(INCLUDES) -HF -o$(*B).sym $*.hpp

.HXX.SYM:
	$(CC) $(HFLAGS) $(DEFINES) $(INCLUDES) -HF -o$(*B).sym $*.hxx

.C.EXP:
	$(CPP) $(CFLAGS) $(DEFINES) $(INCLUDES)   $*.c   -o$*.lst

.CPP.EXP:
	$(CPP) $(CFLAGS) $(DEFINES) $(INCLUDES) $*.cpp -o$*.lst

.CXX.EXP:
	$(CPP) $(CFLAGS) $(DEFINES) $(INCLUDES) $*.cxx -o$*.lst

.CP.EXP:
	$(CPP) $(CFLAGS) $(DEFINES) $(INCLUDES)  $*.cp  -o$*.lst

.CC.EXP:
	$(CPP) $(CFLAGS) $(DEFINES) $(INCLUDES)  $*.cc  -o$*.lst

.ASM.EXP:
	$(CPP) $(CFLAGS) $(DEFINES) $(INCLUDES) $*.asm -o$*.lst

.OBJ.COD:
	$(DISASM) $*.OBJ -c

!IF $(DEBUG)
.OBJ.EXE:
		$(LNK) $(LFLAGS) @<<$(PROJ).LNK
ByteEdit.OBJ+
Common.OBJ+
Custom.OBJ+
CustomMethodEdit.OBJ+
DOD.OBJ+
Eraser.OBJ+
FAT.OBJ+
File.OBJ+
FillMemoryWith.OBJ+
FreeSpace.OBJ+
Gutmann.OBJ+
NTFS.OBJ+
OptionPages.OBJ+
Options.OBJ+
OptionsDlg.OBJ+
Pass.OBJ+
PassEditDlg.OBJ+
Random.OBJ+
ReportDialog.OBJ+
RND.OBJ+
Sboxes.OBJ+
StdAfx.OBJ+
Tiger.OBJ
$$SCW$$.DLL
NUL
KERNEL32.LIB GDI32.LIB USER32.LIB 
Eraser.def;
<<
!ELSE
.OBJ.EXE:
		$(LNK) $(LFLAGS) @$(PROJ).LNK<<
h:\eraser\eraser-source\eraserdll\release\ByteEdit.OBJ+
h:\eraser\eraser-source\eraserdll\release\Common.OBJ+
h:\eraser\eraser-source\eraserdll\release\Custom.OBJ+
h:\eraser\eraser-source\eraserdll\release\CustomMethodEdit.OBJ+
h:\eraser\eraser-source\eraserdll\release\DOD.OBJ+
h:\eraser\eraser-source\eraserdll\release\Eraser.OBJ+
h:\eraser\eraser-source\eraserdll\release\FAT.OBJ+
h:\eraser\eraser-source\eraserdll\release\File.OBJ+
h:\eraser\eraser-source\eraserdll\release\FillMemoryWith.OBJ+
h:\eraser\eraser-source\eraserdll\release\FreeSpace.OBJ+
h:\eraser\eraser-source\eraserdll\release\Gutmann.OBJ+
h:\eraser\eraser-source\eraserdll\release\NTFS.OBJ+
h:\eraser\eraser-source\eraserdll\release\OptionPages.OBJ+
h:\eraser\eraser-source\eraserdll\release\Options.OBJ+
h:\eraser\eraser-source\eraserdll\release\OptionsDlg.OBJ+
h:\eraser\eraser-source\eraserdll\release\Pass.OBJ+
h:\eraser\eraser-source\eraserdll\release\PassEditDlg.OBJ+
h:\eraser\eraser-source\eraserdll\release\Random.OBJ+
h:\eraser\eraser-source\eraserdll\release\ReportDialog.OBJ+
h:\eraser\eraser-source\eraserdll\release\RND.OBJ+
h:\eraser\eraser-source\eraserdll\release\Sboxes.OBJ+
h:\eraser\eraser-source\eraserdll\release\StdAfx.OBJ+
h:\eraser\eraser-source\eraserdll\release\Tiger.OBJ
h:\eraser\eraser-source\eraserdll\release\\$$SCW$$.DLL
EraserDll.MAP
KERNEL32.LIB GDI32.LIB USER32.LIB 
Eraser.def;
<<
!ENDIF

.RTF.HLP:
	$(HC) $(HELPFLAGS) $*.HPJ

.ASM.OBJ:
	$(ASM) $(AFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.asm

.RC.RES: 
	$(RC) $(RCDEFINES) $(RESFLAGS) $(INCLUDES) $*.rc -o$*.res

.DLG.RES:
	echo ^#include "windows.h" >$$$*.rc
	echo ^IF EXIST "$*.h" >>$$$*.rc
	echo ^#include "$*.h" >>$$$*.rc
	echo ^#include "$*.dlg" >>$$$*.rc
	$(RC) $(RCDEFINES) $(RESFLAGS) $(INCLUDES) $$$*.rc
	-del $*.res
	-ren $$$*.res $*.res



all:	noteout createdir $(PRECOMPILE) $(SYMS) $(OBJS) $(INCLUDEDOBJS) $(POSTCOMPILE) $(TARGETDIR)\$(PROJ).$(PROJTYPE) $(POSTLINK) _done


all2:	createdir $(PRECOMPILE) $(SYMS) $(OBJS) $(INCLUDEDOBJS) $(POSTCOMPILE) $(TARGETDIR)\$(PROJ).$(PROJTYPE) $(POSTLINK) _done

noteout:
	REM Output to $(OUTPUTDIR)

createdir:
	$(CREATEOUTPUTDIR)
	$(CREATETARGETDIR)
	
$(TARGETDIR)\$(PROJ).$(PROJTYPE): $(OBJS) $(INCLUDEDOBJS) $(RCFILES) $(RESFILES) $(HELPFILES) $(DEFFILE)
			-del $(TARGETDIR)\$(PROJ).$(PROJTYPE)
!IF $(DEBUG)
		$(LNK) $(LFLAGS) @<<$(PROJ).LNK
ByteEdit.OBJ+
Common.OBJ+
Custom.OBJ+
CustomMethodEdit.OBJ+
DOD.OBJ+
Eraser.OBJ+
FAT.OBJ+
File.OBJ+
FillMemoryWith.OBJ+
FreeSpace.OBJ+
Gutmann.OBJ+
NTFS.OBJ+
OptionPages.OBJ+
Options.OBJ+
OptionsDlg.OBJ+
Pass.OBJ+
PassEditDlg.OBJ+
Random.OBJ+
ReportDialog.OBJ+
RND.OBJ+
Sboxes.OBJ+
StdAfx.OBJ+
Tiger.OBJ
$$SCW$$.DLL
NUL
KERNEL32.LIB GDI32.LIB USER32.LIB 
Eraser.def;
<<
!ELSE
		$(LNK) $(LFLAGS) @<<$(PROJ).LNK
h:\eraser\eraser-source\eraserdll\release\ByteEdit.OBJ+
h:\eraser\eraser-source\eraserdll\release\Common.OBJ+
h:\eraser\eraser-source\eraserdll\release\Custom.OBJ+
h:\eraser\eraser-source\eraserdll\release\CustomMethodEdit.OBJ+
h:\eraser\eraser-source\eraserdll\release\DOD.OBJ+
h:\eraser\eraser-source\eraserdll\release\Eraser.OBJ+
h:\eraser\eraser-source\eraserdll\release\FAT.OBJ+
h:\eraser\eraser-source\eraserdll\release\File.OBJ+
h:\eraser\eraser-source\eraserdll\release\FillMemoryWith.OBJ+
h:\eraser\eraser-source\eraserdll\release\FreeSpace.OBJ+
h:\eraser\eraser-source\eraserdll\release\Gutmann.OBJ+
h:\eraser\eraser-source\eraserdll\release\NTFS.OBJ+
h:\eraser\eraser-source\eraserdll\release\OptionPages.OBJ+
h:\eraser\eraser-source\eraserdll\release\Options.OBJ+
h:\eraser\eraser-source\eraserdll\release\OptionsDlg.OBJ+
h:\eraser\eraser-source\eraserdll\release\Pass.OBJ+
h:\eraser\eraser-source\eraserdll\release\PassEditDlg.OBJ+
h:\eraser\eraser-source\eraserdll\release\Random.OBJ+
h:\eraser\eraser-source\eraserdll\release\ReportDialog.OBJ+
h:\eraser\eraser-source\eraserdll\release\RND.OBJ+
h:\eraser\eraser-source\eraserdll\release\Sboxes.OBJ+
h:\eraser\eraser-source\eraserdll\release\StdAfx.OBJ+
h:\eraser\eraser-source\eraserdll\release\Tiger.OBJ
h:\eraser\eraser-source\eraserdll\release\\$$SCW$$.DLL
EraserDll.MAP
KERNEL32.LIB GDI32.LIB USER32.LIB 
Eraser.def;
<<
!ENDIF

			-ren $(TARGETDIR)\$$SCW$$.$(PROJTYPE) $(PROJ).$(PROJTYPE)
			-del $(TARGETDIR)\$(PROJ).LIB
			-ren $(TARGETDIR)\$$SCW$$.LIB $(PROJ).LIB
			-echo $(TARGETDIR)\$(PROJ).$(PROJTYPE) built

_done:
		REM  Project is up to date

buildall:	clean	all


clean:
		-del $(TARGETDIR)\$$SCW$$.$(PROJTYPE)
		-del $(PROJ).CLE
		-del $(OUTPUTDIR)\SCPH.SYM
		-del EraserDll.dpd
		-del $(OBJS)
		-del $(OUTPUTDIR)\Eraser.RES

cleanres:
		-del $(OUTPUTDIR)\Eraser.RES

res:		cleanres $(RCFILES) all


link:
!IF $(DEBUG)
		$(LNK) $(LFLAGS) @<<$(PROJ).LNK
ByteEdit.OBJ+
Common.OBJ+
Custom.OBJ+
CustomMethodEdit.OBJ+
DOD.OBJ+
Eraser.OBJ+
FAT.OBJ+
File.OBJ+
FillMemoryWith.OBJ+
FreeSpace.OBJ+
Gutmann.OBJ+
NTFS.OBJ+
OptionPages.OBJ+
Options.OBJ+
OptionsDlg.OBJ+
Pass.OBJ+
PassEditDlg.OBJ+
Random.OBJ+
ReportDialog.OBJ+
RND.OBJ+
Sboxes.OBJ+
StdAfx.OBJ+
Tiger.OBJ
$$SCW$$.DLL
NUL
KERNEL32.LIB GDI32.LIB USER32.LIB 
Eraser.def;
<<
!ELSE
		$(LNK) $(LFLAGS) @<<$(PROJ).LNK
h:\eraser\eraser-source\eraserdll\release\ByteEdit.OBJ+
h:\eraser\eraser-source\eraserdll\release\Common.OBJ+
h:\eraser\eraser-source\eraserdll\release\Custom.OBJ+
h:\eraser\eraser-source\eraserdll\release\CustomMethodEdit.OBJ+
h:\eraser\eraser-source\eraserdll\release\DOD.OBJ+
h:\eraser\eraser-source\eraserdll\release\Eraser.OBJ+
h:\eraser\eraser-source\eraserdll\release\FAT.OBJ+
h:\eraser\eraser-source\eraserdll\release\File.OBJ+
h:\eraser\eraser-source\eraserdll\release\FillMemoryWith.OBJ+
h:\eraser\eraser-source\eraserdll\release\FreeSpace.OBJ+
h:\eraser\eraser-source\eraserdll\release\Gutmann.OBJ+
h:\eraser\eraser-source\eraserdll\release\NTFS.OBJ+
h:\eraser\eraser-source\eraserdll\release\OptionPages.OBJ+
h:\eraser\eraser-source\eraserdll\release\Options.OBJ+
h:\eraser\eraser-source\eraserdll\release\OptionsDlg.OBJ+
h:\eraser\eraser-source\eraserdll\release\Pass.OBJ+
h:\eraser\eraser-source\eraserdll\release\PassEditDlg.OBJ+
h:\eraser\eraser-source\eraserdll\release\Random.OBJ+
h:\eraser\eraser-source\eraserdll\release\ReportDialog.OBJ+
h:\eraser\eraser-source\eraserdll\release\RND.OBJ+
h:\eraser\eraser-source\eraserdll\release\Sboxes.OBJ+
h:\eraser\eraser-source\eraserdll\release\StdAfx.OBJ+
h:\eraser\eraser-source\eraserdll\release\Tiger.OBJ
h:\eraser\eraser-source\eraserdll\release\\$$SCW$$.DLL
EraserDll.MAP
KERNEL32.LIB GDI32.LIB USER32.LIB 
Eraser.def;
<<
!ENDIF

		-del $(TARGETDIR)\$(PROJ).$(PROJTYPE)
		-ren $(TARGETDIR)\$$SCW$$.$(PROJTYPE) $(PROJ).$(PROJTYPE)
			-del $(TARGETDIR)\$(PROJ).LIB
			-ren $(TARGETDIR)\$$SCW$$.LIB $(PROJ).LIB





!IF EXIST (EraserDll.dpd)
!INCLUDE EraserDll.dpd
!ENDIF



$(OUTPUTDIR)\ByteEdit.OBJ:	ByteEdit.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\ByteEdit.obj ByteEdit.cpp



$(OUTPUTDIR)\Common.OBJ:	Common.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\Common.obj Common.cpp



$(OUTPUTDIR)\Custom.OBJ:	Custom.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\Custom.obj Custom.cpp



$(OUTPUTDIR)\CustomMethodEdit.OBJ:	CustomMethodEdit.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\CustomMethodEdit.obj CustomMethodEdit.cpp



$(OUTPUTDIR)\DOD.OBJ:	DOD.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\DOD.obj DOD.cpp



$(OUTPUTDIR)\Eraser.OBJ:	Eraser.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\Eraser.obj Eraser.cpp



$(OUTPUTDIR)\FAT.OBJ:	FAT.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\FAT.obj FAT.cpp



$(OUTPUTDIR)\File.OBJ:	File.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\File.obj File.cpp



$(OUTPUTDIR)\FillMemoryWith.OBJ:	FillMemoryWith.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\FillMemoryWith.obj FillMemoryWith.cpp



$(OUTPUTDIR)\FreeSpace.OBJ:	FreeSpace.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\FreeSpace.obj FreeSpace.cpp



$(OUTPUTDIR)\Gutmann.OBJ:	Gutmann.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\Gutmann.obj Gutmann.cpp



$(OUTPUTDIR)\NTFS.OBJ:	NTFS.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\NTFS.obj NTFS.cpp



$(OUTPUTDIR)\OptionPages.OBJ:	OptionPages.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\OptionPages.obj OptionPages.cpp



$(OUTPUTDIR)\Options.OBJ:	Options.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\Options.obj Options.cpp



$(OUTPUTDIR)\OptionsDlg.OBJ:	OptionsDlg.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\OptionsDlg.obj OptionsDlg.cpp



$(OUTPUTDIR)\Pass.OBJ:	Pass.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\Pass.obj Pass.cpp



$(OUTPUTDIR)\PassEditDlg.OBJ:	PassEditDlg.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\PassEditDlg.obj PassEditDlg.cpp



$(OUTPUTDIR)\Random.OBJ:	Random.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\Random.obj Random.cpp



$(OUTPUTDIR)\ReportDialog.OBJ:	ReportDialog.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\ReportDialog.obj ReportDialog.cpp



$(OUTPUTDIR)\RND.OBJ:	RND.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\RND.obj RND.cpp



$(OUTPUTDIR)\Sboxes.OBJ:	Sboxes.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\Sboxes.obj Sboxes.cpp



$(OUTPUTDIR)\StdAfx.OBJ:	StdAfx.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\StdAfx.obj StdAfx.cpp



$(OUTPUTDIR)\Tiger.OBJ:	Tiger.cpp
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\Tiger.obj Tiger.cpp



$(OUTPUTDIR)\Eraser.RES:	Eraser.rc
		$(RC) $(RESFLAGS) $(RCDEFINES) $(INCLUDES) -o$(OUTPUTDIR)\Eraser.res Eraser.rc




