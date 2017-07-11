// deviceinfo.cpp : 定义控制台应用程序的入口点。
//

// PrintDeviceInfo.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <windows.h>
#include <devguid.h>    // for GUID_DEVCLASS_CDROM etc
#include <setupapi.h>
#include <cfgmgr32.h>   // for MAX_DEVICE_ID_LEN, CM_Get_Parent and CM_Get_Device_ID
#define INITGUID
#include <tchar.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <locale.h>
#include "time.h"
#include <devpropdef.h>
#include <wchar.h>
#include <shlobj.h>
#include <io.h>
#include <direct.h>

/*
#define RtlPointerToOffset(B,P) ((ULONG)(((PCHAR)(P))-((PCHAR)(B))))
//#include "c:\WinDDK\7600.16385.1\inc\api\devpkey.h"

// include DEVPKEY_Device_BusReportedDeviceDesc from WinDDK\7600.16385.1\inc\api\devpropdef.h
#ifdef DEFINE_DEVPROPKEY
#undef DEFINE_DEVPROPKEY
#endif
#ifdef INITGUID
#define DEFINE_DEVPROPKEY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8, pid) EXTERN_C const DEVPROPKEY DECLSPEC_SELECTANY name = { { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }, pid }
#else
#define DEFINE_DEVPROPKEY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8, pid) EXTERN_C const DEVPROPKEY name
#endif // INITGUID

// include DEVPKEY_Device_BusReportedDeviceDesc from WinDDK\7600.16385.1\inc\api\devpkey.h

DEFINE_DEVPROPKEY(DEVPKEY_Device_BusReportedDeviceDesc, 0x540b947e, 0x8b40, 0x45bc, 0xa8, 0xa2, 0x6a, 0x0b, 0x89, 0x4c, 0xbd, 0xa2, 4);     // DEVPROP_TYPE_STRING
DEFINE_DEVPROPKEY(DEVPKEY_Device_ContainerId, 0x8c7ed206, 0x3f8a, 0x4827, 0xb3, 0xab, 0xae, 0x9e, 0x1f, 0xae, 0xfc, 0x6c, 2);     // DEVPROP_TYPE_GUID
DEFINE_DEVPROPKEY(DEVPKEY_Device_FriendlyName, 0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 14);    // DEVPROP_TYPE_STRING
DEFINE_DEVPROPKEY(DEVPKEY_DeviceDisplay_Category, 0x78c34fc8, 0x104a, 0x4aca, 0x9e, 0xa4, 0x52, 0x4d, 0x52, 0x99, 0x6e, 0x57, 0x5a);  // DEVPROP_TYPE_STRING_LIST
DEFINE_DEVPROPKEY(DEVPKEY_Device_LocationInfo, 0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 15);    // DEVPROP_TYPE_STRING
DEFINE_DEVPROPKEY(DEVPKEY_Device_Manufacturer, 0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 13);    // DEVPROP_TYPE_STRING
DEFINE_DEVPROPKEY(DEVPKEY_Device_SecuritySDS, 0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 26);    // DEVPROP_TYPE_SECURITY_DESCRIPTOR_STRING
*/
#define ARRAY_SIZE(arr)     (sizeof(arr)/sizeof(arr[0]))

#pragma comment (lib, "setupapi.lib")
#pragma comment (lib,"cfgmgr32.lib")
#pragma comment( lib, "shell32.lib")

WCHAR   szDocument[MAX_PATH] = { 0 };

