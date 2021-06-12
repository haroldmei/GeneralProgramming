// Eraser.cpp
//
// Eraser. Secure data removal. For Windows.
// Copyright ?1997-2001  Sami Tolvanen (sami@tolvanen.com).
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include "stdafx.h"
#include "Eraser.h"
#include "EraserDll.h"
#include "Common.h"

#include "Options.h"
#include "OptionsDlg.h"
#include "ReportDialog.h"

#include "RND.h"
#include "DOD.h"
#include "Gutmann.h"
#include "Custom.h"

#include "File.h"
#include "NTFS.h"
#include "FreeSpace.h"
#include "FAT.h"

#include "..\shared\FileHelper.h"
#include "..\shared\key.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCriticalSection critical_section_DIR;

/////////////////////////////////////////////////////////////////////////////
// CEraserApp
void LockDirOp()
{
	critical_section_DIR.Lock();
}
void UnLockDirOp()
{
	critical_section_DIR.Unlock();
}

BEGIN_MESSAGE_MAP(CEraserApp, CWinApp)
    //{{AFX_MSG_MAP(CEraserApp)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEraserApp construction

CEraserApp::CEraserApp()
{
    _set_se_translator(SeTranslator);
}

BOOL CEraserApp::InitInstance()
{
    // determine the operating system
    OSVERSIONINFO ov;
    ZeroMemory(&ov, sizeof(OSVERSIONINFO));
    ov.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&ov);

    //isWindowsNT = (ov.dwPlatformId == VER_PLATFORM_WIN32_NT);
	//isWindowsNT =  (ov.dwPlatformId == VER_PLATFORM_WIN32_NT  && (ov.dwMajorVersion > 5 || (ov.dwMajorVersion == 5 && ov.dwMinorVersion >= 1)));
	isWindowsNT = (ov.dwPlatformId == VER_PLATFORM_WIN32_NT  && (ov.dwMajorVersion >= 4));

    // initialize the context array
    eraserContextArrayAccess();
    ZeroMemory(eraserContextArray, sizeof(CEraserContext*) * (ERASER_MAX_CONTEXT + 1));

    // initialize reference counter
    eraserLibraryUnlock();

    return CWinApp::InitInstance();
}

int CEraserApp::ExitInstance()
{
    // clean up
    eraserLibraryUnlock();
    eraserEnd();

    return CWinApp::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CEraserApp object

CEraserApp theApp;

/////////////////////////////////////////////////////////////////////////////
// definitions

UINT eraserThread(LPVOID);

/////////////////////////////////////////////////////////////////////////////
// misc. helpers

static inline bool
overwriteFileName(LPCTSTR szFile, LPTSTR szLastFileName)
{
    TCHAR szNewName[MAX_PATH];
    PTCHAR pszLastSlash;
    E_UINT32 i, j, index, length;

    try {
        strncpy(szLastFileName, szFile, MAX_PATH);
        pszLastSlash = strrchr(szLastFileName, '\\');

        if (pszLastSlash == NULL) {
            return false;
        }

        index = (pszLastSlash - szLastFileName) / sizeof(TCHAR);

        strncpy(szNewName, szLastFileName, MAX_PATH);
        length = (E_UINT32)strlen(szLastFileName);

        for (i = 0; i < ERASER_FILENAME_PASSES; i++) {
            // replace each non-'.' character with a random letter
            isaacFill((E_PUINT8)(szNewName + index + 1), (length - index - 1) * sizeof(TCHAR));

            for (j = index + 1; j < length; j++) {
                if (szLastFileName[j] != '.') {
                    szNewName[j] = ERASER_SAFEARRAY[((E_UINT16)szNewName[j]) % ERASER_SAFEARRAY_SIZE];
                } else {
                    szNewName[j] = '.';
                }
            }

            if (MoveFile(szLastFileName, szNewName)) {
                strncpy(szLastFileName, szNewName, MAX_PATH);
            }
        }

        return true;
    }
    catch (...) {
        ASSERT(0);
    }

    ZeroMemory(szNewName, MAX_PATH * sizeof(TCHAR));

    return false;
}

static inline bool
isFolderEmpty(LPCTSTR szFolder)
{
    bool            bEmpty = true;
    HANDLE          hFind;
    WIN32_FIND_DATA wfdData;
    CString         strFolder(szFolder);

    // make sure that the folder name ends with a backslash
    if (strFolder[strFolder.GetLength() - 1] != '\\') {
        strFolder += "\\";
    }

    hFind = FindFirstFile((LPCTSTR)(strFolder + "*"), &wfdData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (bitSet(wfdData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY) &&
                ISNT_SUBFOLDER(wfdData.cFileName)) {
                continue;
            }

            bEmpty = false;
            break;
        }
        while (FindNextFile(hFind, &wfdData));

        VERIFY(FindClose(hFind));
    }
    return bEmpty;
}

// removes all files and subfolders from the given folder, use with caution
static inline bool
emptyFolder(LPCTSTR szFolder)
{
    bool            bEmpty = true;
    HANDLE          hFind;
    WIN32_FIND_DATA wfdData;
    CString         strFolder(szFolder);
    CString         strFile;

    // make sure that the folder name ends with a backslash
    if (strFolder[strFolder.GetLength() - 1] != '\\') {
        strFolder += "\\";
    }

    hFind = FindFirstFile((LPCTSTR)(strFolder + "*"), &wfdData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            strFile = strFolder + wfdData.cFileName;

            if (bitSet(wfdData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY)) {
                if (IS_SUBFOLDER(wfdData.cFileName)) {
                    if (eraserError(eraserRemoveFolderA((LPVOID)(LPCTSTR)strFile,
                            (E_UINT16)strFile.GetLength(), ERASER_REMOVE_RECURSIVELY))) {
                        bEmpty = false;
                    }
                }
            } else {
                if (eraserError(eraserRemoveFileA((LPVOID)(LPCTSTR)strFile,
                        (E_UINT16)strFile.GetLength()))) {
                    bEmpty = false;
                }
            }
        }
        while (FindNextFile(hFind, &wfdData));

        VERIFY(FindClose(hFind));
    } else {
        return false;
    }
    return bEmpty;
}

/////////////////////////////////////////////////////////////////////////////
// context helpers

/*static inline */ERASER_RESULT
contextToAddress(E_IN ERASER_HANDLE param1, E_OUT CEraserContext **pointer)
{
    // if you don't count all the error checking, this is quite fast, O(1)
    if (!eraserContextOK(param1)) {
        return ERASER_ERROR_PARAM1;
    } else if (!AfxIsValidAddress(pointer, sizeof(CEraserContext*))) {
        return ERASER_ERROR_PARAM2;
    } else {
        try {
            E_UINT16 index = eraserContextIndex(param1);
            eraserContextArrayAccess();
            *pointer = eraserContextArray[index];
            if (*pointer == 0) {
                return ERASER_ERROR_PARAM1;
            } else if (!AfxIsValidAddress(*pointer, sizeof(CEraserContext))) {
                eraserContextArray[index] = 0;
                *pointer = 0;
                return ERASER_ERROR_PARAM1;
            } else if ((*pointer)->m_uContextID != eraserContextID(param1) ||
                       (*pointer)->m_uOwnerThreadID != ::GetCurrentThreadId()) {
                // invalid context id or attempt to access from another thread
                *pointer = 0;
                return ERASER_ERROR_DENIED;
            }
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

/////////////////////////////////////////////////////////////////////////////
// exported functions

// Library initialization
//
ERASER_EXPORT
eraserInit()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserInit\n"));

    try {
        // increase the reference counter
        eraserLibraryInit();
        randomInit();

        return ERASER_OK;
    } catch (...) {
        ASSERT(0);
        return ERASER_ERROR_EXCEPTION;
    }
}

ERASER_EXPORT
eraserEnd()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserEnd\n"));

    ERASER_RESULT result = ERASER_OK;

    // decrease the reference counter
    eraserLibraryUninit();

    eraserContextArrayAccess();

    try {
        // if nobody else is using this instance of the library, clean up
        if (!eraserIsLibraryInit()) {
            for (ERASER_HANDLE i = ERASER_MIN_CONTEXT; i <= ERASER_MAX_CONTEXT; i++) {
                if (eraserContextArray[i] != 0) {
                    if (AfxIsValidAddress(eraserContextArray[i], sizeof(CEraserContext))) {
                        try {
                            // this will stop unsynchronized access to the context
                            //VERIFY(eraserOK(eraserStop(i)));
                            eraserOK(eraserStop(i));//meihaiyuan
                            eraserContextLock(eraserContextArray[i]);
                            delete eraserContextArray[i];
                        } catch (...) {
                            ASSERT(0);
                            result = ERASER_ERROR_EXCEPTION;
                        }
                    }
                    eraserContextArray[i] = 0;
                }
            }
        }

        // decrease prng reference counter
        randomEnd();
    } catch (...) {
        ASSERT(0);
        result = ERASER_ERROR_EXCEPTION;
    }

    return result;
}


// Context creation and destruction
//
ERASER_EXPORT
eraserCreateContext(E_OUT ERASER_HANDLE *param1)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserCreateContext\n"));

    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    } else if (!AfxIsValidAddress(param1, sizeof(ERASER_HANDLE))) {
        return ERASER_ERROR_PARAM1;
    } else {
        try {
            *param1 = ERASER_INVALID_CONTEXT;
        } catch (...) {
            return ERASER_ERROR_PARAM1;
        }
    }

    eraserContextArrayAccess();

    // find first available context
    for (E_UINT16 i = ERASER_MAX_CONTEXT; i >= ERASER_MIN_CONTEXT; i--) {
        if (eraserContextArray[i] == 0) {
            try {
                eraserContextArray[i] = new CEraserContext();
            } catch (...) {
                eraserContextArray[i] = 0;
                return ERASER_ERROR_MEMORY;
            }

            try {
                if (!loadLibrarySettings(&eraserContextArray[i]->m_lsSettings)) {
                    setLibraryDefaults(&eraserContextArray[i]->m_lsSettings);
                }

                // reseed the prng
                isaacSeed();

                // context identification
                isaacFill((E_PUINT8)&eraserContextArray[i]->m_uContextID, sizeof(E_UINT16));
                eraserContextArray[i]->m_uOwnerThreadID = ::GetCurrentThreadId();

                // context handle is a combination of eraserContextArray
                // index and the number of times this function is called
                eraserSetContextID(*param1, eraserContextArray[i]->m_uContextID);
                eraserSetContextIndex(*param1, i);
            } catch (...) {
                ASSERT(0);
                if (AfxIsValidAddress(eraserContextArray[i], sizeof(CEraserContext))) {
                    delete eraserContextArray[i];
                    eraserContextArray[i] = 0;
                }
                return ERASER_ERROR_EXCEPTION;
            }
            return ERASER_OK;
        }
    }

    return ERASER_ERROR_CONTEXT;
}

