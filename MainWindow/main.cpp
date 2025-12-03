#include<Windows.h>
#include"resource.h"


CONST CHAR g_sz_WINDOW_CLASS[] = "My First Window";

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void WindowTitle(HWND hwnd);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(wClass));

	//Инициализируем размеры и стиль
	wClass.style = NULL;
	wClass.cbSize = sizeof(wClass);	//cb_ - Count Bytes
	wClass.cbClsExtra = 0;
	wClass.cbWndExtra = 0;

	//Инициализируем внешний вид окон:
	wClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_BITCOIN));
	wClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_PALM));
	//wClass.hIcon = (HICON)LoadImage(NULL, "bitcoin.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	//wClass.hIconSm = (HICON)LoadImage(NULL, "Palm.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);

	//wClass.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR1));
	wClass.hCursor = (HCURSOR)LoadImage
	(
		hInstance, 
		"Hornet alt.ani", 
		IMAGE_CURSOR, 
		LR_DEFAULTSIZE, LR_DEFAULTSIZE,
		LR_LOADFROMFILE
	);
	wClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	//Инициализация системных переменных:
	wClass.hInstance = hInstance;
	wClass.lpszClassName = g_sz_WINDOW_CLASS;
	wClass.lpszMenuName = NULL;
	wClass.lpfnWndProc = WndProc;

	if (RegisterClassEx(&wClass) == NULL)
	{
		MessageBox(NULL, "Class registration failed", NULL, MB_OK | MB_ICONERROR);
		return 0;
	}

	int displayWidth = GetSystemMetrics(SM_CXSCREEN);	//SM_CXSCREEN — возвращает ширину основного монитора в пикселях
	int displayHeight = GetSystemMetrics(SM_CYSCREEN);	//SM_CYSCREEN — возвращает высоту основного монитора в пикселях
	//https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getsystemmetrics
	int windowWidth = displayWidth * 3 / 4;
	int windowHeight = displayHeight * 3 / 4;

	int windowX = (displayWidth - windowWidth) / 2;
	int windowY = (displayHeight - windowHeight) / 2;

	HWND hwnd = CreateWindowEx
	(
		NULL,
		g_sz_WINDOW_CLASS,	//Имя класса окна
		g_sz_WINDOW_CLASS,	//Заголовок окна
		WS_OVERLAPPEDWINDOW,//Стиль окна, ситил зависят от класса окна
		windowX, windowY, //Position
		windowWidth, windowHeight, //Размер окна
		NULL,
		NULL, //Для главного окна - это ResourceID главного меню, 
		//для дочернего окна(Control) - ResourceID дочернего окна(IDC_BUTTON_COPY)
		hInstance,
		NULL
	);
	if (hwnd == NULL)
	{
		MessageBox(NULL, "Windows creation failed", NULL, MB_OK | MB_ICONERROR);
		return 0;
	}
	WindowTitle(hwnd);
	ShowWindow(hwnd, nCmdShow);	//Задаёт режим отображения онка - развёрнуто на весь экран, свёрнуто в окно
	//Свёрнуто на панель задач
	UpdateWindow(hwnd);	//Обновляет рабочую область указанного окна, отправляя сообщение 'WM_PAINT', если клиентская область окна не пустая

	//Запуск цикла сообщений:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);	//Преобразует сообщения виртуальных клавиш в символьное сообщение
		DispatchMessage(&msg);	//Отправляет сообщение о процедуре окна
	}
	return 0;
}

void WindowTitle(HWND hwnd)
{
	RECT rect;	//Структура RECT - хранит координаты окна, содержит 4 поля - left, right, top, bottom
	GetWindowRect(hwnd, &rect); //Получаем координаты и размер окна на экране,
	//Функция GetWindowRect заполняет структуру rect значениями

	int x = rect.left;	//Извлекаем X - координату левого верхнего угла окна
	int y = rect.top;	//Извлекаем Y - координату левого верхнего угла окна
	int width = rect.right - rect.left;	//Вычисляем ширину окна
	int height = rect.bottom - rect.top;//Вычисляем высоту окна

	CHAR title[512];	//Создаём буфер для строки заголовка
	CHAR temp[64];		//Создаём буфер для конкретных чисел

	lstrcpy(title, "My First Window: Position("); //Начинаем формировать заголовок
	//https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-lstrcpya
	wsprintf(temp, "%d", x);			//Преобразуем координату X в строку и сохраняем в temp
	lstrcat(title, temp);				//Добавляем координату X к заголовку
	lstrcat(title, ", ");				//Добавляем разделитель между координатами
	//https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-lstrcata
	wsprintf(temp, "%d", y);			//Преобразуем координату Y в строку и делаем всё тоже самое дальше
	lstrcat(title, temp);
	lstrcat(title, ") Size(");

	wsprintf(temp, "%d", width);		//Преобразуем ширину окна в строку
	lstrcat(title, temp);				//Добавляем ширину к заголовку
	lstrcat(title, " x ");				//Добавляем разделитель " x " между шириной и высотой

	wsprintf(temp, "%d", height);		//Преобразуем высоту окна в строку
	lstrcat(title, temp);				//Добавляем высоту к заголовку
	lstrcat(title, ")");				//Закрываем заголовок

	//===================================================================================//

	lstrcat(title, " | ");

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	wsprintf(temp, "Screen: %dx%d", screenWidth, screenHeight);
	lstrcat(title, temp);

	int percentX = (width * 100) / screenWidth;
	int percentY = (height * 100) / screenHeight;
	wsprintf(temp, " (%d%%x%d%%)", percentX, percentY);
	lstrcat(title, temp);


	SetWindowText(hwnd, title);			//Устанавливаем новый заголовок для окна
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		break;
	case WM_COMMAND:
		break;
	case WM_MOVE:
		//https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-move
		WindowTitle(hwnd);
		break;
	case WM_SIZE:
		WindowTitle(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return FALSE;
}