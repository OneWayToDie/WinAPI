#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<iostream>
#include"resource.h"

CONST CHAR g_sz_WINDOW_CLASS[] = "Shingeki";
CONST CHAR* g_sz_SKIN[] = { "square_blue", "metal_mistral", "toxic_yellow"};

CONST INT g_i_BUTTON_SIZE = 50;
CONST INT g_i_INTERVAL = 5;
CONST INT g_i_BUTTON_DOUBLE_SIZE = g_i_BUTTON_SIZE * 2 + g_i_INTERVAL;
CONST INT g_i_DISPLAY_WIDTH = g_i_BUTTON_SIZE * 5 + g_i_INTERVAL * 4;
CONST INT g_i_DISPLAY_HEIGHT = 22;
CONST INT g_i_START_X = 10;
CONST INT g_i_START_Y = 10;
CONST INT g_i_BUTTON_START_X = g_i_START_X;
CONST INT g_i_BUTTON_START_Y = g_i_START_Y + g_i_DISPLAY_HEIGHT + g_i_INTERVAL * 5;

CONST INT g_i_WINDOW_WIDTH = g_i_DISPLAY_WIDTH + g_i_START_X * 2 + 160;
CONST INT g_i_WINDOW_HEIGHT = g_i_DISPLAY_HEIGHT + g_i_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 4 + 64;

#define X_BUTTON_POSITION(position) g_i_BUTTON_START_X + (g_i_BUTTON_SIZE + g_i_INTERVAL) * (position)
#define Y_BUTTON_POSITION(position) g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * (position)

CONST CHAR g_OPERATIONS[] = "+-*/";

static HBITMAP g_hBackground = NULL;
static CHAR g_szCurrentBackground[64] = "Pole";

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

static CHAR g_history[10][256];
static INT g_historyCount = 0;



LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID SetSkin(HWND hwnd, CONST CHAR skin[]);
VOID SetBackground(HWND hwnd, CONST CHAR background[]);
VOID UpdateHistory(HWND hwnd);


INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	//1) Регистрация класа окна:
	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(wClass));

	wClass.style = 0;
	wClass.cbSize = sizeof(wClass);
	wClass.cbClsExtra = 0;
	wClass.cbWndExtra = 0;


	wClass.hIcon = (HICON)LoadImage(NULL, "Calculator.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	wClass.hIconSm = (HICON)LoadImage(NULL, "Calculator.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	wClass.hInstance = hInstance;
	wClass.lpszMenuName = NULL;
	wClass.lpfnWndProc = WndProc;
	wClass.lpszClassName = g_sz_WINDOW_CLASS;

	if (RegisterClassEx(&wClass) == NULL)
	{
		MessageBox(NULL, "Class registration failed", NULL, MB_OK | MB_ICONERROR);
		return 0;
	}
	//2) Создание окна:
	HWND hwnd = CreateWindowEx
	(
		NULL,											//exStyle
		g_sz_WINDOW_CLASS,								//Class
		g_sz_WINDOW_CLASS,								//Title
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,	//Style
		CW_USEDEFAULT, CW_USEDEFAULT,					//Position
		g_i_WINDOW_WIDTH, g_i_WINDOW_HEIGHT,			//Size
		NULL,
		NULL,
		hInstance,
		NULL
	);
	if (hwnd == NULL)
	{
		MessageBox(NULL, "WindowCreation failed", NULL, MB_OK | MB_ICONERROR);
		return 0;
	}
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	//3) Запуск цикла сообщения:
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int skinID = 0;
	switch (uMsg)
	{
	case WM_CREATE:
	{
		AllocConsole();
		freopen("CONOUT$", "w", stdout);

		SetBackground(hwnd, "Pole");

		CreateWindowEx
		(
			NULL,
			"Edit",
			"0",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
			10, 10,
			g_i_DISPLAY_WIDTH, g_i_DISPLAY_HEIGHT,
			hwnd,
			(HMENU)IDC_DISPLAY,
			GetModuleHandle(NULL),
			NULL
		);
		CHAR sz_button[2] = {};
		for (int i = 6; i >= 0; i -= 3)
		{
			for (int j = 0; j < 3; j++)
			{
				sz_button[0] = i + j + '1';
				CreateWindowEx
				(
					NULL, "Button", sz_button,
					WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
					X_BUTTON_POSITION(j), Y_BUTTON_POSITION(2 - i / 3),
					g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
					hwnd,
					HMENU(IDC_BUTTON_1 + i + j),
					GetModuleHandle(NULL),
					NULL
				);
			}
		}
		HWND hButtonZero = CreateWindowEx
		(
			NULL, "Button", "0",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			X_BUTTON_POSITION(0), Y_BUTTON_POSITION(3),
			g_i_BUTTON_DOUBLE_SIZE, g_i_BUTTON_SIZE,
			hwnd,
			(HMENU)IDC_BUTTON_0,
			GetModuleHandle(NULL),
			NULL
		);
		CreateWindowEx
		(
			NULL, "Button", ".",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			X_BUTTON_POSITION(2), Y_BUTTON_POSITION(3),
			g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
			hwnd,
			(HMENU)IDC_BUTTON_POINT,
			GetModuleHandle(NULL),
			NULL
		);
		CHAR sz_operation[2] = "";
		for (int i = 0; i < 4; i++)
		{
			sz_operation[0] = g_OPERATIONS[i];
			CreateWindowEx
			(
				NULL, "Button", sz_operation,
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
				X_BUTTON_POSITION(3), Y_BUTTON_POSITION(3 - i),
				g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
				hwnd,
				HMENU(IDC_BUTTON_PLUS + i),
				GetModuleHandle(NULL),
				NULL
			);
		}
		CreateWindowEx
		(
			NULL, "Button", "<-",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			X_BUTTON_POSITION(4), Y_BUTTON_POSITION(0),
			g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
			hwnd,
			(HMENU)IDC_BUTTON_BSP,
			GetModuleHandle(NULL),
			NULL
		);
		CreateWindowEx
		(
			NULL, "Button", "C",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			X_BUTTON_POSITION(4), Y_BUTTON_POSITION(1),
			g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
			hwnd,
			(HMENU)IDC_BUTTON_CLR,
			GetModuleHandle(NULL),
			NULL
		);
		CreateWindowEx
		(
			NULL, "Button", "=",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			X_BUTTON_POSITION(4), Y_BUTTON_POSITION(2),
			g_i_BUTTON_SIZE, g_i_BUTTON_SIZE * 2.1,
			hwnd,
			(HMENU)IDC_BUTTON_EQUAL,
			GetModuleHandle(NULL),
			NULL
		);

		CreateWindowEx(
			WS_EX_CLIENTEDGE,
			"EDIT",
			"История рассчётов:",
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
			g_i_DISPLAY_WIDTH + g_i_START_X * 2 + 10, 10,
			180, 262,
			hwnd,
			(HMENU)1020,
			GetModuleHandle(NULL),
			NULL
		);
		SetWindowPos
		(
			hwnd,
			NULL,
			0,0,
			g_i_WINDOW_WIDTH + 120,
			g_i_WINDOW_HEIGHT + 15,
			SWP_NOMOVE | SWP_NOZORDER 
		);

		SetSkin(hwnd, "square_blue");
	}
	break;
	case WM_COMMAND:
	{
		static DOUBLE a = DBL_MIN, b = DBL_MIN;
		static INT operation = 0;
		static BOOL input = FALSE;
		static BOOL input_operation = FALSE;
		static CHAR lastExpression[256] = "";
		static CHAR lastOperation = 0;  

		SetFocus(hwnd);
		HWND hEdit = GetDlgItem(hwnd, IDC_DISPLAY);
		CONST INT SIZE = 256;
		CHAR sz_buffer[SIZE] = {};
		CHAR sz_digit[2] = {};
		SendMessage(hEdit, WM_GETTEXT, SIZE, (LPARAM)sz_buffer);


		if (LOWORD(wParam) >= IDC_BUTTON_PLUS && LOWORD(wParam) <= IDC_BUTTON_SLASH)
		{
			strcpy(lastExpression, sz_buffer); //Сохраняю текущее содержимое дисплея в первый операнд

			switch (LOWORD(wParam))	//Определяю какой операнд нажат
			{
			case IDC_BUTTON_PLUS: lastOperation = '+'; break;
			case IDC_BUTTON_MINUS: lastOperation = '-'; break;
			case IDC_BUTTON_ASTER: lastOperation = '*'; break;
			case IDC_BUTTON_SLASH: lastOperation = '/'; break;
			}

			if (input)	//Проверяю инпут
			{
				if (a == DBL_MIN) a = atof(sz_buffer);	//Если переменная а содержит минимальное значение, значит первый операнд ещё не установлен
				else b = atof(sz_buffer);	//Или установлен
				input = FALSE;				//СБрасываю
			}
			operation = LOWORD(wParam);		//Сохраняю id операции в переменную operation
			input_operation = TRUE;			//Устанавливаю true

			return 0;
		}

		if (LOWORD(wParam) >= IDC_BUTTON_0 && LOWORD(wParam) <= IDC_BUTTON_9)	//Без изменений
		{
			if (input_operation)
			{
				sz_buffer[0] = 0;
				input_operation = FALSE;
			}
			sz_digit[0] = LOWORD(wParam) - IDC_BUTTON_0 + '0';
			if (strcmp(sz_buffer, "0") == 0)strcpy(sz_buffer, sz_digit);
			else	strcat(sz_buffer, sz_digit);
			SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)sz_buffer);
			input = TRUE;
			input_operation = FALSE;
		}

		if (LOWORD(wParam) == IDC_BUTTON_POINT)	//Без изменений
		{
			if (input_operation)
			{
				sz_buffer[0] = '0';
				sz_buffer[1] = '.';
				sz_buffer[2] = 0;
				input_operation = FALSE;
			}
			if (strchr(sz_buffer, '.') == 0)strcat(sz_buffer, ".");
			SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)sz_buffer);
			input = TRUE;
		}

		if (LOWORD(wParam) == IDC_BUTTON_BSP)	//Без изменений
		{
			if (!input)break;
			if (strlen(sz_buffer) == 1)sz_buffer[0] = '0';
			else	sz_buffer[strlen(sz_buffer) - 1] = 0;
			SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)sz_buffer);
		}

		if (LOWORD(wParam) == IDC_BUTTON_CLR)
		{
			a = b = DBL_MIN;
			operation = 0;
			input = input_operation = FALSE;
			lastOperation = 0;
			lastExpression[0] = '\0';
			SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)"0");
		}

		if (LOWORD(wParam) == IDC_BUTTON_EQUAL)
		{
			if (input)
			{
				if (a == DBL_MIN) a = atof(sz_buffer);
				else b = atof(sz_buffer);
				input = FALSE;
			}

			if (b == DBL_MIN)
			{
				b = atof(sz_buffer);
			}

			if (b == DBL_MIN) break;


			DOUBLE secondOperand = b;

			switch (operation)
			{
			case IDC_BUTTON_PLUS: a += b;	break;
			case IDC_BUTTON_MINUS: a -= b;	break;
			case IDC_BUTTON_ASTER: a *= b;	break;
			case IDC_BUTTON_SLASH: a /= b;	break;
			}

			input_operation = FALSE;
			sprintf(sz_buffer, "%g", a);
			SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)sz_buffer);

			if (lastOperation != 0 && strlen(lastExpression) > 0)	//Проверяю была ли сохранена операция(не равна нулю,
				//и есть ли сохранённое выражение
			{
				CHAR historyEntry[256];	//Создал временный буфер, для хранения строки истории
				sprintf(historyEntry, "%s %c %g = %s",	//Формирую строку истории с шаблоном "%s %c %g = %s"
					//lastExpression lastOperation secondOperand = sz_buffer
					lastExpression, lastOperation, secondOperand, sz_buffer);


				for (INT i = 9; i > 0; i--)	//Начинаю цикл с девяти и до нуля
				{
					strcpy(g_history[i], g_history[i - 1]);	//Копирую запись из предыдущей позиции массива в текущую
				}
				strcpy(g_history[0], historyEntry);	//Копирую новую запись истории в самую первую позицию массива

				if (g_historyCount < 10)	//Увеличиваю счётчик записей, но не больше чем на 10
					g_historyCount++;


				UpdateHistory(hwnd);	//Вызываю функцию обновления истории


				lastOperation = 0;	//Сбрасываю сохраненную операцию в 0
				lastExpression[0] = '\0';	//Делаю строку lastExpression пустой
			}

			b = DBL_MIN; //Сбрасываю второй операнд b в минимальное значение
		}
	}
	break;
	case WM_KEYDOWN:
	{
		if (GetKeyState(VK_SHIFT) < 0 && wParam == '8')
			SendMessage(GetDlgItem(hwnd, IDC_BUTTON_ASTER), BM_SETSTATE, TRUE, 0);
		else if (wParam >= '0' && wParam <= '9')
			SendMessage(GetDlgItem(hwnd, wParam - '0' + IDC_BUTTON_0), BM_SETSTATE, TRUE, 0);
		else if (wParam >= 0x60 && wParam <= 0x69)
			SendMessage(GetDlgItem(hwnd, wParam - 0x60 + IDC_BUTTON_0), BM_SETSTATE, TRUE, 0);
		switch (wParam)
		{
		case VK_OEM_PLUS:
		case VK_ADD: SendMessage(GetDlgItem(hwnd, IDC_BUTTON_PLUS), BM_SETSTATE, TRUE, 0); break;
		case VK_OEM_MINUS:
		case VK_SUBTRACT: SendMessage(GetDlgItem(hwnd, IDC_BUTTON_MINUS), BM_SETSTATE, TRUE, 0); break;
		case VK_MULTIPLY: SendMessage(GetDlgItem(hwnd, IDC_BUTTON_ASTER), BM_SETSTATE, TRUE, 0); break;
		case VK_OEM_2:
		case VK_DIVIDE: SendMessage(GetDlgItem(hwnd, IDC_BUTTON_SLASH), BM_SETSTATE, TRUE, 0); break;
		case VK_OEM_PERIOD:
		case VK_DECIMAL:
			SendMessage(GetDlgItem(hwnd, IDC_BUTTON_POINT), BM_SETSTATE, TRUE, 0); break;



		case VK_BACK: SendMessage(GetDlgItem(hwnd, IDC_BUTTON_BSP), BM_SETSTATE, TRUE, 0); break;
		case VK_ESCAPE: SendMessage(GetDlgItem(hwnd, IDC_BUTTON_CLR), BM_SETSTATE, TRUE, 0); break;
		case VK_RETURN: SendMessage(GetDlgItem(hwnd, IDC_BUTTON_EQUAL), BM_SETSTATE, TRUE, 0); break;
		}
	}
	break;
	case WM_KEYUP:
	{
		if (GetKeyState(VK_SHIFT) < 0 && wParam == '8')
		{
			SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_ASTER), 0);
			SendMessage(GetDlgItem(hwnd, IDC_BUTTON_ASTER), BM_SETSTATE, FALSE, 0);
		}
		else if (wParam >= '0' && wParam <= '9')
		{
			SendMessage(hwnd, WM_COMMAND, LOWORD(wParam - '0' + IDC_BUTTON_0), 0);
			SendMessage(GetDlgItem(hwnd, wParam - '0' + IDC_BUTTON_0), BM_SETSTATE, FALSE, 0);
		}
		else if (wParam >= 0x60 && wParam <= 0x69)
		{
			SendMessage(hwnd, WM_COMMAND, LOWORD(wParam - 0x60 + IDC_BUTTON_0), 0);
			SendMessage(GetDlgItem(hwnd, wParam - '0' + IDC_BUTTON_0), BM_SETSTATE, FALSE, 0);
		}
		switch (wParam)
		{
		case VK_OEM_PLUS:
		case VK_ADD:
			SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_PLUS), 0);
			SendMessage(GetDlgItem(hwnd, IDC_BUTTON_PLUS), BM_SETSTATE, FALSE, 0);
			break;
		case VK_OEM_MINUS:
		case VK_SUBTRACT:
			SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_MINUS), 0);
			SendMessage(GetDlgItem(hwnd, IDC_BUTTON_MINUS), BM_SETSTATE, FALSE, 0);
			break;
		case VK_MULTIPLY:
			SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_ASTER), 0);
			SendMessage(GetDlgItem(hwnd, IDC_BUTTON_ASTER), BM_SETSTATE, FALSE, 0);
			break;
		case VK_OEM_2:
		case VK_DIVIDE:
			SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_SLASH), 0);
			SendMessage(GetDlgItem(hwnd, IDC_BUTTON_SLASH), BM_SETSTATE, FALSE, 0);
			break;
		case VK_OEM_PERIOD:
		case VK_DECIMAL:
			SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_POINT), 0);
			SendMessage(GetDlgItem(hwnd, IDC_BUTTON_POINT), BM_SETSTATE, FALSE, 0);
			break;

		case VK_BACK:
			SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_BSP), 0);
			SendMessage(GetDlgItem(hwnd, IDC_BUTTON_BSP), BM_SETSTATE, FALSE, 0);
			break;
		case VK_ESCAPE:
			SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_CLR), 0);
			SendMessage(GetDlgItem(hwnd, IDC_BUTTON_CLR), BM_SETSTATE, FALSE, 0);
			break;
		case VK_RETURN:
			SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_EQUAL), 0);
			SendMessage(GetDlgItem(hwnd, IDC_BUTTON_EQUAL), BM_SETSTATE, FALSE, 0);
			break;
		}
	}
	break;
	case WM_CONTEXTMENU:
	{
		HMENU cmMain = CreatePopupMenu();
		AppendMenu(cmMain, MF_STRING, IDM_SQUARE_BLUE, "Square blue");
		AppendMenu(cmMain, MF_STRING, IDM_METAL_MISTRAL, "Metal mistral");
		AppendMenu(cmMain, MF_STRING, IDM_TOXIC_YELLOW, "Toxic Yellow");
		AppendMenu(cmMain, MF_SEPARATOR, NULL, NULL);

		HMENU hBackgroundMenu = CreatePopupMenu();
		AppendMenu(hBackgroundMenu, MF_STRING, IDM_BACKGROUND_NONE, "EmPtY");
		AppendMenu(cmMain, MF_SEPARATOR, NULL, NULL);
		AppendMenu(hBackgroundMenu, MF_STRING, IDM_TOKYO, "Tokyo");
		AppendMenu(hBackgroundMenu, MF_STRING, IDM_TOKYO_2, "Tokyo 2");
		AppendMenu(hBackgroundMenu, MF_STRING, IDM_DOMA_I_NEBO, "Doma");
		AppendMenu(hBackgroundMenu, MF_STRING, IDM_KOTIKI, "Kotiki");
		AppendMenu(hBackgroundMenu, MF_STRING, IDM_NOCHNOY_GOROD, "Nochnoy gorod");
		AppendMenu(hBackgroundMenu, MF_STRING, IDM_POLE, "Pole");
		AppendMenu(hBackgroundMenu, MF_STRING, IDM_UTRENNII_TRAMVAI, "Utro");

		AppendMenu(cmMain, MF_POPUP, (UINT_PTR)hBackgroundMenu, "Выбор фона");
		AppendMenu(cmMain, MF_SEPARATOR, NULL, NULL);
		AppendMenu(cmMain, MF_STRING, IDM_EXIT, "Exit");

		BOOL selected_item = TrackPopupMenuEx
		(
			cmMain, 
			TPM_RIGHTALIGN | TPM_BOTTOMALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_VERNEGANIMATION,
			LOWORD(lParam), HIWORD(lParam),
			hwnd,
			NULL
		);
		switch (selected_item)
		{
		case IDM_SQUARE_BLUE:		skinID = 0;/*SetSkin(hwnd, "square_blue");*/			break;
		case IDM_METAL_MISTRAL:		skinID = 1;/*SetSkin(hwnd, "metal_mistral");*/			break;
		case IDM_TOXIC_YELLOW:		skinID = 2;/*SetSkin(hwnd, "toxic_yellow");*/			break;
		case IDM_BACKGROUND_NONE:	SetBackground(hwnd, "none");			break;
		case IDM_TOKYO:				SetBackground(hwnd, "Tokyo");			break;
		case IDM_TOKYO_2:			SetBackground(hwnd, "Tokyo_2");			break;
		case IDM_DOMA_I_NEBO:		SetBackground(hwnd, "doma_i_nebo");		break;
		case IDM_KOTIKI:			SetBackground(hwnd, "Kotiki");			break;
		case IDM_NOCHNOY_GOROD:		SetBackground(hwnd, "Nochnoy_gorod");	break;
		case IDM_POLE:				SetBackground(hwnd, "Pole");			break;
		case IDM_UTRENNII_TRAMVAI:	SetBackground(hwnd, "Utrennii_tramvai");break;
		case IDM_EXIT:				SendMessage(hwnd, WM_CLOSE, 0, 0);		break;
		}
		SetSkin(hwnd, g_sz_SKIN[skinID]);
		DestroyMenu(cmMain);
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;//PaintStruct содержит информацию о том, какую область нужно перерисовывать
		HDC hdc = BeginPaint(hwnd, &ps);	//BeginPaint - подготавливает окно к рисованию и возвращает HDC

		if (g_hBackground && strcmp(g_szCurrentBackground, "none") != 0)	//Проверяю два условия - существует загруженное изображение и текущий фон не none
		{
			HDC hdcMem = CreateCompatibleDC(hdc);	//Создаю совместимый контекст устройства в памяти
			HBITMAP Bitmap = (HBITMAP)SelectObject(hdcMem, g_hBackground);	//Вставляю изображение в контекст памяти, возвращаю старое изображение и сохраняю его, чтобы потом восстановить

			BITMAP bm;	
			GetObject(g_hBackground, sizeof(BITMAP), &bm);	//Получаю информацию о bitmap, заполняю структуру данными

			RECT rectClient;	
			GetClientRect(hwnd, &rectClient);	//Получаю размеры области окна
			StretchBlt(hdc, 0, 0, rectClient.right, rectClient.bottom,	//Рисование с растяжением
				hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
			SelectObject(hdcMem, Bitmap);	//Восстанавливаю старое изображение в контекст памяти, чтоб не было утечки ресурсов
			DeleteDC(hdcMem);	//Очищаю всё за собой
		}
		else	//Если нет фона - рисую стандартный фон windows
		{
			HBRUSH hBrush = (HBRUSH)(COLOR_WINDOW + 1);
			FillRect(hdc, &ps.rcPaint, hBrush);	//Заливаю область стандартной кистью
		}

		INT y = 40; // Начальная позиция
		for (INT i = 0; i < g_historyCount; i++)
		{
			TextOut(hdc, g_i_WINDOW_WIDTH + 15, y, g_history[i], strlen(g_history[i]));
			y += 20; // Отступ между записями
		}
		EndPaint(hwnd, &ps);	//Завершаю процесс рисования
		return 0; 
	}
	break;
	case WM_DESTROY:
	{
		FreeConsole();
		PostQuitMessage(0);
	}
	break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return FALSE;
}

