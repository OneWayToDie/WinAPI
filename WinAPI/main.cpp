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
LRESULT CALLBACK Placeholder(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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
		//SendMessage(GetDlgItem(hwnd, IDC_EDIT_LOGIN), WM_SETFOCUS, (WPARAM)GetDlgItem(hwnd,IDOK), NULL);
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
	return FALSE;
}

void AddPlaceholder(HWND hEdit, const char* placeholderText)
{
	EditData* data = new EditData;				//Создал новую структуру данных для хранения состояния placeholder'a
	data->placeholderText = placeholderText;	//Сохранил текст
	data->isShowingPlaceholder = true;			//Показал наличие этого текста
	data->originalProcedure = (WNDPROC)GetWindowLongPtr(hEdit, GWLP_WNDPROC);//сохранил оригинал Edit control'a	SetWindowLongPtr(hEdit, GWLP_USERDATA, (LONG_PTR)data);//
	SetWindowLongPtr(hEdit, GWLP_USERDATA, (LONG_PTR)data);//Сохранил указатель на структуру в USERDATA окна
	SetWindowLongPtr(hEdit, GWLP_WNDPROC, (LONG_PTR)Placeholder);//Заменил оконную процедуру на новую
	SetWindowText(hEdit, placeholderText);		//Установил текст подсказки в поле ввода
}
LRESULT CALLBACK Placeholder(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	EditData* data = (EditData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);	//Получил данные из USERDATA
	if (!data) return DefWindowProc(hWnd, uMsg, wParam, lParam);		//Если данных нет - вызывается стандартная обработка
	switch (uMsg)	//Обрабатываю сообщения windows
	{
	case WM_SETFOCUS:	//Окно получает фокус ввода
		if (data->isShowingPlaceholder)
		{
			data->isShowingPlaceholder = false;	//Убрал подсказку
			SetWindowTextA(hWnd, "");	//Очистил текст
		}
		break;
	case WM_KILLFOCUS:	//Окно теряет фокус ввода
		if (GetWindowTextLength(hWnd) == 0 && !data->isShowingPlaceholder)	//Если оно пустое и подсказка не отображается
		{
			data->isShowingPlaceholder = true;	//Показываю подсказку опять
			SetWindowTextA(hWnd, data->placeholderText);	//Устанавливаю текст подсказки
		}
		break;
	//case WM_CHAR:	//Если пользователь вводит символ
	//	if (data->isShowingPlaceholder)
	//	{
	//		data->isShowingPlaceholder = false;	//Убираю подсказку при вводе
	//		SetWindowTextA(hWnd, "");
	//		return (wParam == VK_BACK) ? 0 : CallWindowProc(data->originalProcedure, hWnd, uMsg, wParam, lParam);//Если не Backspace - передаю символ дальше
	//	}
	//	break;
	}
	return CallWindowProc(data->originalProcedure, hWnd, uMsg, wParam, lParam); //Для всех остальных сообщений вызываю оригинальную процедуру
}
 
 

 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
