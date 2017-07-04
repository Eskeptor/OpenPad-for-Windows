#pragma once
//#include <Windows.h>
#include "resource.h"
#include <tchar.h>
#include <afxwin.h>
#include <WinSock2.h>
#include <CommCtrl.h>

#define CLASS_NAME		TEXT("OpenPad for Windows")
#define CPARTS			2
#define KOREAN			0x412
#define ENGLISH			0x409
#define NULL_TEXT		TEXT("")
#define FILTER_FILE		TEXT("Text Files(*.txt)\0*.txt\0")
#define FORMAT_TEXT		TEXT(".txt")

HWND DoCreateStatusBar(HWND hwndParent, HMENU menu, HINSTANCE hinst, int cParts);
void initMenu(HWND hWnd);
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
void resizeMainHWND(HWND mainHWND, WPARAM wParam, LPARAM lParam);