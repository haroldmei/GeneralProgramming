#include <windows.h>
#include <setupapi.h>
#include <stdio.h>
#include <objbase.h>
#include "atlstr.h"
#ifndef __BORLANDC__
#pragma comment(lib, "setupapi.lib")
#endif

void CrnReplaceString(LPSTR lpBuf, char c1, char c2)
{
    int nLen = strlen(lpBuf);
    LPSTR lpTemp = new char[nLen];
    memset(lpTemp, 0, nLen);
    int j=0;
    for(int i=0; i < nLen; i++)
    {
        if(lpBuf[i] == c1)
        {
            if(c2 != 0x00)
            {
                lpTemp[j] = c2;
                j++;
            }
        }
        else
        {
            lpTemp[j] = lpBuf[i];
            j++;
        }
    }
    //
    memset(lpBuf, 0, nLen);
    strncpy(lpBuf, lpTemp, nLen);
    delete []lpTemp;
}
//---------------------------------------------------------------------------
// 获取指定盘符的序列号(针对USB优盘有效)
// char cDiskID: 指定盘符，如 'L'
// LPSTR lpPID : 序列号存放的缓冲，如char szPID[32]
// by ccrun(老妖) info@ccrun.com
//---------------------------------------------------------------------------
bool CrnGetUSBDiskID(char cDiskID, LPSTR lpPID)
{
    char szDrv[4];
    sprintf(szDrv, "%c:\\", cDiskID);
	UINT val = GetDriveType(szDrv);
	
    if(val != DRIVE_REMOVABLE)
    {	
        MessageBox(0, "软件使用权限验证失败,请联系厂家!", "程序退出", MB_OK | MB_ICONWARNING);
        return false;
    }

    char lpRegPath[512] = { 0 };
    char lpRegValue[256] = { 0 };
    sprintf(lpRegPath, "SYSTEM\\MountedDevices");
    sprintf(lpRegValue, "\\DosDevices\\%c:", cDiskID);
    //
    DWORD dwDataSize(0);
    DWORD dwRegType(REG_BINARY);
    LPBYTE lpRegBinData(NULL);
    LPSTR lpUSBKeyData(NULL);

    // 查询注册表中映射驱动器的设备信息
    HKEY hKey;
    long lRet = ::RegOpenKeyEx(
            HKEY_LOCAL_MACHINE, // root key
            lpRegPath, // 要访问的键的位置
            0,         //
            KEY_READ,  // 以查询的方式访问注册表
            &hKey);    // hKEY保存此函数所打开的键的句柄
    if(lRet != ERROR_SUCCESS)
        return false;
    else
    {
        lRet = ::RegQueryValueEx(hKey, // 所打开的键的句柄
                lpRegValue,    // 要查询的键值名
                NULL,
                &dwRegType,    // 查询数据的类型
                lpRegBinData,  // 保存所查询的数据
                &dwDataSize);  // 预设置的数据长度
        if(lRet != ERROR_SUCCESS)
        {
            ::RegCloseKey(hKey);
            return false;
        }
        else
        {
            lpRegBinData = new BYTE[dwDataSize];
            lpUSBKeyData = new char[dwDataSize];
            memset(lpUSBKeyData, 0, dwDataSize);
            lRet = ::RegQueryValueEx(hKey,
                    lpRegValue,
                    NULL,
                    &dwRegType,
                    lpRegBinData,
                    &dwDataSize);
            if(lRet != ERROR_SUCCESS)
            {
                delete []lpRegBinData;
                delete []lpUSBKeyData;
                ::RegCloseKey(hKey);
                return false;
            }
        }
    }
    ::RegCloseKey(hKey);
    // 过滤二进制串中的无用信息(将0x0字符去除)
    int j = 0;
    for(DWORD i=0; i<dwDataSize; i++)
    {
        if(lpRegBinData[i] != 0x0)
        {
            lpUSBKeyData[j] = lpRegBinData[i];
            j++;
        }
    }
    delete []lpRegBinData;

    // 截取lpUSBKeyData中的有用信息, 例: 7&100a16f&0
    // \??\STORAGE#RemovableMedia#7&100a16f&0&RM#{53f5630d-b6bf-11d0-94f2-00a0c91efb8b}
    // 63 63 72 75 6E 2E 63 6F 6D
    LPSTR lpPos1 = strstr(lpUSBKeyData, "#RemovableMedia#") + 16;
    LPSTR lpPos2 = strstr(lpUSBKeyData, "RM");
    strncpy(lpUSBKeyData, lpPos1, int(lpPos2) - int(lpPos1));
    lpUSBKeyData[int(lpPos2) - int(lpPos1) - 1] = 0x0;
    strcpy(lpUSBKeyData, strupr(lpUSBKeyData));

    // Disk Device(磁盘设备)的GUID
    GUID guidUSB;
    CLSIDFromString(L"{53f56307-b6bf-11d0-94f2-00a0c91efb8b}", &guidUSB);

    //
    HDEVINFO hUSB = SetupDiGetClassDevs(
            &guidUSB, NULL, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if(hUSB == INVALID_HANDLE_VALUE)
    {
        delete []lpUSBKeyData;
        return false;
    }
    //
    int nDevIndex = 0;
    bool bSuccess;
    SP_DEVINFO_DATA DevData;
    SP_DEVICE_INTERFACE_DATA DevIntData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA lpDevIntDetailData;
    DWORD dwBytesReturned;

    // 遍历磁盘设备
    do
    {
        DevIntData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        bSuccess = SetupDiEnumDeviceInterfaces(hUSB, NULL, &guidUSB,
                nDevIndex, &DevIntData) > 0;

        if(bSuccess)
        {
            // 获取接口详细信息
            DevData.cbSize = sizeof(SP_DEVINFO_DATA);
            dwBytesReturned = 0;
            SetupDiGetDeviceInterfaceDetailA(hUSB, &DevIntData,
                    NULL, 0, &dwBytesReturned, &DevData);
            if(dwBytesReturned != 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                lpDevIntDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)
                        GlobalAlloc(GMEM_FIXED, dwBytesReturned);
                lpDevIntDetailData->cbSize =
                        sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
                if(SetupDiGetDeviceInterfaceDetailA(hUSB, &DevIntData,
                        lpDevIntDetailData, dwBytesReturned, &dwBytesReturned, &DevData))
                {
                    // 取得设备接口详细信息并根据转化后的路径在注册表中查询
                    LPSTR lpPathTemp = new char[strlen(lpDevIntDetailData->DevicePath) + 256];
                    strcpy(lpRegPath, "SYSTEM\\CurrentControlSet\\Enum\\");
                    strcpy(lpPathTemp, lpDevIntDetailData->DevicePath);
                    lpPos1 = LPSTR(int(lpPathTemp) + 4);
                    lpPos2 = LPSTR(int(strstr(lpPathTemp, "{")) - 1);
                    strncpy(lpPathTemp, lpPos1, int(lpPos2) - int(lpPos1));
                    lpPathTemp[int(lpPos2) - int(lpPos1)] = 0x0;
                    CrnReplaceString(lpPathTemp, '#', '\\');

                    strcat(lpRegPath, lpPathTemp);
                    //
                    if(RegOpenKeyEx(
                            HKEY_LOCAL_MACHINE,
                            lpRegPath,
                            0,
                            KEY_READ,
                            &hKey) == ERROR_SUCCESS)
                    {
                        dwRegType = REG_SZ;
                        LPSTR lpRegSzData = NULL;
                        dwDataSize = 0;
                        lRet = ::RegQueryValueEx(hKey,
                                "ParentIdPrefix",
                                NULL,
                                &dwRegType,
                                (LPBYTE)lpRegSzData,
                                &dwDataSize);
                        if(lRet == ERROR_SUCCESS)
                        {
                            lpRegSzData = new char[dwDataSize];
                            lRet = ::RegQueryValueEx(hKey,
                                    "ParentIdPrefix",
                                    NULL,
                                    &dwRegType,
                                    (LPBYTE)lpRegSzData,
                                    &dwDataSize);
                            if(lRet == ERROR_SUCCESS)
                            {
                                strcpy(lpRegSzData, strupr(lpRegSzData));
                                if(!strcmp(lpUSBKeyData, lpRegSzData))
                                {
                                    // 经比对，找到要查询的磁盘设备
                                    strcpy(lpPathTemp, LPSTR(int(strstr(
                                        lpDevIntDetailData->DevicePath, "#")) + 1));
                                    lpPos1 = LPSTR(int(strstr(lpPathTemp, "#")) + 1);
                                    lpPos2 = LPSTR(int(strstr(lpPathTemp, "#{")));
                                    strncpy(lpPathTemp, lpPos1, int(lpPos2) - int(lpPos1));
                                    lpPathTemp[int(lpPos2) - int(lpPos1)] = 0x0;

                                    // 获取最终的磁盘序列号
                                    CrnReplaceString(lpPathTemp, '&', 0x00);
                                    if(lpPID)
                                        strncpy(lpPID, strupr(lpPathTemp), 32);
                                    //
                                    delete []lpRegSzData;
                                    delete []lpPathTemp;
                                    GlobalFree(lpDevIntDetailData);
                                    ::RegCloseKey(hKey);
                                    break;
                                }
                            }
                            delete []lpRegSzData;
                        }
                        ::RegCloseKey(hKey);
                    }
                    delete []lpPathTemp;
                    nDevIndex++;
                }
                GlobalFree(lpDevIntDetailData);
            }
        }
    }while(bSuccess);

    SetupDiDestroyDeviceInfoList(hUSB);

    delete []lpUSBKeyData;

    return true;
}