ERASER_EXPORT
eraserCreateContextEx(E_OUT ERASER_HANDLE *param1, E_IN ERASER_METHOD param2, E_IN E_UINT16 param3, E_IN E_UINT8 param4)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserCreateContextEx\n"));
    if (!eraserIsValidMethod(param2)) {
        return ERASER_ERROR_PARAM2;
    } else if (param3 > PASSES_MAX || param3 < 1) {
        return ERASER_ERROR_PARAM3;
    }

    // this one does all our basic sanity checks
    ERASER_RESULT result = eraserCreateContext(param1);
    if (eraserError(result)) {
        return result;
    } else {
        try {
            CEraserContext *context = 0;
            if (eraserOK(contextToAddress(*param1, &context))) {
                eraserContextAccess(context);
                if (param4) {
                    context->m_lsSettings.m_uItems = param4;
                }
                switch (param2) {
                case ERASER_METHOD_LIBRARY:
                    // use standard library settings loaded by eraserCreateContext
                    break;
                case ERASER_METHOD_GUTMANN:
                    context->m_lsSettings.m_nFileMethodID = GUTMANN_METHOD_ID;
                    context->m_lsSettings.m_nUDSMethodID  = GUTMANN_METHOD_ID;
                    break;
                case ERASER_METHOD_DOD:
                    context->m_lsSettings.m_nFileMethodID = DOD_METHOD_ID;
                    context->m_lsSettings.m_nUDSMethodID  = DOD_METHOD_ID;
                    break;
                case ERASER_METHOD_DOD_E:
                    context->m_lsSettings.m_nFileMethodID = DOD_E_METHOD_ID;
                    context->m_lsSettings.m_nUDSMethodID  = DOD_E_METHOD_ID;
                    break;
                case ERASER_METHOD_PSEUDORANDOM:
                    context->m_lsSettings.m_nFileMethodID = RANDOM_METHOD_ID;
                    context->m_lsSettings.m_nUDSMethodID  = RANDOM_METHOD_ID;
                    context->m_lsSettings.m_nFileRandom   = param3;
                    context->m_lsSettings.m_nUDSRandom    = param3;
                    break;
                default:
                    NODEFAULT;
                }

                return ERASER_OK;
            }
            return ERASER_ERROR_CONTEXT;

        } catch (...) {
            ASSERT(0);
            try {
                eraserDestroyContext(*param1);
            } catch (...) {
            }
            return ERASER_ERROR_EXCEPTION;
        }
    }
}

