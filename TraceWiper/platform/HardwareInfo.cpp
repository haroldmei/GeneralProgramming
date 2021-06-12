 
//获取硬盘ID信息及网卡MAC地址的代码

/*+++
HDID.CPP
Written by Lu Lin
http://lu0.126.com
2000.11.3
---*/
#include "afx.h"
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include "afxstr.h"
#include "LocalAuth.h"
using namespace std;

#define DFP_GET_VERSION 0x00074080
#define DFP_SEND_DRIVE_COMMAND 0x0007c084
#define DFP_RECEIVE_DRIVE_DATA 0x0007c088

#pragma pack(1)
typedef struct _GETVERSIONOUTPARAMS {
    BYTE bVersion; // Binary driver version.
    BYTE bRevision; // Binary driver revision.
    BYTE bReserved; // Not used.
    BYTE bIDEDeviceMap; // Bit map of IDE devices.
    DWORD fCapabilities; // Bit mask of driver capabilities.
    DWORD dwReserved[4]; // For future use.
} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;

typedef struct _IDEREGS {
    BYTE bFeaturesReg; // Used for specifying SMART "commands".
    BYTE bSectorCountReg; // IDE sector count register
    BYTE bSectorNumberReg; // IDE sector number register
    BYTE bCylLowReg; // IDE low order cylinder value
    BYTE bCylHighReg; // IDE high order cylinder value
    BYTE bDriveHeadReg; // IDE drive/head register
    BYTE bCommandReg; // Actual IDE command.
    BYTE bReserved; // reserved for future use. Must be zero.
} IDEREGS, *PIDEREGS, *LPIDEREGS;

typedef struct _SENDCMDINPARAMS {
    DWORD cBufferSize; // Buffer size in bytes
    IDEREGS irDriveRegs; // Structure with drive register values.
    BYTE bDriveNumber; // Physical drive number to send
    // command to (0,1,2,3).
    BYTE bReserved[3]; // Reserved for future expansion.
    DWORD dwReserved[4]; // For future use.
    //BYTE bBuffer[1]; // Input buffer.
} SENDCMDINPARAMS, *PSENDCMDINPARAMS, *LPSENDCMDINPARAMS;

typedef struct _DRIVERSTATUS {
    BYTE bDriverError; // Error code from driver,
    // or 0 if no error.
    BYTE bIDEStatus; // Contents of IDE Error register.
    // Only valid when bDriverError
    // is SMART_IDE_ERROR.
    BYTE bReserved[2]; // Reserved for future expansion.
    DWORD dwReserved[2]; // Reserved for future expansion.
} DRIVERSTATUS, *PDRIVERSTATUS, *LPDRIVERSTATUS;

typedef struct _SENDCMDOUTPARAMS {
    DWORD cBufferSize; // Size of bBuffer in bytes
    DRIVERSTATUS DriverStatus; // Driver status structure.
    BYTE bBuffer[512]; // Buffer of arbitrary length
    // in which to store the data read from the drive.
} SENDCMDOUTPARAMS, *PSENDCMDOUTPARAMS, *LPSENDCMDOUTPARAMS;

typedef struct _IDSECTOR {
    USHORT wGenConfig;
    USHORT wNumCyls;
    USHORT wReserved;
    USHORT wNumHeads;
    USHORT wBytesPerTrack;
    USHORT wBytesPerSector;
    USHORT wSectorsPerTrack;
    USHORT wVendorUnique[3];
    CHAR sSerialNumber[20];
    USHORT wBufferType;
    USHORT wBufferSize;
    USHORT wECCSize;
    CHAR sFirmwareRev[8];
    CHAR sModelNumber[40];
    USHORT wMoreVendorUnique;
    USHORT wDoubleWordIO;
    USHORT wCapabilities;
    USHORT wReserved1;
    USHORT wPIOTiming;
    USHORT wDMATiming;
    USHORT wBS;
    USHORT wNumCurrentCyls;
    USHORT wNumCurrentHeads;
    USHORT wNumCurrentSectorsPerTrack;
    ULONG ulCurrentSectorCapacity;
    USHORT wMultSectorStuff;
    ULONG ulTotalAddressableSectors;
    USHORT wSingleWordDMA;
    USHORT wMultiWordDMA;
    BYTE bReserved[128];
} IDSECTOR, *PIDSECTOR;

