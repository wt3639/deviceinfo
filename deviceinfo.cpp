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

#define ARRAY_SIZE(arr)     (sizeof(arr)/sizeof(arr[0]))

#pragma comment (lib, "setupapi.lib")
#pragma comment (lib,"cfgmgr32.lib")

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
		DEVPROPTYPE PropertyType;
		ULONG Status, ulProblemNumber;

		if (CM_Get_DevInst_Status(&Status, &ulProblemNumber, dnDevInst, 0) == CR_SUCCESS)
		{
			PWSTR FriendlyName = NULL;
			do
			{
				if (cb < rcb)
				{
					rcb = cb = RtlPointerToOffset(buf = alloca(rcb - cb), stack);
				}

				if ((err = CM_Get_DevNode_PropertyW(dnDevInst, &DEVPKEY_Device_FriendlyName,
					&PropertyType, pb, &rcb, 0)) == CR_SUCCESS)
				{
					if (PropertyType == DEVPROP_TYPE_STRING)
					{
						FriendlyName = sz;
					}
				}

			} while (err == CR_BUFFER_SMALL);

			printf("%08x %S %S %d \n", Status, Name,FriendlyName,Status & DN_HAS_PROBLEM);
			fprintf_s(fp2, "%S\n", Name);
			if (Status&DN_HAS_PROBLEM) {
				time_t rawtime;
				struct tm timeinfo;
				char ti[1000];
				time(&rawtime);
			    localtime_s(&timeinfo,&rawtime);
				FILE *fp;
				fopen_s(&fp, "ErrorDevice.txt", "a+");
				if (fp == 0) { printf("can't open file\n"); return; }
				fseek(fp, 0, SEEK_END);
				asctime_s(ti, &timeinfo);
				fprintf_s(fp, "%S %s", Name, ti);
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

void enumDN()
{
	DEVINST dnDevInst;
	FILE *fp2;
	fopen_s(&fp2, "CurrentDevice.txt", "w");
	if (fp2 == 0) { printf("can't open file\n"); return; }

	if (CM_Locate_DevInstW(&dnDevInst, NULL, 0) == CR_SUCCESS)
	{
		enumDN(dnDevInst,fp2);
	}
	fclose(fp2);
}

int main() {
	setlocale(LC_CTYPE, "");
	enumDN();
	WCHAR strStand[1000];
	WCHAR strCurr[1000];
	FILE *fp4, *fp5;
	int status = 0;
	fopen_s(&fp4, "CurrentDevice.txt", "r");
	if (fp4 == 0) { printf("can't open file\n"); return 0; }
	fopen_s(&fp5, "StandardList.txt", "r");
	if (fp5 == 0) { printf("can't open file\n"); return 0; }
	while (fgetws(strStand,1000,fp5)!=0) {
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
			fopen_s(&fp6, "LostDevice.txt", "a+");
			if (fp6 == 0) { printf("can't open file\n"); return 0; }
			fseek(fp6, 0, SEEK_END);
			asctime_s(ti, &timeinfo);
			strStand[lstrlenW(strStand)-2] = '\0';
			fprintf_s(fp6, "%S %s", strStand, ti);
			fclose(fp6);
		}
	}
	fclose(fp4);
	fclose(fp5);
	//getchar();
	return 0;
}