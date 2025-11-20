#include<Windows.h>
#include"resource.h"

CONST CHAR* g_sz_VALUES[] = { "There never has been any choice for me", "Loneliness in all I have", "There never has never been any light in my life", "Darkness is all I have", "My life is pointing in one direction, I see it now", "I don't understand, I donТt understand people", "Why do they want to live, why do they want to live?", "I don't understand, I don't understand them", "Why do they run from me, why do they run from me?", 
"There never has been peace in my mind Hatred is all I have, hatred is all I feel", "There never has been any choice for me Loneliness is all I have, loneliness is all I feel" };

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(ListBox_Display), NULL, (DLGPROC)DlgProc, 0);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		HWND hList = GetDlgItem(hwnd, IDC_LIST1);
		for (int i = 0; i < sizeof(g_sz_VALUES)/sizeof(g_sz_VALUES[0]); i++)
		{
			SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)g_sz_VALUES[i]);
		}
	}
	break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			if (GetDlgItem(hwnd, IDC_LIST1) != NULL)
			{
				HWND hList = GetDlgItem(hwnd, IDC_LIST1);
				int index = (int)SendMessage(hList, LB_GETCURSEL, 0, 0);
				if (index != LB_ERR)
				{
					char selectedText[256];
					SendMessage(hList, LB_GETTEXT, index, (LPARAM)selectedText);
					char message[1024];
					wsprintf(message, "Ёлемент %d: %s", index, selectedText);
					MessageBox(hwnd, message, "»нформаци€", MB_OK | MB_ICONINFORMATION);
				}
				else
				{
					MessageBox(hwnd, "¬ы ничего не выбрали", "Info", MB_OK | MB_ICONINFORMATION);
				}
			}
			else if (GetDlgItem(hwnd, IDC_ADD) != NULL)
			{
				char text[256];
				GetDlgItemText(hwnd, IDC_ADD, text, sizeof(text));

				HWND hMain = GetParent(hwnd);
				HWND hList = GetDlgItem(hMain, IDC_LIST1);
				SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)text);
				EndDialog(hwnd, 0);
			}
		}
		break;
		case IDC_ADD:
		{
			HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE);
			DialogBoxParam(hInstance, MAKEINTRESOURCE(IDC_ADD), hwnd, (DLGPROC)DlgProc, 0);
		}
		break;
		case IDC_DELETE:
		{
			HWND hList = GetDlgItem(hwnd, IDC_LIST1);
			int index = (int)SendMessage(hList, LB_GETCURSEL, 0, 0);
			SendMessage(hList, LB_DELETESTRING, index, 0);
		}
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