/*+++
Global vars
---*/
GETVERSIONOUTPARAMS vers;
SENDCMDINPARAMS in;
SENDCMDOUTPARAMS out;
HANDLE h;
DWORD i;
BYTE j;


VOID ChangeByteOrder(PCHAR szString, USHORT uscStrSize)
{
    
    USHORT i;
    CHAR temp;
    
    for (i = 0; i < uscStrSize; i+=2)
    {
        temp = szString[i];
        szString[i] = szString[i+1];
        szString[i+1] = temp;
    }
}

void DetectIDE(BYTE bIDEDeviceMap){
    if (bIDEDeviceMap&1){
        if (bIDEDeviceMap&16){
            cout<<"ATAPI device is attached to primary controller, drive 0."<<endl;
        }else{
            cout<<"IDE device is attached to primary controller, drive 0."<<endl;
        }
    }
    if (bIDEDeviceMap&2){
        if (bIDEDeviceMap&32){
            cout<<"ATAPI device is attached to primary controller, drive 1."<<endl;
        }else{
            cout<<"IDE device is attached to primary controller, drive 1."<<endl;
        }
    }
    if (bIDEDeviceMap&4){
        if (bIDEDeviceMap&64){
            cout<<"ATAPI device is attached to secondary controller, drive 0."<<endl;
        }else{
            cout<<"IDE device is attached to secondary controller, drive 0."<<endl;
        }
    }
    if (bIDEDeviceMap&8){
        if (bIDEDeviceMap&128){
            cout<<"ATAPI device is attached to secondary controller, drive 1."<<endl;
        }else{
            cout<<"IDE device is attached to secondary controller, drive 1."<<endl;
        }
    }
}

static bool hdid9x(vector<CString> &driveId)
{
    char s[64]= {0};
	CString strId;

	
    ZeroMemory(&vers,sizeof(vers));
    //We start in 95/98/Me
    h=CreateFile("\\\\.\\Smartvsd",0,0,0,CREATE_NEW,0,0);
    if (!h){
        cout<<"open smartvsd.vxd failed"<<endl;
        exit(0);
    }
    
    if (!DeviceIoControl(h,DFP_GET_VERSION,0,0,&vers,sizeof(vers),&i,0)){
        cout<<"DeviceIoControl failed:DFP_GET_VERSION"<<endl;
        CloseHandle(h);
        return false;
    }
    //If IDE identify command not supported, fails
    if (!(vers.fCapabilities&1)){
        cout<<"Error: IDE identify command not supported.";
        CloseHandle(h);
        return false;
    }
    //Display IDE drive number detected
    DetectIDE(vers.bIDEDeviceMap);
    //Identify the IDE drives
    for (j=0;j<4;j++){
        PIDSECTOR phdinfo;
        char s[64];
        
        ZeroMemory(&in,sizeof(in));
        ZeroMemory(&out,sizeof(out));
        if (j&1){
            in.irDriveRegs.bDriveHeadReg=0xb0;
        }else{
            in.irDriveRegs.bDriveHeadReg=0xa0;
        }
        if (vers.fCapabilities&(16>>j)){
            
            //We don't detect a ATAPI device.
            cout<<"Drive "<<(int)(j+1)<<" is a ATAPI device, we don't detect it"<<endl;
            continue;
        }else{
            in.irDriveRegs.bCommandReg=0xec;
        }
        in.bDriveNumber=j;
        in.irDriveRegs.bSectorCountReg=1;
        in.irDriveRegs.bSectorNumberReg=1;
        in.cBufferSize=512;
        if (!DeviceIoControl(h,DFP_RECEIVE_DRIVE_DATA,&in,sizeof(in),&out,sizeof(out),&i,0)){
            cout<<"DeviceIoControl failed:DFP_RECEIVE_DRIVE_DATA"<<endl;
            CloseHandle(h);
            return false;
        }
        phdinfo=(PIDSECTOR)out.bBuffer;
        memcpy(s,phdinfo->sModelNumber,40);
        s[40]=0;
        ChangeByteOrder(s,40);
        cout<<endl<<"Module Number:"<<s<<endl;
        memcpy(s,phdinfo->sFirmwareRev,8);
        s[8]=0;
        ChangeByteOrder(s,8);
        cout<<"\tFirmware rev:"<<s<<endl;
        memcpy(s,phdinfo->sSerialNumber,20);
        s[20]=0;
        ChangeByteOrder(s,20);

		strId = CString(s);
        cout<<"\tSerial Number:"<<s<<endl;
        cout<<"\tCapacity:"<<phdinfo->ulTotalAddressableSectors/2/1024<<"M"<<endl<<endl;
    }
    
    //Close handle before quit
    CloseHandle(h);

	return true;
    
}