void enumDN(DEVINST dnDevInst,FILE *fp2)
{
	union {
		PVOID buf;
		PBYTE pb;
		PWSTR sz;
	};

	ULONG cb = 0, rcb = 256;

	static volatile UCHAR guz;

	PVOID stack = alloca(guz);

	WCHAR Name[MAX_DEVICE_ID_LEN];

	CONFIGRET err;
	
	if (CM_Get_Device_ID(dnDevInst, Name, RTL_NUMBER_OF(Name), 0) == CR_SUCCESS)
	{
	//	DEVPROPTYPE PropertyType;
		ULONG Status, ulProblemNumber;

		if (CM_Get_DevInst_Status(&Status, &ulProblemNumber, dnDevInst, 0) == CR_SUCCESS)
		{
			/*
			PWSTR FriendlyName = NULL;
			do
			{
				if (cb < rcb)
				{
					rcb = cb = RtlPointerToOffset(buf = alloca(rcb - cb), stack);
				}

				if ((err = CM_Get_DevNode_PropertyW(dnDevInst, &DEVPKEY_Device_BusReportedDeviceDesc,
					&PropertyType, pb, &rcb, 0)) == CR_SUCCESS)
				{
					if (PropertyType == DEVPROP_TYPE_STRING)
					{
						FriendlyName = sz;
					}
				}

			} while (err == CR_BUFFER_SMALL);
            */
			//printf("%08x %S %S %d \n", Status, Name,FriendlyName,Status & DN_HAS_PROBLEM);
			fprintf_s(fp2, "%S\n", Name);
			if (Status&DN_HAS_PROBLEM) {
				time_t rawtime;
				struct tm timeinfo;
				char ti[1000];
				time(&rawtime);
			    localtime_s(&timeinfo,&rawtime);
				FILE *fp;
				WCHAR   eDocument[MAX_PATH] = { 0 };
				wcscpy_s(eDocument, szDocument);
				wcscat_s(eDocument, L"\\deviceinfo\\ErrorDevice.txt");
				_wfopen_s(&fp, eDocument, L"a+");
				//fopen_s(&fp, "ErrorDevice.txt", "a+");
				if (fp == 0) { printf("can't open file\n"); return; }
				fseek(fp, 0, SEEK_END);
				asctime_s(ti, &timeinfo);
				fprintf_s(fp, "%s%S %08lx\n",ti, Name ,Status);
				//fwrite(Name, wcslen(Name), 1, fp);
				
				fclose(fp);
				}
			
		}
	}

	if ((err = CM_Get_Child(&dnDevInst, dnDevInst, 0)) == CR_SUCCESS)
	{
		do
		{
			enumDN(dnDevInst,fp2);

		} while ((err = CM_Get_Sibling(&dnDevInst, dnDevInst, 0)) == CR_SUCCESS);
	}
}

void enumDN(int flag)
{
	DEVINST dnDevInst;
	FILE *fp2;
	if (flag == 2) {
		WCHAR   cDocument[MAX_PATH] = { 0 };
		wcscpy_s(cDocument, szDocument);
		wcscat_s(cDocument, L"\\deviceinfo\\CurrentDevice.txt");
		_wfopen_s(&fp2, cDocument, L"w");
		//fopen_s(&fp2, "CurrentDevice.txt", "w");
		if (fp2 == 0) { printf("can't open file\n"); return; }
		}
	else {
		WCHAR   sDocument[MAX_PATH] = { 0 };
		wcscpy_s(sDocument, szDocument);
		wcscat_s(sDocument, L"\\deviceinfo\\StandardList.txt");
		_wfopen_s(&fp2, sDocument, L"w");
		//fopen_s(&fp2, "StandardList.txt", "w");
		if (fp2 == 0) { printf("can't open file\n"); return; }
	}
	if (CM_Locate_DevInstW(&dnDevInst, NULL, 0) == CR_SUCCESS)
	{
		enumDN(dnDevInst,fp2);
	}
	fclose(fp2);
}

int compareDevice() {
	
	WCHAR strStand[1000];
	WCHAR strCurr[1000];
	FILE *fp4, *fp5;
	int status = 0;
	WCHAR   cDocument[MAX_PATH] = { 0 };
	wcscpy_s(cDocument, szDocument);
	wcscat_s(cDocument, L"\\deviceinfo\\CurrentDevice.txt");
	_wfopen_s(&fp4, cDocument, L"r");
	//fopen_s(&fp4, "CurrentDevice.txt", "r");
	if (fp4 == 0) { printf("can't open file\n"); return 0; }
	WCHAR   sDocument[MAX_PATH] = { 0 };
	wcscpy_s(sDocument, szDocument);
	wcscat_s(sDocument, L"\\deviceinfo\\StandardList.txt");
	_wfopen_s(&fp5, sDocument, L"r");
	//fopen_s(&fp5, "StandardList.txt", "r");
	if (fp5 == 0) { printf("can't open file\n"); return 0; }
	while (fgetws(strStand, 1000, fp5) != 0) {
		status = 0;
		fseek(fp4, 0, 0);
		while (fgetws(strCurr, 1000, fp4) != 0) {
			if (wcscmp(strStand, strCurr) == 0) {
				status = 1;
				break;
			}
		}
		if (status == 0) {
			time_t rawtime;
			struct tm timeinfo;
			char ti[1000];
			time(&rawtime);
			localtime_s(&timeinfo, &rawtime);
			FILE *fp6;
			WCHAR   lDocument[MAX_PATH] = { 0 };
			wcscpy_s(lDocument, szDocument);
			wcscat_s(lDocument, L"\\deviceinfo\\LostDevice.txt");
			_wfopen_s(&fp6, lDocument, L"a+");
			//fopen_s(&fp6, "LostDevice.txt", "a+");
			if (fp6 == 0) { printf("can't open file\n"); return 0; }
			fseek(fp6, 0, SEEK_END);
			asctime_s(ti, &timeinfo);
			strStand[lstrlenW(strStand) - 2] = '\0';
			fprintf_s(fp6, "%S %s", strStand, ti);
			fclose(fp6);
		}
	}
	fclose(fp4);
	fclose(fp5);
	return 1;
}