ERASER_EXPORT
eraserDestroyContext(E_IN ERASER_HANDLE param1)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserDestroyContext\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else {
        try {
            // make sure we are not running this one
            VERIFY(eraserOK(eraserStop(param1)));
            // remove from array
            eraserContextArrayAccess();
            eraserContextArray[eraserContextIndex(param1)] = 0;
            eraserContextArrayRelease();
            // free the memory
            eraserContextLock(context);
            delete context;
            context = 0;
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserIsValidContext(E_IN ERASER_HANDLE param1)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceQuery("eraserIsValidContext\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else {
        return ERASER_OK;
    }
}


// Data type
//
ERASER_EXPORT
eraserSetDataType(E_IN ERASER_HANDLE param1, E_IN ERASER_DATA_TYPE param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserSetDataType\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!eraserIsValidDataType(param2)) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (eraserInternalIsRunning(context)) {
        return ERASER_ERROR_RUNNING;
    } else {
        try {
            eraserContextAccess(context);
            if (context->m_saData.GetSize() == 0) {
                context->m_edtDataType = param2;
            } else {
                // cannot change data type after adding items to erase
                return ERASER_ERROR_DENIED;
            }
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserGetDataType(E_IN ERASER_HANDLE param1, E_OUT ERASER_DATA_TYPE *param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceQuery("eraserGetDataType\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param2, sizeof(ERASER_DATA_TYPE))) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else {
        try {
            eraserContextAccess(context);
            *param2 = context->m_edtDataType;
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}


// Assign data
//
ERASER_EXPORT
eraserAddItemA(E_IN ERASER_HANDLE param1, E_IN LPVOID param2, E_IN E_UINT16 param3)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserAddItemA\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (param3 < _MAX_DRIVE || param3 > _MAX_PATH) {
        return ERASER_ERROR_PARAM3;
    } else if (!AfxIsValidString((LPCSTR)param2, param3)) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (eraserInternalIsRunning(context)) {
        return ERASER_ERROR_RUNNING;
    } else {
        try {
            LPCSTR szItem = (LPCSTR)param2;

            // if the item is a file, a fully qualified name is required,
            // drive must be given as "X:\\"

            if (!isalpha(szItem[0]) || szItem[1] != ':' || szItem[2] != '\\') {
                return ERASER_ERROR_PARAM2;
            }

            eraserContextAccess(context);

            if ((context->m_edtDataType == ERASER_DATA_FILES  && strlen(szItem) > _MAX_PATH) ||
                (context->m_edtDataType == ERASER_DATA_DRIVES && strlen(szItem) > _MAX_DRIVE)) {
                return ERASER_ERROR_PARAM2;
            } else {
#ifdef _UNICODE
				LPWSTR	lpItem;

				lpItem = new wchar_t[strlen(szItem) * sizeof(wchar_t)];
				if (lpItem == NULL)
				{
					return ERASER_ERROR_MEMORY;
				}

				asciToUnicode(szItem, lpItem);
				context->m_saData.Add(lpItem);
#else
                context->m_saData.Add(szItem);
#endif // #ifdef _UNICODE
            }
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserAddItemW(E_IN ERASER_HANDLE param1, E_IN LPVOID param2, E_IN E_UINT16 param3)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserAddItemW\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (param3 < _MAX_DRIVE || param3 > _MAX_PATH) {
        return ERASER_ERROR_PARAM3;
    } else if (!AfxIsValidString((LPCWSTR)param2, param3)) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (eraserInternalIsRunning(context)) {
        return ERASER_ERROR_RUNNING;
    } else {
        try {
			LPCWSTR szItem = (LPCWSTR) param2;

            // if the item is a file, a fully qualified name is required,
            // drive must be given as "X:\\"

            if (!iswalpha(szItem[0]) || szItem[1] != L':' || szItem[2] != L'\\') {
                return ERASER_ERROR_PARAM2;
            }

            eraserContextAccess(context);

            if ((context->m_edtDataType == ERASER_DATA_FILES  && wcslen(szItem) > _MAX_PATH) ||
                (context->m_edtDataType == ERASER_DATA_DRIVES && wcslen(szItem) > _MAX_DRIVE)) {
                return ERASER_ERROR_PARAM2;
            } else {
#ifdef _UNICODE
				context->m_saData.Add(szItem);
#else
			// Converting UNICODE String to ANSI
			CString	oItem;

			unicodeToCString(szItem, oItem);
			context->m_saData.Add(oItem);
#endif // #ifdef _UNICODE
            }
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserClearItems(E_IN ERASER_HANDLE param1)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserClearItems\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (eraserInternalIsRunning(context)) {
        return ERASER_ERROR_RUNNING;
    } else {
        try {
            eraserContextAccess(context);
            context->m_saData.RemoveAll();
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}


// Notification
//
ERASER_EXPORT
eraserSetWindow(E_IN ERASER_HANDLE param1, E_IN HWND param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserSetWindow\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!IsWindow(param2)) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else {
        try {
            eraserContextAccess(context);
            context->m_hwndWindow = param2;
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserGetWindow(E_IN ERASER_HANDLE param1, E_OUT HWND* param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceQuery("eraserGetWindow\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param2, sizeof(HWND))) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else {
        try {
            eraserContextAccess(context);
            *param2 = context->m_hwndWindow;
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserSetWindowMessage(E_IN ERASER_HANDLE param1, E_IN E_UINT32 param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserSetWindowMessage\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else {
        try {
            eraserContextAccess(context);
            context->m_uWindowMessage = param2;
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserGetWindowMessage(E_IN ERASER_HANDLE param1, E_OUT E_PUINT32 param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceQuery("eraserGetWindowMessage\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param2, sizeof(E_UINT32))) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else {
        try {
            eraserContextAccess(context);
            *param2 = context->m_uWindowMessage;
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}


// Statistics
//
ERASER_EXPORT
eraserStatGetArea(E_IN ERASER_HANDLE param1, E_OUT E_PUINT64 param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceProgress("eraserStatGetArea\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param2, sizeof(E_UINT64))) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (eraserInternalIsRunning(context)) {
        return ERASER_ERROR_RUNNING;
    } else {
        try {
            eraserContextAccess(context);
            *param2 = context->m_uStatErasedArea;
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserStatGetTips(E_IN ERASER_HANDLE param1, E_OUT E_PUINT64 param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceProgress("eraserStatGetTips\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param2, sizeof(E_UINT64))) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (eraserInternalIsRunning(context)) {
        return ERASER_ERROR_RUNNING;
    } else {
        try {
            eraserContextAccess(context);
            *param2 = context->m_uStatTips;
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserStatGetWiped(E_IN ERASER_HANDLE param1, E_OUT E_PUINT64 param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceProgress("eraserStatGetWiped\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param2, sizeof(E_UINT64))) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (eraserInternalIsRunning(context)) {
        return ERASER_ERROR_RUNNING;
    } else {
        try {
            eraserContextAccess(context);
            *param2 = context->m_uStatWiped;
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserStatGetTime(E_IN ERASER_HANDLE param1, E_OUT E_PUINT32 param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceProgress("eraserStatGetTime\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param2, sizeof(E_UINT32))) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (eraserInternalIsRunning(context)) {
        return ERASER_ERROR_RUNNING;
    } else {
        try {
            eraserContextAccess(context);
            *param2 = context->m_uStatTime;
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}


// Display
//
ERASER_EXPORT
eraserDispFlags(E_IN ERASER_HANDLE param1, E_OUT E_PUINT8 param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceProgress("eraserDispFlags\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param2, sizeof(E_UINT8))) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (!eraserInternalIsRunning(context)) {
        return ERASER_ERROR_NOTRUNNING;
    } else {
        try {
            eraserContextAccess(context);
            // low-order byte
            *param2 = (E_UINT8)context->m_uProgressFlags;
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}


// Progress information
//
ERASER_EXPORT
eraserProgGetTimeLeft(E_IN ERASER_HANDLE param1, E_OUT E_PUINT32 param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceProgress("eraserProgGetTimeLeft\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param2, sizeof(E_UINT32))) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (!eraserInternalIsRunning(context)) {
        return ERASER_ERROR_NOTRUNNING;
    } else {
        try {
            eraserContextAccess(context);
            *param2 = context->m_uProgressTimeLeft;
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserProgGetPercent(E_IN ERASER_HANDLE param1, E_OUT E_PUINT8 param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceProgress("eraserProgGetPercent\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param2, sizeof(E_UINT8))) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (!eraserInternalIsRunning(context)) {
        return ERASER_ERROR_NOTRUNNING;
    } else {
        try {
            eraserContextAccess(context);
            *param2 = min(context->m_uProgressPercent, (E_UINT8)100);
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserProgGetTotalPercent(E_IN ERASER_HANDLE param1, E_OUT E_PUINT8 param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceProgress("eraserProgGetTotalPercent\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param2, sizeof(E_UINT8))) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (!eraserInternalIsRunning(context)) {
        return ERASER_ERROR_NOTRUNNING;
    } else {
        try {
            eraserContextAccess(context);
            *param2 = min(context->m_uProgressTotalPercent, (E_UINT8)100);
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserProgGetCurrentPass(E_IN ERASER_HANDLE param1, E_OUT E_PUINT16 param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceProgress("eraserProgGetCurrentPass\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param2, sizeof(E_UINT16))) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (!eraserInternalIsRunning(context)) {
        return ERASER_ERROR_NOTRUNNING;
    } else {
        try {
            eraserContextAccess(context);
            *param2 = context->m_uProgressCurrentPass;
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserProgGetPasses(E_IN ERASER_HANDLE param1, E_OUT E_PUINT16 param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceProgress("eraserProgGetPasses\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param2, sizeof(E_UINT16))) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (!eraserInternalIsRunning(context)) {
        return ERASER_ERROR_NOTRUNNING;
    } else {
        try {
            eraserContextAccess(context);
            *param2 = context->m_uProgressPasses;
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserProgGetMessageA(E_IN ERASER_HANDLE param1, E_OUT LPVOID param2, E_INOUT E_PUINT16 param3)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceProgress("eraserProgGetMessageA\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param3, sizeof(E_UINT16))) {
        return ERASER_ERROR_PARAM3;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (!eraserInternalIsRunning(context)) {
        return ERASER_ERROR_NOTRUNNING;
    } else {
        try {
            eraserContextAccess(context);
            LPSTR pszError = (LPSTR)param2;
            if (pszError == 0) {
                *param3 = (E_UINT16)(context->m_strProgressMessage.GetLength() + 1);
                return ERASER_OK;
            } else if (*param3 < 1) {
                return ERASER_ERROR_PARAM3;
            } else if (!AfxIsValidAddress((LPCSTR)pszError, *param3)) {
                return ERASER_ERROR_PARAM2;
            }
            ZeroMemory(pszError, *param3);
#ifndef _UNICODE
            lstrcpyn(pszError, (LPCSTR)context->m_strProgressMessage, *param3);
#else
			LPSTR	lpError;
			int		iSize;

			iSize = WideCharToMultiByte(CP_ACP, 0, context->m_strProgressMessage, -1, NULL, 0, NULL, NULL);

			lpError = new char[iSize];

			if (lpError == NULL)
			{
				return ERASER_ERROR_MEMORY;
			}

			ZeroMemory(lpError, context->m_strProgressMessage.GetLength() + 1);
			WideCharToMultiByte(CP_ACP, 0, context->m_strProgressMessage, -1, lpError, iSize, NULL, NULL);

            lstrcpyn(pszError, lpError, *param3);

			delete lpError;
#endif // #ifndef _UNICODE
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserProgGetMessageW(E_IN ERASER_HANDLE param1, E_OUT LPVOID param2, E_INOUT E_PUINT16 param3)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceProgress("eraserProgGetMessageW\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param3, sizeof(E_UINT16))) {
        return ERASER_ERROR_PARAM3;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (!eraserInternalIsRunning(context)) {
        return ERASER_ERROR_NOTRUNNING;
    } else {
        try {
            eraserContextAccess(context);
            LPWSTR pszError = (LPWSTR)param2;
            if (pszError == 0) {
                *param3 = (E_UINT16)(context->m_strProgressMessage.GetLength() + 1);
                return ERASER_OK;
            } else if (*param3 < 1) {
                return ERASER_ERROR_PARAM3;
            } else if (!AfxIsValidAddress((LPCWSTR)pszError, (*param3) * sizeof(wchar_t))) {
                return ERASER_ERROR_PARAM2;
            }
            ZeroMemory(pszError, (*param3) * sizeof(wchar_t));
#ifdef _UNICODE
			lstrcpynW(pszError, (LPCWSTR)context->m_strProgressMessage, *param3);
#else
			LPSTR	szError = new char[*param3];
			if (szError == NULL)
			{
				return ERASER_ERROR_MEMORY;
			}
			ZeroMemory(szError, *param3);
            lstrcpyn(szError, (LPCSTR)context->m_strProgressMessage, *param3);
			asciiToUnicode(szError, pszError);
			delete szError;
#endif // #ifndef _UNICODE
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserProgGetCurrentDataStringA(E_IN ERASER_HANDLE param1, E_OUT LPVOID param2, E_INOUT E_PUINT16 param3)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceProgress("eraserProgGetCurrentDataStringA\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param3, sizeof(E_UINT16))) {
        return ERASER_ERROR_PARAM3;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (!eraserInternalIsRunning(context)) {
        return ERASER_ERROR_NOTRUNNING;
    } else {
        try {
            eraserContextAccess(context);
            LPSTR pszError = (LPSTR)param2;
            if (pszError == 0) {
                *param3 = (E_UINT16)(context->m_strData.GetLength() + 1);
                return ERASER_OK;
            } else if (*param3 < 1) {
                return ERASER_ERROR_PARAM3;
            } else if (!AfxIsValidAddress((LPCSTR)pszError, *param3)) {
                return ERASER_ERROR_PARAM2;
            }
            ZeroMemory(pszError, *param3);
#ifndef _UNICODE
            lstrcpyn(pszError, (LPCSTR)context->m_strData, *param3);
#else
			LPSTR	lpError;
			int		iSize;

			iSize = WideCharToMultiByte(CP_ACP, 0, context->m_strData, -1, NULL, 0, NULL, NULL);

			lpError = new char[iSize];

			if (lpError == NULL)
			{
				return ERASER_ERROR_MEMORY;
			}

			ZeroMemory(lpError, context->m_strData.GetLength() + 1);
			WideCharToMultiByte(CP_ACP, 0, context->m_strData, -1, lpError, iSize, NULL, NULL);

            lstrcpyn(pszError, lpError, *param3);

			delete lpError;
#endif // #ifndef _UNICODE
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserProgGetCurrentDataStringW(E_IN ERASER_HANDLE param1, E_OUT LPVOID param2, E_INOUT E_PUINT16 param3)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceProgress("eraserProgGetCurrentDataStringW\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param3, sizeof(E_UINT16))) {
        return ERASER_ERROR_PARAM3;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (!eraserInternalIsRunning(context)) {
        return ERASER_ERROR_NOTRUNNING;
    } else {
        try {
            eraserContextAccess(context);
            LPWSTR pszError = (LPWSTR)param2;
            if (pszError == 0) {
                *param3 = (E_UINT16)(context->m_strData.GetLength() + 1);
                return ERASER_OK;
            } else if (*param3 < 1) {
                return ERASER_ERROR_PARAM3;
            } else if (!AfxIsValidAddress((LPCWSTR)pszError, (*param3) * sizeof(wchar_t))) {
                return ERASER_ERROR_PARAM2;
            }
            ZeroMemory(pszError, *param3 * sizeof(wchar_t));
#ifdef _UNICODE
            lstrcpynW(pszError, (LPCWSTR)context->m_strData, *param3);
#else
			LPSTR	szError = new char[*param3];
			if (szError == NULL)
			{
				return ERASER_ERROR_MEMORY;
			}
			ZeroMemory(szError, *param3);
            lstrcpyn(szError, (LPCSTR)context->m_strData, *param3);
			asciiToUnicode(szError, pszError);
			delete szError;
#endif // #ifndef _UNICODE
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

// Control
//
ERASER_EXPORT
eraserStart(E_IN ERASER_HANDLE param1)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserStart\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (eraserInternalIsRunning(context)) {
        return ERASER_ERROR_RUNNING;
    } else {
        try {
            eraserContextAccess(context);
            // preset flags
            context->m_evDone.SetEvent();
            context->m_evKillThread.ResetEvent();
            // create the thread
            context->m_pwtThread = AfxBeginThread(eraserThread, (LPVOID)context);
            if (context->m_pwtThread == NULL) {
                return ERASER_ERROR_THREAD;
            }
            // start operation
            eraserContextRelease();
            context->m_evStart.SetEvent();
        } catch (...) {
            ASSERT(0);
            eraserStop(param1);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserStartSync(E_IN ERASER_HANDLE param1)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserStartSync\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (eraserInternalIsRunning(context)) {
        return ERASER_ERROR_RUNNING;
    } else {
        try {
            eraserContextAccess(context);
            // preset flags
            context->m_evDone.SetEvent();
            context->m_evKillThread.ResetEvent();
            context->m_evStart.SetEvent();
            eraserContextRelease();
            // start erasing
            if (eraserThread((LPVOID)context) == EXIT_SUCCESS) {
                return ERASER_OK;
            } else {
                return ERASER_ERROR;
            }
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
    }
}


ERASER_EXPORT
eraserStop(E_IN ERASER_HANDLE param1)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserStop\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else {
        try {
            // set the kill flag
            context->m_evKillThread.SetEvent();
            context->m_evStart.SetEvent();
            // if in test mode, enable execution again
            eraserContextAccess(context);
            if (context->m_uTestMode) {
                context->m_evTestContinue.SetEvent();
            }
            eraserContextRelease();
            // two minutes should be enough for any thread (don't quote me)
            if (WaitForSingleObject(context->m_evThreadKilled, 120000) != WAIT_OBJECT_0) {
                // if the thread is still active, just kill it
                eraserContextRelock();
                if (AfxIsValidAddress(context->m_pwtThread, sizeof(CWinThread))) {
                    E_UINT32 uStatus = 0;
                    if (::GetExitCodeThread(context->m_pwtThread->m_hThread, &uStatus) &&
                        uStatus == STILL_ACTIVE) {
                        VERIFY(::TerminateThread(context->m_pwtThread->m_hThread, (E_UINT32)ERASER_ERROR));
                    }
                }
                context->m_evThreadKilled.SetEvent();
            }
            eraserContextRelock();
            context->m_pwtThread = 0;
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserIsRunning(E_IN ERASER_HANDLE param1, E_OUT E_PUINT8 param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceQuery("eraserIsRunning\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param2, sizeof(E_UINT8))) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else {
        try {
            eraserContextAccess(context);
            *param2 = 0;
            if (eraserInternalIsRunning(context)) {
                *param2 = 1;
            }
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

// Result
//
ERASER_EXPORT
eraserTerminated(E_IN ERASER_HANDLE param1, E_OUT E_PUINT8 param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceQuery("eraserTerminated\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param2, sizeof(E_UINT8))) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (eraserInternalIsRunning(context)) {
        return ERASER_ERROR_RUNNING;
    } else {
        try {
            eraserContextAccess(context);
            *param2 = 0;
            if (eraserInternalTerminated(context)) {
                *param2 = 1;
            }
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserCompleted(E_IN ERASER_HANDLE param1, E_OUT E_PUINT8 param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceQuery("eraserCompleted\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param2, sizeof(E_UINT8))) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (eraserInternalIsRunning(context)) {
        return ERASER_ERROR_RUNNING;
    } else {
        try {
            eraserContextAccess(context);
            *param2 = 0;
            if (eraserInternalCompleted(context)) {
                *param2 = 1;
            }
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserFailed(E_IN ERASER_HANDLE param1, E_OUT E_PUINT8 param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceQuery("eraserFailed\n");
    ERASER_RESULT result = eraserCompleted(param1, param2);

    if (eraserOK(result)) {
        try {
            if (*param2) {
                *param2 = 0;
            } else {
                *param2 = 1;
            }
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
    }

    return result;
}

ERASER_EXPORT
eraserErrorStringCount(E_IN ERASER_HANDLE param1, E_OUT E_PUINT16 param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceQuery("eraserErrorStringCount\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param2, sizeof(E_UINT16))) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (eraserInternalIsRunning(context)) {
        return ERASER_ERROR_RUNNING;
    } else {
        try {
            eraserContextAccess(context);
            *param2 = (E_UINT16)context->m_saError.GetSize();
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserErrorStringA(E_IN ERASER_HANDLE param1, E_IN E_UINT16 param2, E_OUT LPVOID param3, E_INOUT E_PUINT16 param4)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceQuery("eraserErrorStringA\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param4, sizeof(E_UINT16))) {
        return ERASER_ERROR_PARAM4;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (eraserInternalIsRunning(context)) {
        return ERASER_ERROR_RUNNING;
    } else {
        try {
            eraserContextAccess(context);
            if (context->m_saError.GetSize() <= param2) {
                return ERASER_ERROR_PARAM2;
            }
            LPSTR pszError = (LPSTR)param3;
            if (pszError == 0) {
                *param4 = (E_UINT16)(context->m_saError[param2].GetLength() + 1);
                return ERASER_OK;
            } else if (*param4 < 1) {
                return ERASER_ERROR_PARAM4;
            } else if (!AfxIsValidAddress((LPCSTR)pszError, *param4)) {
                return ERASER_ERROR_PARAM3;
            }
            ZeroMemory(pszError, *param4);
#ifndef _UNICODE
            lstrcpyn(pszError, (LPCSTR)context->m_saError[param2], *param4);
#else
			LPSTR	lpError;
			int		iSize;

			iSize = WideCharToMultiByte(CP_ACP, 0, context->m_saError[param2], -1, NULL, 0, NULL, NULL);

			lpError = new char[iSize];

			if (lpError == NULL)
			{
				return ERASER_ERROR_MEMORY;
			}

			ZeroMemory(lpError, context->m_saError[param2].GetLength() + 1);
			WideCharToMultiByte(CP_ACP, 0, context->m_saError[param2], -1, lpError, iSize, NULL, NULL);

            lstrcpyn(pszError, lpError, *param4);

			delete lpError;
#endif // #ifndef _UNICODE
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserErrorStringW(E_IN ERASER_HANDLE param1, E_IN E_UINT16 param2, E_OUT LPVOID param3, E_INOUT E_PUINT16 param4)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceQuery("eraserErrorStringW\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param4, sizeof(E_UINT16))) {
        return ERASER_ERROR_PARAM4;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (eraserInternalIsRunning(context)) {
        return ERASER_ERROR_RUNNING;
    } else {
        try {
            eraserContextAccess(context);
            if (context->m_saError.GetSize() <= param2) {
                return ERASER_ERROR_PARAM2;
            }
            LPWSTR pszError = (LPWSTR)param3;
            if (pszError == 0) {
                *param4 = (E_UINT16)(context->m_saError[param2].GetLength() + 1);
                return ERASER_OK;
            } else if (*param4 < 1) {
                return ERASER_ERROR_PARAM4;
            } else if (!AfxIsValidAddress((LPCWSTR)pszError, (*param4) * sizeof(wchar_t))) {
                return ERASER_ERROR_PARAM3;
            }
            ZeroMemory(pszError, (*param4) * sizeof(wchar_t));
#ifdef _UNICODE
            lstrcpyn(pszError, (LPCWSTR)context->m_saError[param2], *param4);
#else
			LPSTR	szError = new char[*param4];
			if (szError == NULL)
			{
				return ERASER_ERROR_MEMORY;
			}
			ZeroMemory(szError, *param4);
            lstrcpyn(szError, (LPCSTR)context->m_saError[param2], *param4);
			asciiToUnicode(szError, pszError);
			delete szError;
#endif // #ifndef _UNICODE
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserFailedCount(E_IN ERASER_HANDLE param1, E_OUT E_PUINT32 param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceQuery("eraserFailedCount\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param2, sizeof(E_UINT32))) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (eraserInternalIsRunning(context)) {
        return ERASER_ERROR_RUNNING;
    } else {
        try {
            eraserContextAccess(context);
            *param2 = (E_UINT32)context->m_saFailed.GetSize();
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserFailedStringA(E_IN ERASER_HANDLE param1, E_IN E_UINT32 param2, E_OUT LPVOID param3, E_INOUT E_PUINT16 param4)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceQuery("eraserFailedStringA\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param4, sizeof(E_UINT16))) {
        return ERASER_ERROR_PARAM4;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (eraserInternalIsRunning(context)) {
        return ERASER_ERROR_RUNNING;
    } else {
        try {
            eraserContextAccess(context);
            if ((E_UINT32)context->m_saFailed.GetSize() <= param2) {
                return ERASER_ERROR_PARAM2;
            }
            LPSTR pszError = (LPSTR)param3;
            if (pszError == 0) {
                *param4 = (E_UINT16)(context->m_saFailed[param2].GetLength() + 1);
                return ERASER_OK;
            } else if (*param4 < 1) {
                return ERASER_ERROR_PARAM4;
            } else if (!AfxIsValidAddress((LPCSTR)pszError, *param4)) {
                return ERASER_ERROR_PARAM3;
            }
            ZeroMemory(pszError, *param4);
#ifndef _UNICODE
            lstrcpyn(pszError, (LPCSTR)context->m_saFailed[param2], *param4);
#else
			LPSTR	lpError;
			int		iSize;

			iSize = WideCharToMultiByte(CP_ACP, 0, context->m_saFailed[param2], -1, NULL, 0, NULL, NULL);

			lpError = new char[iSize];

			if (lpError == NULL)
			{
				return ERASER_ERROR_MEMORY;
			}

			ZeroMemory(lpError, context->m_saFailed[param2].GetLength() + 1);
			WideCharToMultiByte(CP_ACP, 0, context->m_saFailed[param2], -1, lpError, iSize, NULL, NULL);

            lstrcpyn(pszError, lpError, *param4);

			delete lpError;
#endif // #ifndef _UNICODE
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserFailedStringW(E_IN ERASER_HANDLE param1, E_IN E_UINT32 param2, E_OUT LPVOID param3, E_INOUT E_PUINT16 param4)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceQuery("eraserFailedStringW\n");
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!AfxIsValidAddress(param4, sizeof(E_UINT16))) {
        return ERASER_ERROR_PARAM4;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (eraserInternalIsRunning(context)) {
        return ERASER_ERROR_RUNNING;
    } else {
        try {
            eraserContextAccess(context);
            if ((E_UINT32)context->m_saFailed.GetSize() <= param2) {
                return ERASER_ERROR_PARAM2;
            }
            LPWSTR pszError = (LPWSTR)param3;
            if (pszError == 0) {
                *param4 = (E_UINT16)(context->m_saFailed[param2].GetLength() + 1);
                return ERASER_OK;
            } else if (*param4 < 1) {
                return ERASER_ERROR_PARAM4;
            } else if (!AfxIsValidAddress((LPCWSTR)pszError, (*param4) * sizeof(wchar_t))) {
                return ERASER_ERROR_PARAM3;
            }
            ZeroMemory(pszError, (*param4) * sizeof(wchar_t));
#ifdef _UNICODE
            lstrcpyn(pszError, (LPCWSTR)context->m_saFailed[param2], *param4);
#else
			LPSTR	szError = new char[*param4];
			if (szError == NULL)
			{
				return ERASER_ERROR_MEMORY;
			}
			ZeroMemory(szError, *param4);
            lstrcpyn(szError, (LPCSTR)context->m_saFailed[param2], *param4);
			asciiToUnicode(szError, pszError);
			delete szError;
#endif // #ifndef _UNICODE
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

// Display report
//
ERASER_EXPORT
eraserShowReport(E_IN ERASER_HANDLE param1, E_IN HWND param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserShowReport\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (!IsWindow(param2)) {
        return ERASER_ERROR_PARAM2;
    } else if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (eraserInternalIsRunning(context)) {
        return ERASER_ERROR_RUNNING;
    }

    ERASER_RESULT result = ERASER_OK;
    CWnd wndParent;
    wndParent.Attach(param2);

    try {
        CStringArray straFailures;
        CString strTemp;
        CString strUnit;
        E_UINT32 uIndex, uSize;
        E_UINT64 uTemp;
        double dTime;
        CReportDialog rd;

        // completion
        if (eraserInternalCompleted(context)) {
			rd.m_strCompletion.LoadString(IDS_REPORT_COMPLETED);
        } else if (eraserInternalTerminated(context)) {
			rd.m_strCompletion.LoadString(IDS_REPORT_TERMINATED);
        } else {
            if (context->m_saError.GetSize() > 0) {
				rd.m_strCompletion.LoadString(IDS_REPORT_NOT_COMPLETED);
            } else {
				rd.m_strCompletion.LoadString(IDS_REPORT_NOT_ALLDATA);
            }
        }

        #define reportMaxByteValue    (10 * 1024)
        #define reportMaxkBValue    (1000 * 1024)
        
        #define divideByK(value) \
            (value) = (((value) + 512) / 1024)

        #define setValueAndUnit(value) \
            do { \
                uTemp = (value); \
                if (uTemp > reportMaxByteValue) { \
                    divideByK(uTemp); \
                    if (uTemp > reportMaxkBValue) { \
                        divideByK(uTemp); \
                        strUnit.LoadString(IDS_REPORT_MB); \
                    } else { \
                        strUnit.LoadString(IDS_REPORT_KB); \
                    } \
                } else { \
                    if ((value) != 1) { \
                        strUnit.LoadString(IDS_REPORT_BYTES); \
                    } else { \
                        strUnit.LoadString(IDS_REPORT_BYTE); \
                    } \
                } \
            } while (0)

        // information header
		rd.m_strStatistics.LoadString(IDS_REPORT_STATISTIC);
        // erased area
        setValueAndUnit(context->m_uStatErasedArea);
        strTemp.Format(IDS_REPORT_AREA, uTemp, strUnit);
        rd.m_strStatistics += strTemp;
        // cluster tips
        setValueAndUnit(context->m_uStatTips);
        strTemp.Format(IDS_REPORT_TIPS, uTemp, strUnit);
        rd.m_strStatistics += strTemp;
        // written
        setValueAndUnit(context->m_uStatWiped);
        strTemp.Format(IDS_REPORT_DATAWRITEN, uTemp, strUnit);
        rd.m_strStatistics += strTemp;
        // time
        dTime = (double)context->m_uStatTime / 1000.0f;
        strTemp.Format(IDS_REPORT_TIME, dTime);
        rd.m_strStatistics += strTemp;
        // speed
        if (dTime > 0.0) {
            strTemp.Format(IDS_REPORT_SPEED, (E_UINT64)
                ((((E_INT64)context->m_uStatWiped / dTime) + 512.0f) / 1024.0f));
            rd.m_strStatistics += strTemp;
        }

        uSize = context->m_saError.GetSize();
        for (uIndex = 0; uIndex < uSize; uIndex++) {
            strTemp.Format(IDS_REPORT_ERROR, context->m_saError[uIndex]);
            straFailures.Add(strTemp);
        }

        uSize = context->m_saFailed.GetSize();
        for (uIndex = 0; uIndex < uSize; uIndex++) {
            strTemp.Format(IDS_REPORT_FAILED, context->m_saFailed[uIndex]);
            straFailures.Add(strTemp);
        }

        rd.m_pstraErrorArray = &straFailures;

        rd.DoModal();
    } catch (...) {
        ASSERT(0);
        result = ERASER_ERROR_EXCEPTION;
    }

    wndParent.Detach();
    return result;
}


// Display library options
//
ERASER_EXPORT
eraserShowOptions(E_IN HWND param1, E_IN ERASER_OPTIONS_PAGE param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserShowOptions\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    } else if (!IsWindow(param1)) {
        return ERASER_ERROR_PARAM1;
    }

    E_UINT16 uActive;
    if (param2 == ERASER_PAGE_DRIVE) {
        uActive = 1;
    } else if (param2 == ERASER_PAGE_FILES) {
        uActive = 0;
    } else {
        return ERASER_ERROR_PARAM2;
    }

    ERASER_RESULT result = ERASER_OK;

    CWnd wndParent;
    wndParent.Attach(param1);

    try {
        COptionsDlg dlg(&wndParent);
        dlg.SetActivePage(uActive);

        if (!loadLibrarySettings(&dlg.m_lsSettings))
            setLibraryDefaults(&dlg.m_lsSettings);

        if (dlg.DoModal() == IDOK)
            saveLibrarySettings(&dlg.m_lsSettings);
    } catch (...) {
        ASSERT(0);
        result = ERASER_ERROR_EXCEPTION;
    }

    wndParent.Detach();
    return result;
}


// File / directory deletion
//
ERASER_EXPORT
eraserRemoveFileA(E_IN LPVOID param1, E_IN E_UINT16 param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserRemoveFileA\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    LPCSTR pszFile = (LPCSTR)param1;
    if (!AfxIsValidString(pszFile, param2)) {
        return ERASER_ERROR_PARAM1;
    }

#ifndef _UNICODE
    SetFileAttributes(pszFile, FILE_ATTRIBUTE_NORMAL);
	SetFileAttributes(pszFile, FILE_ATTRIBUTE_NOT_CONTENT_INDEXED);

    if (isWindowsNT) {
        TCHAR szLastFileName[MAX_PATH + 1];

        overwriteFileName(pszFile, szLastFileName);
        return truthToResult(DeleteFile(szLastFileName));
    }

    return truthToResult(DeleteFile(pszFile));
#else
	LPWSTR	lpFile = new wchar_t[param2];

	if (lpFile == NULL)
	{
		return ERASER_ERROR_MEMORY;
	}

	ZeroMemory(lpFile param2 * sizeof(wchar_t));
	asciiToUnicode(pszFile, lpFile);
	ERASER_RESULT er = eraserRemoveFileW(lpFile, param2);
	delete lpFile;

	return er;
#endif // #ifndef _UNICODE
}

ERASER_EXPORT
eraserRemoveFileW(E_IN LPVOID param1, E_IN E_UINT16 param2)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserRemoveFileW\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    LPCWSTR pszFile = (LPCWSTR)param1;
    if (!AfxIsValidString(pszFile, param2 * sizeof(wchar_t))) {
        return ERASER_ERROR_PARAM1;
    }

#ifndef _UNICODE
	CString	oFile;

	unicodeToCString(pszFile, oFile);
	return eraserRemoveFileA((LPVOID)oFile.GetBuffer(), param2);
#else
    SetFileAttributes(pszFile, FILE_ATTRIBUTE_NORMAL);
	SetFileAttributes(pszFile, FILE_ATTRIBUTE_NOT_CONTENT_INDEXED);

    if (isWindowsNT) {
        TCHAR szLastFileName[MAX_PATH + 1];

        overwriteFileName(pszFile, szLastFileName);
        return truthToResult(DeleteFile(szLastFileName));
    }

    return truthToResult(DeleteFile(pszFile));
#endif // #ifndef _UNICODE
}

ERASER_EXPORT
eraserRemoveFolderA(E_IN LPVOID param1, E_IN E_UINT16 param2, E_IN E_UINT8 param3)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserRemoveFolderA\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    LPCSTR pszFolder = (LPCSTR)param1;
    if (!AfxIsValidString(pszFolder, param2)) {
        return ERASER_ERROR_PARAM1;
    }

#ifndef _UNICODE
    SetFileAttributes(pszFolder, FILE_ATTRIBUTE_NORMAL);

    // recursively delete all subfolders and files !?
    if (param3 != ERASER_REMOVE_FOLDERONLY) {
        emptyFolder(pszFolder);
    }

    if (isWindowsNT) {
        if (!isFolderEmpty(pszFolder)) {
            return ERASER_ERROR;
        }

        CString strFolder(pszFolder);
        TCHAR   szLastFileName[MAX_PATH + 1];

        if (strFolder[strFolder.GetLength() - 1] == '\\') {
            strFolder = strFolder.Left(strFolder.GetLength() - 1);
        }

        overwriteFileName((LPCSTR)strFolder, szLastFileName);
        return truthToResult(RemoveDirectory(szLastFileName));
    }

    return truthToResult(RemoveDirectory(pszFolder));
#else
	LPWSTR	lpFolder = new wchar_t[param2];

	if (lpFolder == NULL)
	{
		return ERASER_ERROR_MEMORY;
	}
	ZeroMemory(lpFolder, param2 * sizeof(wchar_t));
	asciiToUnicode(pszFolder, lpFolder);
	ERASER_RESULT	er = eraserRemoveFolderW(lpFolder, param2, param3);
	delete lpFolder;

	return er;
#endif // #ifndef _UNICODE
}

ERASER_EXPORT
eraserRemoveFolderW(E_IN LPVOID param1, E_IN E_UINT16 param2, E_IN E_UINT8 param3)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserRemoveFolderW\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    LPCWSTR pszFolder = (LPCWSTR)param1;
    if (!AfxIsValidString(pszFolder, param2 * sizeof(wchar_t))) {
        return ERASER_ERROR_PARAM1;
    }

#ifndef _UNICODE
	CString	oFolder;

	unicodeToCString(pszFolder, oFolder);
	return eraserRemoveFolderA((LPVOID)oFolder.GetBuffer(), param2, param3);
#else
    SetFileAttributes(pszFolder, FILE_ATTRIBUTE_NORMAL);

    // recursively delete all subfolders and files !?
    if (param3 != ERASER_REMOVE_FOLDERONLY) {
        emptyFolder(pszFolder);
    }

    if (isWindowsNT) {
        if (!isFolderEmpty(pszFolder)) {
            return ERASER_ERROR;
        }

        CString strFolder(pszFolder);
        TCHAR   szLastFileName[MAX_PATH + 1];

        if (strFolder[strFolder.GetLength() - 1] == '\\') {
            strFolder = strFolder.Left(strFolder.GetLength() - 1);
        }

        overwriteFileName((LPCWSTR)strFolder, szLastFileName);
        return truthToResult(RemoveDirectory(szLastFileName));
    }

    return truthToResult(RemoveDirectory(pszFolder));
#endif // #ifndef _UNICODE
}

// Helpers
//
ERASER_EXPORT
eraserGetFreeDiskSpaceA(E_IN LPVOID param1, E_IN E_UINT16 param2, E_OUT E_PUINT64 param3)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserGetFreeDiskSpaceA\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    LPCSTR pszDrive = (LPCSTR)param1;
    if (!AfxIsValidString(pszDrive, param2)) {
        return ERASER_ERROR_PARAM1;
    } else if (!AfxIsValidAddress(param3, sizeof(E_UINT64))) {
        return ERASER_ERROR_PARAM3;
    } else {
        try {
            *param3 = 0;
        } catch (...) {
            return ERASER_ERROR_PARAM3;
        }
    }

#ifndef _UNICODE
    ERASER_RESULT result = ERASER_ERROR;
    HINSTANCE hInst = AfxLoadLibrary(ERASER_MODULENAME_KERNEL);

    if (hInst != NULL) {
        ULARGE_INTEGER FreeBytesAvailableToCaller;
        ULARGE_INTEGER TotalNumberOfBytes;
        ULARGE_INTEGER TotalNumberOfFreeBytes;

        GETDISKFREESPACEEX pGetDiskFreeSpaceEx;

        pGetDiskFreeSpaceEx =
            (GETDISKFREESPACEEX)(GetProcAddress(hInst, ERASER_FUNCTIONNAME_GETDISKFREESPACEEX));

        if (pGetDiskFreeSpaceEx) {
            try {
                if (pGetDiskFreeSpaceEx(pszDrive, &FreeBytesAvailableToCaller,
                        &TotalNumberOfBytes, &TotalNumberOfFreeBytes)) {
                    *param3 = TotalNumberOfFreeBytes.QuadPart;
                    result = ERASER_OK;
                }
            } catch (...) {
                result = ERASER_ERROR_EXCEPTION;
            }
        }

        AfxFreeLibrary(hInst);
    }

    if (eraserError(result)) {
        E_UINT32 dwSecPerClus;
        E_UINT32 dwBytPerSec;
        E_UINT32 dwFreeClus;
        E_UINT32 dwClus;

        try {
            if (GetDiskFreeSpace(pszDrive, &dwSecPerClus, &dwBytPerSec,
                    &dwFreeClus, &dwClus)) {

                *param3 = UInt32x32To64(dwFreeClus, dwSecPerClus * dwBytPerSec);
                result = ERASER_OK;
            }
        } catch (...) {
            result = ERASER_ERROR_EXCEPTION;
        }
    }

    return result;
#else
	LPWSTR	lpDrive = new wchar_t[param2];

	if (lpFile == NULL)
	{
		return ERASER_ERROR_MEMORY;
	}

	ZeroMemory(lpDrive param2 * sizeof(wchar_t));
	asciiToUnicode(pszDrive, lpDrive);
	ERASER_RESULT er = eraserGetFreeDiskSpaceW(lpDrive, param2, param3);
	delete lpDrive;

	return er;
#endif // #ifndef _UNICODE
}

ERASER_EXPORT
eraserGetFreeDiskSpaceW(E_IN LPVOID param1, E_IN E_UINT16 param2, E_OUT E_PUINT64 param3)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserGetFreeDiskSpaceW\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    LPCWSTR pszDrive = (LPCWSTR)param1;
    if (!AfxIsValidString(pszDrive, param2 * sizeof(wchar_t))) {
        return ERASER_ERROR_PARAM1;
    } else if (!AfxIsValidAddress(param3, sizeof(E_UINT64))) {
        return ERASER_ERROR_PARAM3;
    } else {
        try {
            *param3 = 0;
        } catch (...) {
            return ERASER_ERROR_PARAM3;
        }
    }

#ifndef _UNICODE
	CString	oDrive;

	unicodeToCString(pszDrive, oDrive);
	return eraserGetFreeDiskSpaceA((LPVOID)oDrive.GetBuffer(), param2, param3);
#else
    ERASER_RESULT result = ERASER_ERROR;
    HINSTANCE hInst = AfxLoadLibrary(ERASER_MODULENAME_KERNEL);

    if (hInst != NULL) {
        ULARGE_INTEGER FreeBytesAvailableToCaller;
        ULARGE_INTEGER TotalNumberOfBytes;
        ULARGE_INTEGER TotalNumberOfFreeBytes;

        GETDISKFREESPACEEX pGetDiskFreeSpaceEx;

        pGetDiskFreeSpaceEx =
            (GETDISKFREESPACEEX)(GetProcAddress(hInst, ERASER_FUNCTIONNAME_GETDISKFREESPACEEX));

        if (pGetDiskFreeSpaceEx) {
            try {
                if (pGetDiskFreeSpaceEx(pszDrive, &FreeBytesAvailableToCaller,
                        &TotalNumberOfBytes, &TotalNumberOfFreeBytes)) {
                    *param3 = TotalNumberOfFreeBytes.QuadPart;
                    result = ERASER_OK;
                }
            } catch (...) {
                result = ERASER_ERROR_EXCEPTION;
            }
        }

        AfxFreeLibrary(hInst);
    }

    if (eraserError(result)) {
        E_UINT32 dwSecPerClus;
        E_UINT32 dwBytPerSec;
        E_UINT32 dwFreeClus;
        E_UINT32 dwClus;

        try {
            if (GetDiskFreeSpace(pszDrive, &dwSecPerClus, &dwBytPerSec,
                    &dwFreeClus, &dwClus)) {

                *param3 = UInt32x32To64(dwFreeClus, dwSecPerClus * dwBytPerSec);
                result = ERASER_OK;
            }
        } catch (...) {
            result = ERASER_ERROR_EXCEPTION;
        }
    }

    return result;
#endif // #ifndef _UNICODE
}

ERASER_EXPORT
eraserGetClusterSizeA(E_IN LPVOID param1, E_IN E_UINT16 param2, E_OUT E_PUINT32 param3)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserGetClusterSizeA\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    LPCSTR pszDrive = (LPCSTR)param1;
    if (!AfxIsValidString(pszDrive, param2)) {
        return ERASER_ERROR_PARAM1;
    } else if (!AfxIsValidAddress(param3, sizeof(E_UINT64))) {
        return ERASER_ERROR_PARAM3;
    } else {
        try {
            *param3 = 0;
        } catch (...) {
            return ERASER_ERROR_PARAM3;
        }
    }

#ifndef _UNICODE
    ERASER_RESULT result = ERASER_ERROR;

    try {
        result = truthToResult(getClusterSize(pszDrive, *param3));
    } catch (...) {
        result = ERASER_ERROR_EXCEPTION;
    }

    return result;
#else
	LPWSTR	lpDrive = new wchar_t[param2];

	if (lpFile == NULL)
	{
		return ERASER_ERROR_MEMORY;
	}

	ZeroMemory(lpDrive param2 * sizeof(wchar_t));
	asciiToUnicode(pszDrive, lpDrive);
	ERASER_RESULT er = eraserGetClusterSizeW(lpDrive, param2, param3);
	delete lpDrive;

	return er;
#endif // #ifndef _UNICODE
}

ERASER_EXPORT
eraserGetClusterSizeW(E_IN LPVOID param1, E_IN E_UINT16 param2, E_OUT E_PUINT32 param3)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserGetClusterSizeW\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    LPCWSTR pszDrive = (LPCWSTR)param1;
    if (!AfxIsValidString(pszDrive, param2 * sizeof(wchar_t))) {
        return ERASER_ERROR_PARAM1;
    } else if (!AfxIsValidAddress(param3, sizeof(E_UINT64))) {
        return ERASER_ERROR_PARAM3;
    } else {
        try {
            *param3 = 0;
        } catch (...) {
            return ERASER_ERROR_PARAM3;
        }
    }

#ifndef _UNICODE
	CString	oDrive;

	unicodeToCString(pszDrive, oDrive);
	return eraserGetClusterSizeA((LPVOID)oDrive.GetBuffer(), param2, param3);
#else
    ERASER_RESULT result = ERASER_ERROR;

    try {
        result = truthToResult(getClusterSize(pszDrive, *param3));
    } catch (...) {
        result = ERASER_ERROR_EXCEPTION;
    }

    return result;
#endif // #ifndef _UNICODE
}

ERASER_EXPORT
eraserTestEnable(E_IN ERASER_HANDLE param1)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserTestEnable\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (eraserInternalIsRunning(context)) {
        return ERASER_ERROR_RUNNING;
    } else {
        try {
            eraserContextAccess(context);
            context->m_uTestMode = 1;
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}

ERASER_EXPORT
eraserTestContinueProcess(E_IN ERASER_HANDLE param1)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserTestContinueProcess\n"));
    if (!eraserIsLibraryInit()) {
        return ERASER_ERROR_INIT;
    }

    CEraserContext *context = 0;
    if (eraserError(contextToAddress(param1, &context))) {
        return ERASER_ERROR_PARAM1;
    } else if (!eraserInternalIsRunning(context)) {
        return ERASER_ERROR_NOTRUNNING;
    } else {
        try {
            eraserContextAccess(context);
            if (!context->m_uTestMode) {
                return ERASER_ERROR_DENIED;
            }
            context->m_evTestContinue.SetEvent();
        } catch (...) {
            ASSERT(0);
            return ERASER_ERROR_EXCEPTION;
        }
        return ERASER_OK;
    }
}


UINT
eraserThread(LPVOID param1)
{
	LockDirOp();
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    eraserTraceBase(TEXT("eraserThread\n"));
    ASSERT(AfxIsValidAddress(param1, sizeof(CEraserContext)));

    // structured exception handling
    _set_se_translator(SeTranslator);

    // parameters class
    CEraserContext *context = (CEraserContext*)param1;

    try {
        context->m_evThreadKilled.ResetEvent();

        // do not start before told
        WaitForSingleObject(context->m_evStart, INFINITE);
        context->m_evDone.ResetEvent();

        // user wants to terminate already !?
        if (eraserInternalTerminated(context)) {
			UnLockDirOp();
            eraserEndThread(context, EXIT_FAILURE);
        }

        // set default progress information
        eraserDispDefault(context);

        // determine the erasing method
        E_UINT8 nMethodID = (context->m_edtDataType == ERASER_DATA_FILES) ?
                                context->m_lsSettings.m_nFileMethodID : context->m_lsSettings.m_nUDSMethodID;

        // initialize method information to the context
        if (bitSet(nMethodID, BUILTIN_METHOD_ID)) {
            for (E_UINT8 i = 0; i < nBuiltinMethods; i++) {
                if (bmMethods[i].m_nMethodID == nMethodID) {
                    // we need a thread local copy of the method structure
                    // to prevent problems if the calling application runs
                    // multiple threads at the same time
                    context->m_mThreadLocalMethod = bmMethods[i];
                    context->m_lpmMethod = &context->m_mThreadLocalMethod;

                    // restore saved information
                    if (nMethodID == RANDOM_METHOD_ID) {
                        context->m_lpmMethod->m_nPasses =
                            (context->m_edtDataType == ERASER_DATA_FILES) ?
                                context->m_lsSettings.m_nFileRandom :
                                context->m_lsSettings.m_nUDSRandom;
                    }

                    break;
                }
            }
        } else if (nMethodID <= MAX_CUSTOM_METHOD_ID) {
            // find the custom method
            for (E_UINT8 i = 0; i < context->m_lsSettings.m_nCMethods; i++) {
                if (context->m_lsSettings.m_lpCMethods[i].m_nMethodID == nMethodID) {
                    context->m_lpmMethod = &context->m_lsSettings.m_lpCMethods[i];
                    context->m_lpmMethod->m_pwfFunction = wipeFileWithCustom;

                    break;
                }
            }
        }

        // A Bad Thing(TM)
        if (context->m_lpmMethod == 0 || context->m_lpmMethod->m_pwfFunction == 0) {
            eraserAddError(context, IDS_ERROR_INTERNAL);
			UnLockDirOp();
            eraserEndThread(context, EXIT_FAILURE);
        } else {
            // set progress information
            eraserSafeAssign(context, context->m_uProgressPasses,
                             (E_UINT16)context->m_lpmMethod->m_nPasses);
        }

        // allocate write buffer used by all wipe functions
        context->m_puBuffer = (E_PUINT32)VirtualAlloc(NULL, ERASER_DISK_BUFFER_SIZE,
                                                      MEM_COMMIT, PAGE_READWRITE);

        if (context->m_puBuffer == NULL) {
            eraserAddError(context, IDS_ERROR_MEMORY);
			UnLockDirOp();
            eraserEndThread(context, EXIT_FAILURE);
        }

        // we'll see about this...
        bool bCompleted = true;

        if (context->m_edtDataType == ERASER_DATA_FILES) {
            // files

            // number of files to process
            context->m_uProgressWipedFiles = 0;
            context->m_uProgressFiles = context->m_saData.GetSize();

            if (context->m_uProgressFiles > 0) {
                E_UINT32 uLength = 0;
                E_INT32 iPosition = -1;
                TCHAR szShortPath[_MAX_PATH];
                CString strDirectory;
                CStringList strlDirectories[26]; // drive A = 0, ..., Z = 25

                szShortPath[_MAX_PATH - 1] = 0;

                // overwrite files
                while (context->m_uProgressWipedFiles < context->m_uProgressFiles) {
                    if (eraserInternalTerminated(context)) {
                        bCompleted = false;
                        break;
                    }

                    // file to process
                    eraserSafeAssign(context, context->m_strData,
                        context->m_saData[context->m_uProgressWipedFiles]);

                    // partition information
                    getPartitionInformation(context, context->m_strData[0]);

                    // remember which directories to clear
                    if (!isWindowsNT && bitSet(context->m_lsSettings.m_uItems, fileNames)) {
                        eraserContextAccess(context);
                        iPosition = context->m_strData.ReverseFind('\\');

                        if (iPosition > 0) {
                            strDirectory = context->m_strData.Left(iPosition);

                            if (strDirectory.GetLength() > _MAX_DRIVE) {
                                uLength = GetShortPathName(strDirectory, szShortPath, _MAX_PATH - 1);

                                if (uLength > 2 && uLength <= _MAX_PATH) {
                                    strDirectory.Format("%s\\", (LPCTSTR)&szShortPath[2]);
                                    strDirectory.MakeUpper();
                                } else {
                                    strDirectory.Empty();
                                }
                            } else {
                                // root directory
                                strDirectory = "\\";
                            }

                            iPosition = (E_INT32)(context->m_piCurrent.m_szDrive[0] - 'A');

                            if (!strDirectory.IsEmpty() &&
                                strlDirectories[iPosition].Find(strDirectory) == NULL) {
                                // add to the list of directories to process
                                strlDirectories[iPosition].AddHead(strDirectory);
                            }
                        }
                    }

                    // wipe the file
                    eraserBool(bCompleted, wipeFile(context));

                    // next file
                    context->m_uProgressWipedFiles++;

                    // progress
                    eraserSafeAssign(context, context->m_uProgressTotalPercent,
                        (E_UINT8)((context->m_uProgressWipedFiles * 100) / context->m_uProgressFiles));
                    eraserUpdateNotify(context);
                }

                // clear file names
                if (!isWindowsNT && bitSet(context->m_lsSettings.m_uItems, fileNames)) {
                    // no progress
                    context->m_uProgressFolders = 0;

                    for (E_INT32 i = 0; i < 26; i++) {
                        eraserDispFileNames(context);

                        // go through partitions we accessed
                        if (!strlDirectories[i].IsEmpty()) {
                            // partition information
                            eraserSafeAssign(context, context->m_strData,
                                (TCHAR)('A' + i) + CString(":\\"));

                            if (getPartitionInformation(context, context->m_strData[0])) {
                                // list of directories to clear
                                context->m_pstrlDirectories = &strlDirectories[i];

                                eraserBool(bCompleted, wipeFATFileEntries(context,
                                        ERASER_MESSAGE_FILENAMES_RETRY) == WFE_SUCCESS);
                            } else {
                                bCompleted = false;
                            }
                        }
                    }

                    context->m_pstrlDirectories = 0;
                }
            } else {
                // no files to wipe !?
                //eraserAddError(context, IDS_ERROR_NODATA);
            }
        } else {
            // unused space on drive(s)

            // number of drives to process
            context->m_uProgressWipedDrives = 0;
            context->m_uProgressDrives = context->m_saData.GetSize();

            if (context->m_uProgressDrives > 0) {
                while (context->m_uProgressWipedDrives < context->m_uProgressDrives) {
                    if (eraserInternalTerminated(context)) {
                        bCompleted = false;
                        break;
                    }

                    // drive to process
                    eraserSafeAssign(context, context->m_strData,
                        context->m_saData[context->m_uProgressWipedDrives]);

                    // partition information
                    getPartitionInformation(context, context->m_strData[0]);

                    // start progress from the beginning
                    context->m_uProgressTaskPercent = 0;
                    context->m_uProgressFiles = 0;
                    context->m_uProgressFolders = 0;

                    // how many separate tasks, for total progress information
                    countTotalProgressTasks(context);

                    // progress information
                    if (bitSet(context->m_lsSettings.m_uItems, diskClusterTips) ||
                        bitSet(context->m_lsSettings.m_uItems, diskDirEntries)) {
                        if (context->m_piCurrent.m_uCluster > 0) {
                            // set display options
                            eraserDispSearch(context);
                            eraserBeginNotify(context);

                            countFilesOnDrive(context, context->m_strData, context->m_uProgressFiles,
                                              context->m_uProgressFolders);

                            // add entropy to the pool
                            randomAddEntropy((E_PUINT8)&context->m_uProgressFiles, sizeof(E_UINT32));
                            randomAddEntropy((E_PUINT8)&context->m_uProgressFolders, sizeof(E_UINT32));
                        }
                    }

                    // cluster tips
                    if (bitSet(context->m_lsSettings.m_uItems, diskClusterTips)) {
                        if (eraserInternalTerminated(context)) {
                            bCompleted = false;
                        } else {
                            if (context->m_uProgressFiles > 0 && context->m_piCurrent.m_uCluster > 0) {
                                eraserDispClusterTips(context);
                                eraserBool(bCompleted, wipeClusterTips(context));

                                // restore drive
                                eraserSafeAssign(context, context->m_strData,
                                    context->m_saData[context->m_uProgressWipedDrives]);
                            }

                            // task completed
                            increaseTotalProgressPercent(context);
                        }
                    }

                    // free space
                    if (bitSet(context->m_lsSettings.m_uItems, diskFreeSpace)) {
                        if (eraserInternalTerminated(context)) {
                            bCompleted = false;
                        } else {
                            eraserDispDefault(context);
                            eraserBool(bCompleted, wipeFreeSpace(context));

                            // task completed
                            increaseTotalProgressPercent(context);
                        }
                    }

                    // directory entries
                    if (bitSet(context->m_lsSettings.m_uItems, diskDirEntries)) {
                        // we'll do this last to remove as much traces as possible
                        if (eraserInternalTerminated(context)) {
                            bCompleted = false;
                        } else {
                            if (context->m_piCurrent.m_uCluster > 0) {
                                eraserDispDirEntries(context);

                                if (isWindowsNT && isFileSystemNTFS(context->m_piCurrent)) {
                                    // we'll estimate the progress based on MFT size and number of files
                                    eraserBool(bCompleted, wipeNTFSFileEntries(context));
                                } else {
                                    // once again, need to handle the progress ourselves
                                    // but this time it is not necessary to show file names

                                    context->m_uProgressFolders++; // add one for the root directory
                                    eraserBool(bCompleted, wipeFATFileEntries(context,
                                               ERASER_MESSAGE_DIRENTRY_RETRY) == WFE_SUCCESS);
                                }
                            }

                            // no need to call increaseTotalProgressPercent since we have
                            // now completed work for this drive
                        }
                    }

                    // next drive
                    context->m_uProgressWipedDrives++;

                    // progress
                    eraserSafeAssign(context, context->m_uProgressTotalPercent,
                        (E_UINT8)((context->m_uProgressWipedDrives * 100) / context->m_uProgressDrives));
                    eraserUpdateNotify(context);
                }
            } else {
                // nothing to wipe
                eraserAddError(context, IDS_ERROR_NODATA);
            }
        } // unused disk space

        // free previously allocated write buffer
        if (context->m_puBuffer) {
            ZeroMemory(context->m_puBuffer, ERASER_DISK_BUFFER_SIZE);
            VirtualFree(context->m_puBuffer, 0, MEM_RELEASE);
            context->m_puBuffer = 0;
        }

        // set done flag if nothing has failed
        if (bCompleted &&
            context->m_saFailed.GetSize() == 0 && context->m_saError.GetSize() == 0) {
            context->m_evDone.SetEvent();
        }

        // bye.
        if (eraserInternalCompleted(context)) {
			UnLockDirOp();
            eraserEndThread(context, EXIT_SUCCESS);
        } else {
			UnLockDirOp();
            eraserEndThread(context, EXIT_FAILURE);
        }
    } catch (CException *e) {
        handleException(e, context);

        if (context->m_puBuffer) {
            try {
                ZeroMemory(context->m_puBuffer, ERASER_DISK_BUFFER_SIZE);
            } catch (...) {
            }

            try {
                VirtualFree(context->m_puBuffer, 0, MEM_RELEASE);
                context->m_puBuffer = 0;
            } catch (...) {
            }
        }

        try {
			UnLockDirOp();
            eraserEndThread(context, EXIT_FAILURE);
        } catch (...) {
        }
    } catch (...) {
        ASSERT(0);

        try {
            if (context->m_puBuffer) {
                ZeroMemory(context->m_puBuffer, ERASER_DISK_BUFFER_SIZE);
                VirtualFree(context->m_puBuffer, 0, MEM_RELEASE);
                context->m_puBuffer = 0;
            }
        } catch (...) {
        }

        try {
            eraserAddError(context, IDS_ERROR_INTERNAL);
        } catch (...) {
        }

        try {
			UnLockDirOp();
            eraserEndThread(context, EXIT_FAILURE);
        } catch (...) {
        }
    }
	UnLockDirOp();
    return EXIT_FAILURE;
}

E_UINT32 eraserRetrieveValueProgressTimeLeft(LPARAM x)
{
	if (!x)
		return -1;
	return ((CEraserContext *)x)->m_uProgressTimeLeft;
}

E_UINT32 eraserRetrieveValueProgressPercent(LPARAM x)
{
	if (!x)
		return -1;
	return ((CEraserContext *)x)->m_uProgressPercent;
}

E_UINT32 eraserRetrieveValueProgressTotalPercent(LPARAM x)
{
	if (!x)
		return -1;
	return ((CEraserContext *)x)->m_uProgressTotalPercent;
}

E_UINT32 eraserRetrieveValueContextID(LPARAM x)
{
	if (!x)
		return -1;
	return ((CEraserContext *)x)->m_uContextID;
}

E_UINT32 eraserRetrieveValueListIndexSet(LPARAM x, E_UINT32 index)
{
	if (!x)
		return -1;

	
	((CEraserContext *)x)->m_ListIndex = index;
	return 0;//
}

E_UINT32 eraserRetrieveValueListIndex(LPARAM x)
{
	if (!x)
		return -1;

	
	return ((CEraserContext *)x)->m_ListIndex;
}

LPCTSTR eraserRetrieveValueStrData(LPARAM x)
{
	if (!x)
		return FALSE;

	LPCTSTR str = LPCTSTR((((CEraserContext *)x)->m_strData));
	return str;
}

void ContextSet(ERASER_HANDLE  m_ehContext, CTXT_SETTINGS set)
{
	CEraserContext *pointer;
	contextToAddress(m_ehContext, &pointer);
	setBit(pointer->m_lsSettings.m_uItems, set);
}

