#include "head.h"

HWND statusbar;
HWND editBox;
int prevX, prevY;
HINSTANCE langFile;
LANGID curLangID = 0;

OPENFILENAME fileDialog;
TCHAR openFileName[MAX_PATH] = { 0, };
wchar_t* editText;

enum _DataChanged
{
	CHANGED = 0,
	NOT_CHANGED = 1
}DataChanged;

// TODO: 할일

INT APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE, TCHAR *pszLine, INT nShow)
{
	WNDCLASS wndClass;
	HWND hWnd;
	MSG message;
	HACCEL hAccel; // 단축키

	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDC_ICON);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = CLASS_NAME;
	wndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wndClass.style = CS_OWNDC;

	// 단축키 연결
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	RegisterClass(&wndClass);

	hWnd = CreateWindow(CLASS_NAME, CLASS_NAME, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	statusbar = DoCreateStatusBar(hWnd, GetMenu(hWnd), hInstance, CPARTS);
	editBox = CreateWindowEx(0, TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | WS_HSCROLL,
		0, 0, 0, 0, hWnd, (HMENU)ID_EDIT, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
	
	ShowWindow(hWnd, SW_SHOW);
	ShowWindow(editBox, SW_SHOW);
	ShowWindow(statusbar, SW_HIDE);

	while (GetMessage(&message, NULL, NULL, NULL))
	{
		if (!TranslateAccelerator(hWnd, hAccel, &message))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HMENU menu;
	MENUITEMINFO itemInfo;

	HANDLE file;

	switch (iMsg)
	{
	case WM_CREATE:
		initMenu(hWnd);
		DataChanged = NOT_CHANGED;
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_MENU_FILE_NEW:
			if (DataChanged == CHANGED)
			{
				// TODO : 메시지박스에 넣을 문구 지역화하기
				int select = MessageBox(hWnd, TEXT(""), TEXT(""), MB_YESNOCANCEL);
				if (select == IDCANCEL)
				{
					break;
				}			
				else if (select == IDYES)
				{
					free(editText);
				}
			}
			openFileName[0] = '\0';
			SetWindowText(editBox, NULL_TEXT);
			break;
		case ID_MENU_FILE_OPEN:
			if (DataChanged == NOT_CHANGED)
			{
				memset(&fileDialog, 0, sizeof(OPENFILENAME));
				fileDialog.lStructSize = sizeof(OPENFILENAME);
				fileDialog.hwndOwner = hWnd;
				fileDialog.lpstrFilter = FILTER_FILE;
				fileDialog.lpstrFile = openFileName;
				fileDialog.nMaxFile = MAX_PATH;
				// TODO : ANSI와 UTF-8 구분하기
				if (GetOpenFileName(&fileDialog))
				{
					file = CreateFile(openFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					int fileSize = GetFileSize(file, NULL);
					DWORD dwRead;
					editText = (wchar_t*)malloc(sizeof(wchar_t) * fileSize);
					ReadFile(file, editText, _tcslen(editText), &dwRead, NULL);
					SetWindowText(editBox, editText);
					CloseHandle(file);
				}
			}
			else
			{
				int select = MessageBox(hWnd, TEXT(""), TEXT(""), MB_YESNOCANCEL);
				if (select == IDCANCEL)
				{
					break;
				}
				else if (select == IDYES)
				{
					free(editText);
				}
			}
			
			break;
		case ID_MENU_FILE_SAVE:
			memset(&fileDialog, 0, sizeof(OPENFILENAME));
			fileDialog.lStructSize = sizeof(OPENFILENAME);
			fileDialog.hwndOwner = hWnd;
			fileDialog.lpstrFilter = FILTER_FILE;
			fileDialog.lpstrFile = openFileName;
			fileDialog.nMaxFile = MAX_PATH;
			if (GetSaveFileName(&fileDialog) != 0)
			{
				TCHAR* save = (TCHAR*)malloc(sizeof(TCHAR) * _tcslen(openFileName) + 4);
				_tcscpy(save, openFileName);
				_tcscat(save, FORMAT_TEXT);
				file = CreateFile(save, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
				int length = (GetWindowTextLength(editBox) + 1) * 2;
				DWORD dwWritten;
				editText = (wchar_t*)malloc(sizeof(wchar_t) * length);
				GetWindowText(editBox, editText, length);
				WriteFile(file, editText, length, &dwWritten, NULL);
				CloseHandle(file);
			}
			break;
		case ID_MENU_VIEW_STATUS:
			menu = GetMenu(hWnd);
			itemInfo = { sizeof(MENUITEMINFO) };
			itemInfo.fMask = MIIM_STATE;
			GetMenuItemInfo(menu, ID_MENU_VIEW_STATUS, FALSE, &itemInfo);
			if (itemInfo.fState == MFS_UNCHECKED)
			{
				itemInfo.fState = MFS_CHECKED;
				ShowWindow(statusbar, SW_SHOW);
			}
			else
			{
				itemInfo.fState = MFS_UNCHECKED;
				ShowWindow(statusbar, SW_HIDE);
			}
			SetMenuItemInfo(menu, ID_MENU_VIEW_STATUS, FALSE, &itemInfo);
			resizeMainHWND(hWnd, wParam, lParam);
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case ID_MENU_FORMAT_AUTOLINE:
			menu = GetMenu(hWnd);
			itemInfo = { sizeof(MENUITEMINFO) };
			itemInfo.fMask = MIIM_STATE;
			GetMenuItemInfo(menu, ID_MENU_FORMAT_AUTOLINE, FALSE, &itemInfo);
			if (itemInfo.fState == MFS_UNCHECKED)
			{
				itemInfo.fState = MFS_CHECKED;
			}
			else
			{
				itemInfo.fState = MFS_UNCHECKED;
			}
			SetMenuItemInfo(menu, ID_MENU_FORMAT_AUTOLINE, FALSE, &itemInfo);
			break;
		}
		return 0;
	case WM_SIZE:
		resizeMainHWND(hWnd, wParam, lParam);
		return 0;
	case WM_DESTROY:
		FreeLibrary(langFile);
		free(editText);
		PostQuitMessage(0);
	}
	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

void initMenu(HWND hWnd)
{
	HMENU menu = GetMenu(hWnd);
	MENUITEMINFO itemInfo = { sizeof(MENUITEMINFO) };
	itemInfo.fMask = MIIM_STATE | MIIM_TYPE | MIIM_DATA;
	TCHAR menuText[MAX_PATH] = { 0, };

	curLangID = GetUserDefaultLangID();

	switch (curLangID)
	{
	case KOREAN:
		langFile = LoadLibrary(_T("Openpad_Localization_kr.dll"));
		//langFile = LoadLibrary(_T("Openpad_Localization_en.dll"));
		break;
	case ENGLISH:
		langFile = LoadLibrary(_T("Openpad_Localization_en.dll"));
		//langFile = LoadLibrary(_T("Openpad_Localization_kr.dll"));
		break;
	default:
		break;
	}

	for (int i = ID_MENU_FILE_NEW; i <= ID_MENU_HELP_INFO; i++)
	{
		GetMenuItemInfo(menu, i, FALSE, &itemInfo);
		LoadString(langFile, i, menuText, MAX_PATH);
		itemInfo.dwTypeData = menuText;
		if (i >= ID_MENU_EDIT_CANCEL && i <= ID_MENU_EDIT_FIND)
		{
			itemInfo.fState = MFS_DISABLED;
		}
		SetMenuItemInfo(menu, i, FALSE, &itemInfo);
	}

	CMenu* pMenu = CMenu::FromHandle(menu);
	int cnt = pMenu->GetMenuItemCount();
	UINT uID;
	CString ss;
	for (int i = ID_MENU_FILE; i <= ID_MENU_HELP; i++)
	{
		uID = pMenu->GetMenuItemID(i - ID_MENU_FILE);
		if (uID == 0)
		{
			continue;
		}
		pMenu->GetMenuString(i - ID_MENU_FILE, ss, MF_BYPOSITION);
		if (uID == (UINT)-1)
		{
			LoadString(langFile, i, menuText, MAX_PATH);
			ss = menuText;
			pMenu->ModifyMenu(i - ID_MENU_FILE, MF_BYPOSITION, 0, ss);
		}
	}
}

void resizeMainHWND(HWND mainHWND, WPARAM wParam, LPARAM lParam)
{
	// 에딧박스와 하단 상태바 리사이즈 함수

	HMENU menu = GetMenu(mainHWND);
	int curX, curY, gapY;
	RECT rect;
	MENUITEMINFO itemInfo = { sizeof(MENUITEMINFO) };

	itemInfo.fMask = MIIM_STATE;
	GetMenuItemInfo(menu, ID_MENU_VIEW_STATUS, FALSE, &itemInfo);
	curX = LOWORD(lParam);
	curY = HIWORD(lParam);

	if (curX == 0 && curY == 0)
	{
		curX = prevX;
		curY = prevY;
	}

	if (itemInfo.fState == MFS_CHECKED)
	{
		// 상태바 리사이즈
		GetWindowRect(mainHWND, &rect);
		gapY = rect.bottom - rect.top;
		//MoveWindow(statusbar, 0, curY - gapY, curX, gapY, TRUE);
		MoveWindow(statusbar, 0, curY - gapY, curX, curY, TRUE);

		// 에딧컨트롤 리사이즈
		GetWindowRect(statusbar, &rect);
		gapY = rect.bottom - rect.top;
		MoveWindow(editBox, 0, 0, curX, curY - gapY, TRUE);
	}
	else
	{
		// 에딧컨트롤 리사이즈
		MoveWindow(editBox, 0, 0, curX, curY, TRUE);
	}
	prevX = curX;
	prevY = curY;
}