int createRefer() {
	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA DeviceInfoData;
	DWORD i;
	FILE *fp;
	WCHAR   rDocument[MAX_PATH] = { 0 };
	wcscpy_s(rDocument, szDocument);
	wcscat_s(rDocument, L"\\deviceinfo\\ReferenceList.txt");
	_wfopen_s(&fp, rDocument, L"a+");
	//fopen_s(&fp, "ReferenceList.txt", "a+");
	if (fp == 0) { printf("can't open file\n"); return 0; }
	// 得到所有设备 HDEVINFO      
	hDevInfo = SetupDiGetClassDevs(NULL, 0, 0, DIGCF_PRESENT | DIGCF_ALLCLASSES);

	if (hDevInfo == INVALID_HANDLE_VALUE)
		return 0;

	// 循环列举     
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
	{
		
		WCHAR *szDescBuf = new WCHAR[MAX_PATH];
		memset(szDescBuf, 0, MAX_PATH);
		PWSTR szDevIdBuf = new WCHAR[MAX_PATH];
		memset(szDevIdBuf, 0, MAX_PATH);

		if (!SetupDiGetDeviceInstanceIdW(hDevInfo, &DeviceInfoData, szDevIdBuf, MAX_PATH - 1, NULL))
			continue;

		if (!SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_DEVICEDESC, NULL, (LPBYTE)szDescBuf, MAX_PATH - 1, NULL))
			continue;
		fwprintf_s(fp, L"DeviceID:%s\r\nDesc:%s\r\n\r\n", szDevIdBuf, szDescBuf);
		//printf("DeviceID:%ls\r\nDesc:%ls\r\n\r\n", szDevIdBuf, szDescBuf);
	}

	//  释放     
	SetupDiDestroyDeviceInfoList(hDevInfo);
	return 1;
}

void firstTime() {

	WCHAR   path[MAX_PATH] = { 0 };
	wcscpy_s(path, szDocument);
	wcscat_s(path, L"\\deviceinfo");
	_wmkdir(path);

	enumDN(1);
	if (createRefer()) {
		printf("Initialize successfully");
	}
	else
		printf("Opps!Something wrong!");
}





void openRun() {
	//写入注册表,开机自启动 
	HKEY hKey;
	//找到系统的启动项 
	LPCTSTR lpRun = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	//打开启动项Key 
	long lRet = RegOpenKeyEx(HKEY_CURRENT_USER, lpRun, 0, KEY_WRITE, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		PWSTR pFileName= new WCHAR[MAX_PATH];
		//得到程序自身的全路径 
		DWORD dwRet = GetModuleFileNameW(NULL, pFileName, MAX_PATH);
		//添加一个子Key,并设置值 // 下面的"getip"是应用程序名字（不加后缀.exe）
		lRet = RegSetValueExW(hKey, L"deviceinfo", 0, REG_SZ, (BYTE *)pFileName, dwRet*2);
	
		//关闭注册表 
		RegCloseKey(hKey);
	}
		if (lRet != ERROR_SUCCESS)
		{
			printf("system args error,cannot set open run");
		}
	
}

void checkFirstTime() {
	FILE *fp;
	WCHAR   pDocument[MAX_PATH] = { 0 };
	wcscpy_s(pDocument,szDocument);
	wcscat_s(pDocument, L"\\deviceinfo\\StandardList.txt");
	_wfopen_s(&fp, pDocument,L"r");
	if (fp == 0) {
		firstTime();
		openRun();
	}
	else {
		fclose(fp);
		enumDN(2);
		if (compareDevice())
			printf("check complete!");
		else
			printf("check error!");
	}
}



int main() {
	WCHAR m_lpszDefaultDir[MAX_PATH];
	memset(m_lpszDefaultDir, 0, _MAX_PATH);
	LPITEMIDLIST pidl = NULL;
	SHGetSpecialFolderLocation(NULL, CSIDL_PERSONAL, &pidl);
	if (pidl   &&   SHGetPathFromIDList(pidl, szDocument))
	{
		//GetShortPathName(szDocument, m_lpszDefaultDir, _MAX_PATH);
	}
	setlocale(LC_CTYPE, "");
	checkFirstTime();
	getchar();
	return 0;
}