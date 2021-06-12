// File.h
//
// Eraser. Secure data removal. For Windows.
// Copyright � 1997-2001  Sami Tolvanen (sami@tolvanen.com).
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

#ifndef FILE_H
#define FILE_H

typedef class _DataStream {
public:
    _DataStream() {
        clear();
    }

    ~_DataStream() {
        clear();
    }

    CString m_strName;
    E_UINT64 m_uSize;
    bool m_bDefault;

private:
    void clear() {
        m_strName.Empty();
        m_uSize = 0;
        m_bDefault = false;
    }

} DataStream;

typedef CArray<DataStream, DataStream&> DataStreamArray;


bool
resetDate(HANDLE hFile);

bool
wipeFile(CEraserContext *context);

#endif
