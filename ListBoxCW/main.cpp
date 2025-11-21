#include<Windows.h>
#include"resource.h"

CONST CHAR* g_sz_VALUES[] = { "This", "is", "my", "first", "List", "Box" };

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
			HWND hListBox = GetDlgItem(hwnd, IDC_LIST1);
			for (int i = 0; i < sizeof(g_sz_VALUES) / sizeof(g_sz_VALUES[0]); i++)
				SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)g_sz_VALUES[i]);
		}
		break;
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_BUTTON_ADD:
			{

			}
			break;
			case IDC_BUTTON_DELETE:
			{
				HWND hListBox = GetDlgItem(hwnd, IDC_LIST1);
				INT i = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
				SendMessage(hListBox, LB_DELETESTRING, i, 0);
			}
			break;
			case IDOK:
				break;
			case IDCANCEL:
				EndDialog(hwnd, 0);
			}
		}
		break;
		case WM_CLOSE:
			EndDialog(hwnd, 0);
	}
	return FALSE;
}