static bool hdidnt(vector<CString> &driveId){
    char hd[80];
    PIDSECTOR phdinfo;
    char s[64]= {0};
	CString strId;
    
    ZeroMemory(&vers,sizeof(vers));
    //We start in NT/Win2000
    for (j=0;j<4;j++){
        sprintf(hd,"\\\\.\\PhysicalDrive%d",j);
        h=CreateFile(hd,GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,0,0);
        if (!h){
            continue;
        }
        if (!DeviceIoControl(h,DFP_GET_VERSION,0,0,&vers,sizeof(vers),&i,0)){
            CloseHandle(h);
            continue;
        }
        //If IDE identify command not supported, fails
        if (!(vers.fCapabilities&1)){
            cout<<"Error: IDE identify command not supported.";
            CloseHandle(h);
            return false;
        }
        //Identify the IDE drives
        ZeroMemory(&in,sizeof(in));
        ZeroMemory(&out,sizeof(out));
        if (j&1){
            in.irDriveRegs.bDriveHeadReg=0xb0;
        }else{
            in.irDriveRegs.bDriveHeadReg=0xa0;
        }
        if (vers.fCapabilities&(16>>j)){
            //We don't detect a ATAPI device.
            cout<<"Drive "<<(int)(j+1)<<" is a ATAPI device, we don't detect it"<<endl;
            
            continue;
        }else{
            in.irDriveRegs.bCommandReg=0xec;
        }
        in.bDriveNumber=j;
        in.irDriveRegs.bSectorCountReg=1;
        in.irDriveRegs.bSectorNumberReg=1;
        in.cBufferSize=512;
        if (!DeviceIoControl(h,DFP_RECEIVE_DRIVE_DATA,&in,sizeof(in),&out,sizeof(out),&i,0)){
            cout<<"DeviceIoControl failed:DFP_RECEIVE_DRIVE_DATA"<<endl;
            CloseHandle(h);
            return false;
        }
        phdinfo=(PIDSECTOR)out.bBuffer;
        memcpy(s,phdinfo->sModelNumber,40);
        s[40]=0;
        ChangeByteOrder(s,40);
        cout<<endl<<"Module Number:"<<s<<endl;
        memcpy(s,phdinfo->sFirmwareRev,8);
        s[8]=0;
        ChangeByteOrder(s,8);
        cout<<"\tFirmware rev:"<<s<<endl;
        memcpy(s,phdinfo->sSerialNumber,20);
        s[20]=0;
        ChangeByteOrder(s,20);
		strId = CString(s);
		driveId.push_back(strId);
        cout<<"\tSerial Number:"<<s<<endl;
        cout<<"\tCapacity:"<<phdinfo->ulTotalAddressableSectors/2/1024<<"M"<<endl<<endl;
        CloseHandle(h);
    }

	return true;
}

