// ARX.cpp : 定義主控台應用程式的進入點。
//
#include "stdafx.h"
#include "stdio.h"
#include "conio.h"
#include "time.h"
#include "iostream"
#include "stdlib.h"
#include "windows.h"
#include "string"
#pragma comment(lib, "LogitechLEDLib.lib")
#pragma comment(lib, "LogitechGArxControlLib.lib")
#include "LogitechLEDLib.h"
#include "LogitechGArxControlLib.h"
char keycode[256];

//############################ REPLACE THIS WITH YOUR PATH NAME!!! #########################
#define dir__ L"C:\\Users\\WEI\\Desktop\\Logi_Hack\\paint\\ARX"
//#define dir__ L""

const wchar_t *programTitleW = L"logitech-drawkb";
const wchar_t dir[] = dir__;
int dirlen = lstrlenW(dir);

typedef struct arxAppCallbackMessage
{
	unsigned __int32 eventType;
	unsigned __int32 eventValue;
	wchar_t eventArg[128]; //Max length for this callback param (see documentation for more details)
}arxAppCallbackMessage;


#define WM_ARXAPP_SDK_CALLBACK WM_APP + 1

class GGGG {
public:
	int d[3];
	GGGG() {
		d[0] = d[1] = d[2] = 100;
		d[2] = 80;
	}
	void set(int r, int g, int b)
	{
		d[0] = r, d[1] = g, d[2] = b;
	}
	std::string toStr()const
	{
		std::string s[3];
		char buf[16];
		for (int x = 0; x < 3; x++) {
			sprintf_s(buf,"%d", d[x]*256/100);
			s[x] = buf;
		}
		std::string rtv = "";
		rtv += "rgba(";
		rtv += s[0];
		rtv += ",";
		rtv += s[1];
		rtv += ",";
		rtv += s[2];
		rtv += ",1)";
		return rtv;
	}
}a[9],cur;

void updateIndex(wchar_t idx)
{
	wchar_t i[] = dir__ L"0.html"; i[dirlen] = idx;
	if (!LogiArxSetIndex(i))
	{
		printf("Could not set index2", true);
	}
}
wchar_t last = 0;
void onCallback(int eventType, int eventValue, wchar_t * eventArg, void *context)
{
	if (!(eventType == 4 && eventValue == 0)) return;
//Using PostMessage to notify main thread of callback event


	//std::cout << parameter->eventType << std::endl;
	//std::cout << parameter->eventValue << std::endl;
	//std::cout << reinterpret_cast<wchar_t*>  << std::endl;

	//a[eventArg[3]]
	//std::cout << eventType << std::endl;
	//std::cout << eventValue << std::endl;
	//wprintf(L"User Tapped on tag with id :%ls\n", (eventArg));
	int len = 0;
	if(eventArg != NULL)while (eventArg[len] != 0)len++;
	
	if (len>3&& last!=eventArg[3] && eventType == 4 && eventValue == 0 && eventArg[3] < '9' && eventArg[3] >= '0')
	{
		last = eventArg[3];
		cur = a[eventArg[3]-'0'];
		updateIndex(eventArg[3]);
	}

	
	Sleep(20);
	arxAppCallbackMessage *parameter = new arxAppCallbackMessage;
	parameter->eventType = eventType;
	parameter->eventValue = eventValue;
	wcsncpy_s(parameter->eventArg, eventArg, _TRUNCATE);
	HWND main_hwnd = (HWND)context;
	::PostMessage(main_hwnd, WM_ARXAPP_SDK_CALLBACK, reinterpret_cast<WPARAM> (parameter), static_cast<LPARAM>(0));
}

DWORD WINAPI blink(void* data) {
	using namespace std;
	srand(time(NULL));
	while (1) {
		for (short i = 0; i < 0xFF; i++) {
			short s = GetKeyState(MapVirtualKey(i, MAPVK_VSC_TO_VK_EX));
			if (s & 0x8000) {
				LogiLedSetLightingForKeyWithKeyName((LogiLed::KeyName)i, cur.d[0], 
					cur.d[1], cur.d[2]);
			}
		}
	}

	return 0;
}

logiArxCbContext callbackStruct;
int main()
{
	// check if running twice same time
	if (FindWindow(NULL, programTitleW)) {
		wprintf(L"%s has been running\n", programTitleW);
		//system("pause");
		return 0; 
	}
	SetConsoleTitle(programTitleW);
	// init color choices
	a[0].set(100, 0, 0);
	a[1].set(0, 100, 0);
	a[2].set(0, 0, 100);
	a[3].set(100, 80, 0);
	a[4].set(0, 50, 50);
	a[5].set(50, 0, 50);
	a[6].set(100, 40, 15);
	a[7].set(100, 100, 80);
	a[8].set(0, 0, 0);
	// init kb led
	if (!LogiLedInit())
	{
		printf("Error initialization\n");
	}
	LogiLedSetLighting(0, 0, 0);
	// init Arx
	callbackStruct.arxContext =  NULL;
	callbackStruct.arxCallBack = (logiArxCb)onCallback;
	if (!LogiArxInit(L"arxsdk.sample.mfcsample", L"HI Apple", &callbackStruct))
	{
		printf("Error initialization1\n");
	}
	else
	{
		printf("CONNECTED\n");
	}
	// init arx - html files
	for (char x = '0'; x <= '8'; x++)
	{
		wchar_t ffsFC[] = dir__ L"0.html"; ffsFC[dirlen] = x;
		wchar_t ffsFN[] = dir__ L"0.html"; ffsFN[dirlen] = x;
		if (!LogiArxAddFileAs(ffsFC, ffsFN, L"text/html"))
		{ 
			printf("Error initialization2\n");
		}
	
	}
	// init arx - set index
	updateIndex(L'7');
	cur = a[7];
	// keys states polling
	HANDLE thread = CreateThread(NULL, 0, blink, NULL, 0, NULL);
	// exit when esc keydown
	while (!(GetKeyState(27)&0x8000)) Sleep(1);
	Sleep(111);
	LogiArxShutdown();

    return 0;
}

