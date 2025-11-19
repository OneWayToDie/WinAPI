//#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<cstdio>
#include"resource.h"

CONST CHAR* g_sz_VALUES[] = { "This", "is", "my", "first", "Combo", "Box" };

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc, 0);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		HWND hCombo = GetDlgItem(hwnd, IDC_COMBO1);
		for (int i = 0; i < sizeof(g_sz_VALUES)/sizeof(g_sz_VALUES[0]); i++)
		{
			SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)g_sz_VALUES[i]);
		}
		//SendMessage(GetDlgItem(hwnd, IDC_COMBO1), CB_SETCURSEL, 0, 0);
		//https://learn.microsoft.com/en-us/windows/win32/controls/cb-setcursel
	}
	break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			//HWND hCombo = GetDlgItem(hwnd, IDC_COMBO1);	//Находим список
			//int index = (int)SendMessage(hCombo, CB_GETCURSEL, 0, 0); //Получаем индекс выбранного элемента в combobox
			////https://learn.microsoft.com/ru-ru/windows/win32/controls/cb-getcursel
			//char selectedText[100];//Создаём буфер для хранения текста выбранного элемента
			//GetWindowTextA(hCombo, selectedText, sizeof(selectedText));//Получаем текст выбранного элемента
			//char message[200];//Создаём буфер для сообщения
			//wsprintf(message, "Вы выбрали элемент № %d со значением '%s'", index, selectedText);//Форматируем сообщение
			////https://cplusplus.com/forum/windows/92842
			//MessageBox(hwnd, message, "Информация о выборе", MB_OK | MB_ICONINFORMATION);//Выводим его
			CONST INT SIZE = 256;
			CHAR sz_buffer[SIZE]{}; //sz - String Zero(NULL Terminated Line)
			CHAR sz_message[SIZE]{}; //sz - String Zero(NULL Terminated Line)
			HWND hCombo = GetDlgItem(hwnd, IDC_COMBO1);
			INT i = SendMessage(hCombo, CB_GETCURSEL, 0, 0); //GetCurrentSelection - получить текущий выбранный элемент
			if (i != CB_ERR)
			{
				SendMessage(hCombo, CB_GETLBTEXT, i, (LPARAM)sz_buffer);
				wsprintf(sz_message, "Вы выбрали элемент №%i со значением \"%s\"", i, sz_buffer);
				MessageBox(hwnd, sz_message, "Info", MB_OK | MB_ICONINFORMATION);
			}
			else
			{
				MessageBox(hwnd, "Вы ничего не выбрали", "Info", MB_OK | MB_ICONINFORMATION);
			}
		}
			break;
		case IDCANCEL:
			EndDialog(hwnd, 0);
			break;
		}
	}
	break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
	}
	return FALSE;
}




//В проекте комбоБокс при нажатии на кнопку ok - должно появляться окно сообщения 
//Отображается номер и содержимое пункта выпадающего списка. Например вы выбрали элемент номер 0,
//Элемент this