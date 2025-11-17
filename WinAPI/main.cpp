#include<Windows.h>
#include"resource.h"

//struct EditData 
//{
//	const char* placeholderText;	//Указатель на текст подсказки
//	WNDPROC originalProcedure;		//Сохраняю оригинал
//	bool isShowingPlaceholder;		//Проверка плэйсхолдера
//};

CONST CHAR g_sz_INVITE[] = "Введите имя пользователя";

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wPAram, LPARAM lParam);
//void AddPlaceholder(HWND hEdit, const char* placeholderText);



INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	//MessageBox
	//(
	//	NULL, 
	//	"Hello WinAPI!", 
	//	"MessageBox", 
	//	MB_YESNOCANCEL | MB_ICONINFORMATION | MB_HELP | MB_DEFBUTTON3 | MB_SYSTEMMODAL
	//);	// MB_ - MessageBox;  

	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc, 0);

	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:	//Выполняется 1 раз, при запуске окна.
	{
		HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		SendMessage(hwnd, WM_SETICON, 0, (LPARAM)hIcon);
		//SetFocus(GetDlgItem(hwnd, IDC_EDIT_LOGIN));
		//AddPlaceholder(GetDlgItem(hwnd, IDC_EDIT_LOGIN), "Введите логин");
		//AddPlaceholder(GetDlgItem(hwnd, IDC_EDIT_PASSWORD), "Введите пароль");
		SendMessage(GetDlgItem(hwnd, IDC_EDIT_LOGIN), WM_SETTEXT, 0, (LPARAM)g_sz_INVITE);
	}
	break;

	case WM_COMMAND:	//Обрабатывает команды с клавиатуры и мыши.
		switch (LOWORD(wParam))
		{
		case IDC_EDIT_LOGIN:
		{
			CONST INT SIZE = 256;
			HWND hEditLogin = GetDlgItem(hwnd, IDC_EDIT_LOGIN);
			CHAR sz_buffer[SIZE] = "";
			if (HIWORD(wParam) == EN_SETFOCUS && strcmp(sz_buffer, g_sz_INVITE)==0);
			{
				SendMessage(hEditLogin, WM_SETTEXT, 0, (LPARAM)"");
			}
			if (HIWORD(wParam) == EN_KILLFOCUS && strcmp(sz_buffer, "") == 0)
			SendMessage(hEditLogin, WM_SETTEXT, 0, (LPARAM)g_sz_INVITE);
		}
		case IDC_BUTTON_COPY:
		{
			CONST INT SIZE = 256;
			CHAR sz_buffer[SIZE] = {};
			HWND hEditLogin = GetDlgItem(hwnd, IDC_EDIT_LOGIN);
			HWND hEditPassword = GetDlgItem(hwnd, IDC_EDIT_PASSWORD);
			SendMessage(hEditLogin, WM_GETTEXT, SIZE, (LPARAM)sz_buffer);
			SendMessage(hEditPassword, WM_SETTEXT, 0, (LPARAM)sz_buffer);
	
		}
		break;
		case IDOK:
			MessageBox(NULL, "Была нажата кнопка 'OK'", "Info", MB_OK | MB_ICONINFORMATION);
			break;
		case IDCANCEL:
			EndDialog(hwnd, 0);
			break;
		}
		break;
	case WM_CLOSE:		//Выполняется при нажатии кнопки 'x'.
		EndDialog(hwnd, 0);
	}
	return FALSE;
}
