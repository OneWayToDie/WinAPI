#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<string>
#include<sstream>
#include"resource.h"

CONST CHAR g_sz_WINDOW_CLASS[] = "KLMSLF";

CONST INT g_i_BUTTON_SIZE = 50;
CONST INT g_i_INTERVAL = 5;
CONST INT g_i_BUTTON_DOUBLE_SIZE = g_i_BUTTON_SIZE * 2 + g_i_INTERVAL;
CONST INT g_i_DISPLAY_WIDTH = g_i_BUTTON_SIZE * 5 + g_i_INTERVAL * 4;
CONST INT g_i_DISPLAY_HEIGHT = 22;
CONST INT g_i_START_X = 10;
CONST INT g_i_START_Y = 10;
CONST INT g_i_BUTTON_START_X = g_i_START_X;
CONST INT g_i_BUTTON_START_Y = g_i_START_Y + g_i_DISPLAY_HEIGHT + g_i_INTERVAL * 5;

CONST INT g_i_WINDOW_WIDTH = g_i_DISPLAY_WIDTH + g_i_START_X * 2 + 16;
CONST INT g_i_WINDOW_HEIGHT = g_i_DISPLAY_HEIGHT + g_i_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 4 + 64;


std::string displayText = "0";
double first_Number = 0.0;
double second_Number = 0.0;
char currentOperation = '\0';
bool new_Number = true;
bool decimalPoint = false;
bool operationPressed = false;


std::string expressionText = "";
std::string firstNumberStr = "";

void Calculate();
void Calc_Function(char digit);
void Operation(char operation);
void Clear();
void Backspace();
void Update_Display(HWND hwnd);
std::string DoubleToString(double num);

#define X_BUTTON_POSITION(position) g_i_BUTTON_START_X + (g_i_BUTTON_SIZE + g_i_INTERVAL) * (position)
#define Y_BUTTON_POSITION(position) g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * (position)