VOID SetSkin(HWND hwnd, CONST CHAR skin[])
{
	CHAR sz_filename[FILENAME_MAX] = {};

	for (int i = 0; i < 18; i++)
	{
		sprintf(sz_filename, "ButtonsBMP\\%s\\button_%i.bmp", skin, i);
		HWND hButton = GetDlgItem(hwnd, IDC_BUTTON_0 + i);
		HBITMAP bmpButton = (HBITMAP)LoadImage
		(
			GetModuleHandle(NULL),
			sz_filename,
			IMAGE_BITMAP,
			i == 0 ? g_i_BUTTON_DOUBLE_SIZE : g_i_BUTTON_SIZE,
			i == 17 ? g_i_BUTTON_DOUBLE_SIZE : g_i_BUTTON_SIZE,
			LR_LOADFROMFILE
		);
		SendMessage(hButton, BM_SETIMAGE, 0, (LPARAM)bmpButton);
	}
}
VOID SetBackground(HWND hwnd, CONST CHAR background[])
{
	if (g_hBackground) // Проверяю на существование предыдущего фонового изображения,
		//если оно не NULL - обнуляю
	{
		DeleteObject(g_hBackground);	//https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-deleteobject
		g_hBackground = NULL;
	}
	strcpy(g_szCurrentBackground, background);	//Сохраняю имя текущего фона в глобальную переменную
	if (strcmp(background, "none") == 0)	//Проверка на то, не выбрал ли пользователь опцию - без фона
	{
		InvalidateRect(hwnd, NULL, TRUE);	//Если выбран none - сообщаю, что нужно перерисовать окно 
		//https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-invalidaterect
		return;
	}
	CHAR szFilename[MAX_PATH]; // Создаю буфер для хранения полного пути к файлу
	sprintf(szFilename, "Backgrounds\\%s.bmp", background);	//Задаю путь к файлу

	g_hBackground = (HBITMAP)LoadImage(NULL, szFilename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);	//Загружаю изображение из файла

	if (!g_hBackground)	//Проверяю, успешно ли загрузилось изображение
	{
		MessageBox(hwnd, "Не удалось загрузить изображение", "Ошибка", MB_OK | MB_ICONERROR);
		return;
	}

	InvalidateRect(hwnd, NULL, TRUE);//Сообщаю винде о перерисовке окна с новым фоном
}

VOID UpdateHistory(HWND hwnd)
{
	HWND hEdit = GetDlgItem(hwnd, 1020);	//Получаю Handle дескриптор Edit-контроля с id 1020
	CHAR historyText[2048] = "История рассчётов:\r\n";	//Создаю буфер для текста истории

	for (INT i = 0; i < g_historyCount; i++)	//Произвожу цикл по всем записям истории
	{
		strcat(historyText, g_history[i]);	//Добавляю текущую запись истории в буфер
		strcat(historyText, "\r\n");		//Добавляю перевод строки после каждой записи
	}
	SetWindowText(hEdit, historyText);	//Устанавливаю текст в Edit-контроль
}