/*int GetMacAddress(char *mac)
{
    NCB ncb; 
    
    typedef struct _ASTAT_ 
    { 
        ADAPTER_STATUS adapt; 
        NAME_BUFFER NameBuff [30]; 
    } ASTAT, * PASTAT; 
    ASTAT Adapter; 
//    ADAPTER_STATUS Adapter;
    
    LANA_ENUM lana_enum; 
    
    
    UCHAR uRetCode; 
    memset( &ncb, 0, sizeof(ncb) ); 
    memset( &lana_enum, 0, sizeof(lana_enum)); 
    
    ncb.ncb_command = NCBENUM; 
    ncb.ncb_buffer = (unsigned char *) &lana_enum; 
    ncb.ncb_length = sizeof(LANA_ENUM); 
    uRetCode = Netbios( &ncb ); 
    if( uRetCode != NRC_GOODRET ) 
        return uRetCode ; 
    
    for( int lana=0; lana<lana_enum.length; lana++ ) 
    { 
        ncb.ncb_command = NCBRESET; 
        ncb.ncb_lana_num = lana_enum.lana[lana]; 
        uRetCode = Netbios( &ncb ); 
        if( uRetCode == NRC_GOODRET ) 
            break ; 
    } 
    if( uRetCode != NRC_GOODRET ) 
        return uRetCode; 
    
    memset( &ncb, 0, sizeof(ncb) ); 
    ncb.ncb_command = NCBASTAT; 
    ncb.ncb_lana_num = lana_enum.lana[0]; 
    strcpy( (char* )ncb.ncb_callname, "*" ); 
    ncb.ncb_buffer = (unsigned char *) &Adapter; 
    ncb.ncb_length = sizeof(Adapter); 
    uRetCode = Netbios( &ncb ); 
    if( uRetCode != NRC_GOODRET ) 
        return uRetCode ; 
    sprintf(mac,"%02X-%02X-%02X-%02X-%02X-%02X", 
        Adapter.adapt.adapter_address[0], 
        Adapter.adapt.adapter_address[1], 
        Adapter.adapt.adapter_address[2], 
        Adapter.adapt.adapter_address[3], 
        Adapter.adapt.adapter_address[4], 
        Adapter.adapt.adapter_address[5] ); 
    return 0;
}
*/

static bool getHardDiskID(vector<CString> &driveId){
    char szAddr[20];
    //GetMacAddress(szAddr);
    //cout<<"\nNet adapter MAC Address:"<<szAddr<<endl<<endl;

    OSVERSIONINFO VersionInfo;
    
    ZeroMemory(&VersionInfo,sizeof(VersionInfo));
    VersionInfo.dwOSVersionInfoSize=sizeof(VersionInfo);
    GetVersionEx(&VersionInfo);
    
    switch (VersionInfo.dwPlatformId){
    case VER_PLATFORM_WIN32s:
        cout<<"Win32s is not supported by this programm."<<endl;
		return false;
    case VER_PLATFORM_WIN32_WINDOWS:
        return hdid9x(driveId);
    case VER_PLATFORM_WIN32_NT:
        return hdidnt(driveId);
	default:
		return false;
    }
    
    // TODO: Add your control notification handler code here
}

extern bool CrnGetUSBDiskID(char cDiskID, LPSTR lpPID);
static bool getUInfo(vector<CString> &driveId)
{
	CString strInfo;
	char lpPID[48] = {0};
	char uInfo[48] = {0};
	TCHAR   exeFullPath[MAX_PATH];
	GetModuleFileName(NULL,exeFullPath,MAX_PATH);
	if (CrnGetUSBDiskID(exeFullPath[0], uInfo) == true)
	{
		strInfo = CString(uInfo);
		driveId.push_back(strInfo);
	}
	else
	{
		getHardDiskID(driveId);
	}
	if (driveId.size() > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool LocalAuth()
{
	static CString astrAuth[] = {
		authStr
	};

	vector<CString> driverId;
	CString strNow = CString("");
	int size = 0;

	getUInfo(driverId);	
	if (driverId.size() <= 0)
		return false;
	size = driverId.size();
	for (int j = 0; j < driverId.size(); j++)
	{
		strNow = driverId.at(j);
		for (int i = 0; i < sizeof(astrAuth)/sizeof(astrAuth[0]); i++)
		{
			if (strNow == astrAuth[i])
				return true;
		}
	}
	
	return false;
}