//#include<Windows.h>
//#include"resource.h"
//
//struct EditData {
//    const char* placeholderText;
//    WNDPROC originalProc;
//    bool isShowingPlaceholder;
//};
//
//BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//LRESULT CALLBACK SimplePlaceholderProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//void AddPlaceholderToEdit(HWND hEdit, const char* placeholderText);
//
//INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
//{
//    DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc, 0);
//    return 0;
//}
//
//BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//    switch (uMsg)
//    {
//    case WM_INITDIALOG:
//    {
//        HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
//        SendMessage(hwnd, WM_SETICON, 0, (LPARAM)hIcon);
//
//        AddPlaceholderToEdit(GetDlgItem(hwnd, IDC_EDIT_LOGIN), "Введите логин");
//        AddPlaceholderToEdit(GetDlgItem(hwnd, IDC_EDIT_PASSWORD), "Введите пароль");
//    }
//    break;
//
//    case WM_COMMAND:
//        switch (LOWORD(wParam))
//        {
//        case IDC_BUTTON_COPY:
//        {
//            CONST INT SIZE = 256;
//            CHAR sz_buffer[SIZE] = {};
//            HWND hEditLogin = GetDlgItem(hwnd, IDC_EDIT_LOGIN);
//            HWND hEditPassword = GetDlgItem(hwnd, IDC_EDIT_PASSWORD);
//
//            // Получаем реальный текст (игнорируем placeholder)
//            EditData* loginData = (EditData*)GetWindowLongPtr(hEditLogin, GWLP_USERDATA);
//            if (loginData && !loginData->isShowingPlaceholder) {
//                SendMessageA(hEditLogin, WM_GETTEXT, SIZE, (LPARAM)sz_buffer);
//                SendMessageA(hEditPassword, WM_SETTEXT, 0, (LPARAM)sz_buffer);
//            }
//            else {
//                // Если показывается placeholder, копируем пустую строку
//                SendMessageA(hEditPassword, WM_SETTEXT, 0, (LPARAM)"");
//            }
//        }
//        break;
//
//        case IDOK:
//            MessageBoxA(NULL, "Была нажата кнопка 'OK'", "Info", MB_OK | MB_ICONINFORMATION);
//            break;
//
//        case IDCANCEL:
//            EndDialog(hwnd, 0);
//            break;
//        }
//        break;
//
//    case WM_CLOSE:
//        EndDialog(hwnd, 0);
//        break;
//    }
//    return FALSE;
//}
//
//void AddPlaceholderToEdit(HWND hEdit, const char* placeholderText) {
//    // Выделяем память для данных
//    EditData* data = new EditData;
//    data->placeholderText = placeholderText;
//    data->isShowingPlaceholder = true;
//
//    // Сохраняем оригинальную процедуру
//    data->originalProc = (WNDPROC)GetWindowLongPtr(hEdit, GWLP_WNDPROC);
//
//    // Устанавливаем пользовательские данные
//    SetWindowLongPtr(hEdit, GWLP_USERDATA, (LONG_PTR)data);
//
//    // Устанавливаем нашу процедуру
//    SetWindowLongPtr(hEdit, GWLP_WNDPROC, (LONG_PTR)SimplePlaceholderProc);
//
//    // Устанавливаем начальный текст placeholder'а
//    SetWindowTextA(hEdit, placeholderText);
//
//    HDC hdc = GetDC(hEdit);
//    SetTextColor(hdc, RGB(128, 128, 128)); // Серый цвет
//    ReleaseDC(hEdit, hdc);
//}
//
//LRESULT CALLBACK SimplePlaceholderProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//    // Получаем наши данные из USERDATA
//    EditData* data = (EditData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
//
//    if (!data)
//    {
//        return DefWindowProc(hWnd, uMsg, wParam, lParam);
//    }
//
//    switch (uMsg)
//    {
//    case WM_SETFOCUS:
//        // При получении фокуса сразу убираем placeholder
//        if (data->isShowingPlaceholder) {
//            data->isShowingPlaceholder = false;
//            SetWindowTextA(hWnd, "");
//            HDC hdc = GetDC(hWnd);
//            SetTextColor(hdc, RGB(0, 0, 0)); // Черный цвет
//            ReleaseDC(hWnd, hdc);
//        }
//        break;
//
//    case WM_KILLFOCUS:
//        // При потере фокуса, если текст пустой - показываем placeholder
//        if (GetWindowTextLength(hWnd) == 0 && !data->isShowingPlaceholder) {
//            data->isShowingPlaceholder = true;
//            SetWindowTextA(hWnd, data->placeholderText);
//            HDC hdc = GetDC(hWnd);
//            SetTextColor(hdc, RGB(128, 128, 128)); // Серый цвет
//            ReleaseDC(hWnd, hdc);
//        }
//        break;
//
//    case WM_CHAR:
//        // При вводе любого символа убираем placeholder
//        if (data->isShowingPlaceholder) {
//            data->isShowingPlaceholder = false;
//            SetWindowTextA(hWnd, ""); // Очищаем поле
//            HDC hdc = GetDC(hWnd);
//            SetTextColor(hdc, RGB(0, 0, 0));
//            ReleaseDC(hWnd, hdc);
//
//            // Если это не Backspace, то обрабатываем символ
//            if (wParam != VK_BACK) {
//                // Передаем символ в оригинальную процедуру
//                return CallWindowProc(data->originalProc, hWnd, uMsg, wParam, lParam);
//            }
//            return 0; // Игнорируем Backspace для placeholder
//        }
//        break;
//
//    case WM_KEYDOWN:
//        // Для некоторых клавиш тоже убираем placeholder
//        if (data->isShowingPlaceholder &&
//            (wParam == VK_DELETE || wParam == VK_LEFT || wParam == VK_RIGHT ||
//                wParam == VK_HOME || wParam == VK_END)) {
//            data->isShowingPlaceholder = false;
//            SetWindowTextA(hWnd, "");
//            HDC hdc = GetDC(hWnd);
//            SetTextColor(hdc, RGB(0, 0, 0));
//            ReleaseDC(hWnd, hdc);
//        }
//        break;
//
//    case WM_PASTE:
//        // При вставке текста убираем placeholder
//        if (data->isShowingPlaceholder) {
//            data->isShowingPlaceholder = false;
//            SetWindowTextA(hWnd, "");
//            HDC hdc = GetDC(hWnd);
//            SetTextColor(hdc, RGB(0, 0, 0));
//            ReleaseDC(hWnd, hdc);
//        }
//        break;
//
//    case WM_GETTEXT:
//        // Если показывается placeholder, возвращаем пустую строку
//        if (data->isShowingPlaceholder) {
//            if (lParam != 0 && wParam > 0) {
//                char* buffer = (char*)lParam;
//                buffer[0] = '\0';
//            }
//            return 0;
//        }
//        break;
//
//    case WM_GETTEXTLENGTH:
//        // Если показывается placeholder, возвращаем 0
//        if (data->isShowingPlaceholder) {
//            return 0;
//        }
//        break;
//
//    case WM_DESTROY:
//        // Очищаем память при уничтожении окна
//        delete data;
//        SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
//        break;
//    }
//
//    // Всегда вызываем оригинальную процедуру
//    return CallWindowProc(data->originalProc, hWnd, uMsg, wParam, lParam);
//}