CONST CHAR g_OPERATIONS[] = "+-*/";

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	//1) Регистрация класса окна:
	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(wClass));

	wClass.style = 0;
	wClass.cbSize = sizeof(wClass);
	wClass.cbClsExtra = 0;
	wClass.cbWndExtra = 0;

	/*wClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);*/
	wClass.hIcon = (HICON)LoadImage(NULL, "calculator.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	wClass.hIconSm = (HICON)LoadImage(NULL, "calculator.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	/*wClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);*/
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
		NULL,
		g_sz_WINDOW_CLASS,
		g_sz_WINDOW_CLASS,
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		g_i_WINDOW_WIDTH, g_i_WINDOW_HEIGHT,
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

std::string DoubleToString(double num)
{
	std::stringstream ss;
	ss << num;
	std::string result = ss.str();

	size_t dotPos = result.find('.');
	if (dotPos != std::string::npos)
	{
		while (result.back() == '0')
			result.pop_back();
		if (result.back() == '.')
			result.pop_back();
	}

	return result;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		CreateWindowEx
		(
			NULL,
			"Edit",
			"0",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT | ES_READONLY,
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
					WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					X_BUTTON_POSITION(j), Y_BUTTON_POSITION(2 - i / 3),
					g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
					hwnd,
					HMENU(IDC_BUTTON_1 + i + j),
					GetModuleHandle(NULL),
					NULL
				);
			}
		}

		CreateWindowEx
		(
			NULL, "Button", "0",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
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
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
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
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
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
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
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
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
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
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			X_BUTTON_POSITION(4), Y_BUTTON_POSITION(2),
			g_i_BUTTON_SIZE, g_i_BUTTON_SIZE * 2 + g_i_INTERVAL,
			hwnd,
			(HMENU)IDC_BUTTON_EQUAL,
			GetModuleHandle(NULL),
			NULL
		);
	}
	break;

	case WM_COMMAND:
	{
		SetFocus(hwnd);
		int id = LOWORD(wParam);
		if (id == IDC_DISPLAY)
			break;

		switch (id)
		{
		case IDC_BUTTON_0: Calc_Function('0'); break;
		case IDC_BUTTON_1: Calc_Function('1'); break;
		case IDC_BUTTON_2: Calc_Function('2'); break;
		case IDC_BUTTON_3: Calc_Function('3'); break;
		case IDC_BUTTON_4: Calc_Function('4'); break;
		case IDC_BUTTON_5: Calc_Function('5'); break;
		case IDC_BUTTON_6: Calc_Function('6'); break;
		case IDC_BUTTON_7: Calc_Function('7'); break;
		case IDC_BUTTON_8: Calc_Function('8'); break;
		case IDC_BUTTON_9: Calc_Function('9'); break;

		case IDC_BUTTON_POINT: Calc_Function('.'); break;

		case IDC_BUTTON_PLUS: Operation('+'); break;
		case IDC_BUTTON_MINUS: Operation('-'); break;
		case IDC_BUTTON_ASTER: Operation('*'); break;
		case IDC_BUTTON_SLASH: Operation('/'); break;

		case IDC_BUTTON_EQUAL: Calculate(); break;
		case IDC_BUTTON_CLR: Clear(); break;
		case IDC_BUTTON_BSP: Backspace(); break;
		}
		Update_Display(hwnd);
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

void Update_Display(HWND hwnd)
{
	HWND hDisplay = GetDlgItem(hwnd, IDC_DISPLAY);	
	if (hDisplay != NULL)	//Провереям окно дисплея на существование
	{
		if (!expressionText.empty())	//Если expressionText не пустая, отображаем полное выражение
		{
			SetWindowText(hDisplay, expressionText.c_str());
		}
		else	//Иначе показываем только текущее число
		{
			SetWindowText(hDisplay, displayText.c_str());
		}
	}
}

void Clear()
{
	displayText = "0";
	first_Number = 0.0;
	second_Number = 0.0;
	currentOperation = '\0';
	new_Number = true;
	decimalPoint = false;
	operationPressed = false;
	expressionText = "";
	firstNumberStr = "";
}

void Backspace()
{
	if (currentOperation != '\0' && !new_Number)	// Если есть операция и мы вводим второе число
	{
		if (displayText.length() > 1)
		{
			if (displayText.back() == '.')
				decimalPoint = false;
			displayText.pop_back();

			expressionText = firstNumberStr + " " + currentOperation + " " + displayText; // Обновляем выражение с текущим вторым числом
		}
		else
		{
			displayText = "0";
			new_Number = true;
			expressionText = firstNumberStr + " " + currentOperation;
		}
	}
	else
	{
		if (displayText.length() > 1)
		{
			if (displayText.back() == '.')
				decimalPoint = false;
			displayText.pop_back();
		}
		else
		{
			displayText = "0";
			new_Number = true;
		}
		expressionText = "";
	}
}

void Calc_Function(char digit)
{
	if (new_Number)
	{
		displayText = digit;
		new_Number = false;
		if (digit == '.')
		{
			displayText = "0.";
			decimalPoint = true;
		}
	}
	else
	{
		if (digit == '.')
		{
			if (!decimalPoint)
			{
				displayText += digit;
				decimalPoint = true;
			}
		}
		else
		{
			displayText += digit;
		}
	}
	if (displayText.length() > 1 && displayText[0] == '0' && displayText[1] != '.')
	{
		displayText.erase(0, 1);
	}

	if (currentOperation != '\0')
	{
		operationPressed = false;  
		expressionText = firstNumberStr + " " + currentOperation + " " + displayText;
	}
}

void Operation(char operation)
{
	if (currentOperation == '\0' || operationPressed)
	{
		std::stringstream ss(displayText);
		ss >> first_Number;
		firstNumberStr = displayText;

		currentOperation = operation;

		expressionText = firstNumberStr + " " + currentOperation;

		displayText = "0";
		new_Number = true;
		decimalPoint = false;
		operationPressed = true;
	}
	else
	{
		Calculate();

		firstNumberStr = DoubleToString(first_Number);
		currentOperation = operation;
		expressionText = firstNumberStr + " " + currentOperation;
		displayText = "0";
		new_Number = true;
		decimalPoint = false;
		operationPressed = true;
	}
}

void Calculate()
{
	if (currentOperation == '\0')
		return;

	std::stringstream ss(displayText);
	ss >> second_Number;

	std::string secondNumberStr = displayText;

	switch (currentOperation)
	{
	case '+':
		second_Number = first_Number + second_Number;
		break;
	case '-':
		second_Number = first_Number - second_Number;
		break;
	case '*':
		second_Number = first_Number * second_Number;
		break;
	case '/':
		if (second_Number != 0)
			second_Number = first_Number / second_Number;
		else
		{
			displayText = "На ноль не делится";
			expressionText = firstNumberStr + " " + currentOperation + " " + secondNumberStr + " = " + displayText;
			return;
		}
		break;
	}

	std::string resultStr = DoubleToString(second_Number);

	expressionText = firstNumberStr + " " + currentOperation + " " + secondNumberStr + " = " + resultStr;

	displayText = resultStr;
	first_Number = second_Number;
	firstNumberStr = resultStr;

	currentOperation = '\0';
	new_Number = true;
	decimalPoint = false;
	operationPressed = false;
}