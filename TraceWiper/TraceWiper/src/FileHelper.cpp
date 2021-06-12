// FileHelper.cpp
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

//#include "stdafx.h"

#include "afx.h"
#include "afxwin.h"
#include "windef.h"
#include "atlbase.h"
#include "atlcom.h"
#include "atlcomtime.h"

#include "..\..\EraserLib\EraserDll.h"
#include "FileHelper.h"
#include <vector>

void GetLocalHardDrives(vector<CString> saDrives)
{
    DWORD dwDriveMask = GetLogicalDrives();

    if (dwDriveMask)
    {
        CString strDrive;
        TCHAR cRoot;
        UINT uType;

        saDrives.clear();

        for (cRoot = 'A'; cRoot <= 'Z'; cRoot++)
        {
            if (dwDriveMask & 1)
            {
                strDrive = cRoot;
                strDrive += ":\\";

                uType = GetDriveType((LPCTSTR) strDrive);

                if (uType != DRIVE_UNKNOWN &&
                    uType != DRIVE_NO_ROOT_DIR &&
                    uType != DRIVE_CDROM &&
                    uType != DRIVE_REMOTE &&
                    uType != DRIVE_REMOVABLE)
                {
                    saDrives.push_back(strDrive);
                }
            }

            dwDriveMask >>= 1;
        }
    }
}


void
findMatchingFiles(CString strSearch, vector<CString> saFiles, BOOL bSubFolders /*=FALSE*/)
{
    HANDLE          hFind, hFolder;
    WIN32_FIND_DATA wfdData, wfdFolder;

    CString         strFolder;
    CString         strBaseFolder;
    TCHAR           szDrive[_MAX_DRIVE];
    TCHAR           szFolder[_MAX_PATH];

    // get the root folder
    _splitpath((LPCTSTR)strSearch, szDrive, szFolder, NULL, NULL);

    strBaseFolder = szDrive;
    strBaseFolder += szFolder;

    if (strBaseFolder.IsEmpty()) {
        if (GetCurrentDirectory(_MAX_PATH, szFolder) != 0) {
            strBaseFolder = szFolder;
            if (strBaseFolder[strBaseFolder.GetLength() - 1] != '\\') {
                strBaseFolder += "\\";
            }
            strSearch = strBaseFolder + strSearch;
        }
    }

    // the search pattern
    if (strSearch.Find('\\') > 0) {
        strSearch = strSearch.Right(strSearch.GetLength() - 1 -
                                    strSearch.ReverseFind('\\'));
    }

    // browse through all files (and directories)
    hFolder = FindFirstFile((LPCTSTR)(strBaseFolder + "*"), &wfdFolder);

    if (hFolder != INVALID_HANDLE_VALUE) {
        hFind = FindFirstFile((LPCTSTR)(strBaseFolder + strSearch), &wfdData);

        // process the current folder first
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (!bitSet(wfdData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY)) {
                    saFiles.push_back(strBaseFolder + wfdData.cFileName);
                }
            } while (FindNextFile(hFind, &wfdData));

            VERIFY(FindClose(hFind));
        }

        // go through the subfolders then
        if (bSubFolders) {
            while (FindNextFile(hFolder, &wfdFolder)) {
                if (bitSet(wfdFolder.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY) &&
                    IS_SUBFOLDER(wfdFolder.cFileName)) {
                    // found one
                    strFolder = strBaseFolder + wfdFolder.cFileName;
                    strFolder += "\\";
                    // recursive
                    findMatchingFiles(strFolder + strSearch, saFiles, bSubFolders);
                }
            }
        }
        VERIFY(FindClose(hFolder));
    }
}

BOOL
parseDirectory(LPCTSTR szDirectory, vector<CString> saFiles, vector<CString> saDirectories,
               BOOL bSubDirectories, LPDWORD pdwFiles /*=0*/, LPDWORD pdwDirectories /*=0*/)
{
    HANDLE          hFind;
    WIN32_FIND_DATA wfdData;
    CString         strDirectory(szDirectory);

    if (!strDirectory.IsEmpty()) {
        if (pdwDirectories) {
             (*pdwDirectories)++;
        }

        if (strDirectory[strDirectory.GetLength() - 1] != '\\') {
            strDirectory += "\\";
        }

        // add current folder to the beginning of the list
        // --> subfolders will be removed first
        saDirectories.push_back(strDirectory);

        hFind = FindFirstFile((LPCTSTR)(strDirectory + "*"), &wfdData);

        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (bitSet(wfdData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY)) {
                    // skip "." and ".."
                    if (!bSubDirectories || ISNT_SUBFOLDER(wfdData.cFileName)) {
                        continue;
                    }

                    // recursive
                    parseDirectory((LPCTSTR)(strDirectory + wfdData.cFileName),
                                   saFiles,
                                   saDirectories,
                                   bSubDirectories,
                                   pdwFiles,
                                   pdwDirectories);
                } else {
                    saFiles.push_back(strDirectory + wfdData.cFileName);
                    if (pdwFiles) {
                            (*pdwFiles)++;
                    }
                }
            } while (FindNextFile(hFind, &wfdData));

            VERIFY(FindClose(hFind));

            return TRUE;
        }
    }

    return FALSE;
}
