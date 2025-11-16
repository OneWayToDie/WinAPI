#include<Windows.h>
#include"resource.h"

struct EditData 
{
	const char* placeholderText;	//Указатель на текст подсказки
	WNDPROC originalProcedure;		//Указатель на оригинал
	bool isShowingPlaceholder;		//Проверка плэйсхолдера
};

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wPAram, LPARAM lParam);
void AddPlaceholder(HWND hEdit, const char* placeholderText);

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
		AddPlaceholder(GetDlgItem(hwnd, IDC_EDIT_LOGIN), "Введите логин");
		AddPlaceholder(GetDlgItem(hwnd, IDC_EDIT_PASSWORD), "Введите пароль");
	}
	break;

	case WM_COMMAND:	//Обрабатывает команды с клавиатуры и мыши.
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_COPY:
		{
			CONST INT SIZE = 256;
			CHAR sz_buffer[SIZE] = {};
			HWND hEditLogin = GetDlgItem(hwnd, IDC_EDIT_LOGIN);
			HWND hEditPassword = GetDlgItem(hwnd, IDC_EDIT_PASSWORD);
			EditData* loginData = (EditData*)GetWindowLongPtr(hEditLogin, GWLP_USERDATA);
			if (loginData && !loginData->isShowingPlaceholder)
			{
				SendMessage(hEditLogin, WM_GETTEXT, SIZE, (LPARAM)sz_buffer);
				SendMessage(hEditPassword, WM_SETTEXT, 0, (LPARAM)sz_buffer);
			}
			else
			{
				SendMessage(hEditPassword, WM_SETTEXT, 0, (LPARAM)"");
			}
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
	//Изменения в коде----------------------------------------------------------------------------------------------------------------
	EditData* data = (EditData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);	//Получил данные из USERDATA
	if (!data) return DefWindowProc(hwnd, uMsg, wParam, lParam);		//Если данных нет - вызывается стандартная обработка
	switch (uMsg)	//Обрабатываю сообщения windows
	{
	case WM_SETFOCUS:	//Окно получает фокус ввода
		if (data->isShowingPlaceholder)
		{
			data->isShowingPlaceholder = false;	//Убрал подсказку
			SetWindowTextA(hwnd, "");	//Очистил текст
		}
		break;
	case WM_KILLFOCUS:	//Окно теряет фокус ввода
		if (GetWindowTextLength(hwnd) == 0 && !data->isShowingPlaceholder)	//Если оно пустое и подсказка не отображается
		{
			data->isShowingPlaceholder = true;	//Показываю подсказку опять
			SetWindowTextA(hwnd, data->placeholderText);	//Устанавливаю текст подсказки
		}
		break;
	}
	return CallWindowProc(data->originalProcedure, hwnd, uMsg, wParam, lParam); //Для всех остальных сообщений вызываю оригинальную процедуру
}
void AddPlaceholder(HWND hEdit, const char* placeholderText)
{
	EditData* data = new EditData;				//Создал новую структуру данных для хранения состояния placeholder'a
	data->placeholderText = placeholderText;	//Сохранил текст
	data->isShowingPlaceholder = true;			//Показал наличие этого текста
	data->originalProcedure = (WNDPROC)GetWindowLongPtr(hEdit, GWLP_WNDPROC);//сохранил оригинал Edit control'a	SetWindowLongPtr(hEdit, GWLP_USERDATA, (LONG_PTR)data);//
	SetWindowLongPtr(hEdit, GWLP_USERDATA, (LONG_PTR)data);//Сохранил указатель на структуру в USERDATA окна
	SetWindowLongPtr(hEdit, GWLP_WNDPROC, (LONG_PTR)DlgProc);//Заменил оконную процедуру на новую
	SetWindowText(hEdit, placeholderText);		//Установил текст подсказки в поле ввода
}