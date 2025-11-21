#include<Windows.h>
#include"resource.h"


CONST CHAR* g_sz_VALUES[] = 
{ 
			"Сигаретный дым клубится в темноте",
		   "Призраки прошлого шепчутся во мгле",
		   "Одинокий вечер, тишина в квартире",
		   "Вспоминаю тех, кого больше нет в мире",
		   "Курю с призраками, разговариваю с тенями",
		   "Ошибки молодости, мы стали чужими",
		   "Курю с призраками, в дыму растворяюсь",
		   "С каждым вдохом с прошлым прощаюсь",
		   "Пепел опадает на старый паркет",
		   "В памяти оживают былые сюжеты",
		   "Тени улыбаются, жесты знакомые",
		   "Разговоры о чём-то важном и простом",
		   "Курю с призраками, разговариваю с тенями",
		   "Ошибки молодости, мы стали чужими",
		   "Курю с призраками, в дыму растворяюсь",
		   "С каждым вдохом с прошлым прощаюсь",
		   "Ночь длится бесконечно, время остановилось",
		   "С каждым призраком стало так легко и горько",
		   "Обещания, которые мы не сдержали",
		   "Мечты, что навсегда в прошлом остались",
		   "Курю с призраками, разговариваю с тенями",
		   "Ошибки молодости, мы стали чужими",
		   "Курю с призраками, в дыму растворяюсь",
		   "С каждым вдохом с прошлым прощаюсь",
		   "Рассвет приближается, призраки тают",
		   "Вместе с сигаретным дымом исчезают",
		   "Но знаю, что завтра снова наступят сумерки",
		   "И мы продолжим наши тихие беседы"
};	//Объявил глобальный массив строк

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
		if (GetDlgItem(hwnd, IDC_LIST1) != NULL)
		{
			HWND hList = GetDlgItem(hwnd, IDC_LIST1);	//Главное окно - заполнил LB начальными значениями
			for (int i = 0; i < sizeof(g_sz_VALUES) / sizeof(g_sz_VALUES[0]); i++)
			{
				SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)g_sz_VALUES[i]);
			}
		}
		else if (GetDlgItem(hwnd, IDC_EDIT) != NULL)
		{
			if (lParam != 0)	//Устанавливаю переданный текст
			{
				SetDlgItemText(hwnd, IDC_EDIT, (LPCSTR)lParam);
			}
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
				//Показываю информацию о выбранном элементе
				HWND hList = GetDlgItem(hwnd, IDC_LIST1);
				int index = (int)SendMessage(hList, LB_GETCURSEL, 0, 0);
				if (index != LB_ERR)
				{
					char selectedText[256];
					SendMessage(hList, LB_GETTEXT, index, (LPARAM)selectedText);
					char message[1024];
					wsprintf(message, "Элемент %d: %s", index, selectedText);
					MessageBox(hwnd, message, "Информация", MB_OK | MB_ICONINFORMATION);
				}
				else
				{
					MessageBox(hwnd, "Вы ничего не выбрали", "Info", MB_OK | MB_ICONINFORMATION);
				}
			}
			else if (GetDlgItem(hwnd, IDC_EDIT) != NULL)
			{
				//Окно редактирования: Сохраняю изменения
				char text[256];
				GetDlgItemText(hwnd, IDC_EDIT, text, sizeof(text));

				HWND hMain = GetParent(hwnd);
				HWND hList = GetDlgItem(hMain, IDC_LIST1);
				int index = (int)SendMessage(hList, LB_GETCURSEL, 0, 0);

				//Здесь произвожу замену элемента, удаляю старый и добавляю новый
				SendMessage(hList, LB_DELETESTRING, index, 0);
				SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)text);
				SendMessage(hList, LB_SETCURSEL, index, 0);

				EndDialog(hwnd, 0);
			}
		}
		break;
		case IDC_ADD:
		{
			//Здесь произвожу логику для кнопки ADD
			HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE);
			DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_EDIT_WINDOW), hwnd, (DLGPROC)DlgProc, 0);
		}
		break;
		case IDC_DELETE:
		{
			//Здесь произвожу логику для кнопки Delete
			HWND hList = GetDlgItem(hwnd, IDC_LIST1);
			int index = SendMessage(hList, LB_GETCURSEL, 0, 0);
			SendMessage(hList, LB_DELETESTRING, index, 0);
		}
		break;
		case IDC_LIST1:
			if (HIWORD(wParam) == LBN_DBLCLK)
			{
				//Двойной щелчок - открывает окно редактирования выбранного элемента
				HWND hList = GetDlgItem(hwnd, IDC_LIST1);
				int index = (int)SendMessage(hList, LB_GETCURSEL, 0, 0);
				if (index != LB_ERR)
				{
					char text[256];
					SendMessage(hList, LB_GETTEXT, index, (LPARAM)text);
					HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE);
					DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_EDIT_WINDOW), hwnd, (DLGPROC)DlgProc, (LPARAM)text);
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