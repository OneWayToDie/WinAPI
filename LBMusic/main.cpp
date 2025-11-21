#define _CRT_SECURE_NO_WARNINGS //Отключаю предупреждения о небезопасных функциях CRT
#include<Windows.h>
#include<mmsystem.h>
#include<string>
#include"resource.h"
#pragma comment(lib, "winmm.lib")   //Директива, автоматически подключающая библиотеку winmm.lib для мультимедийных функций

CONST CHAR* g_sz_VALUES[] =
{
"Decalius - Loneliness",
"P.O.D. - Youth of the Nation",
"Reka — Life Long Tragedy",
"takizava cold mental",
"Deer Death — Melt Into You",
"Никто Не Против - Курю с призраками",
};	//Массив из названий песен

struct SongData
{
	const char* songName;//Название песни
	const char* songText;//Текст
	const char* filePath;//Путь к файлу
};

HINSTANCE g_hInstance;
SongData g_currentSong; //структура с данными текущей выбранной песни
HWND g_hCurrentSongDialog = NULL;   //Рукоятка окна с текстом песни

//Прототипы функций
BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
const char* GetSongText(const char* songName);
const char* GetSongFilePath(const char* songName);
void PlayMusicFile(const char* filePath);
void StopMusic();
void CreateSongWindow(HWND hParent);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_EXPLORER), NULL, (DLGPROC)DlgProc, 0);
	return 0;
}


BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
        HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));  //Подключил иконку для первого окна
        SendMessage(hwnd, WM_SETICON, 0, (LPARAM)hIcon);
		HWND hList = GetDlgItem(hwnd, IDC_LISTBOX_MUSIC);
		for (int i = 0; i < sizeof(g_sz_VALUES) / sizeof(g_sz_VALUES[0]); i++)  //Вычисляем количество элементов массива и проходимся по нему
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
            HICON hIcon2 = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON2)); //Прикрепил иконку для второй приложухи
            SendMessage(hwnd, WM_SETICON, 0, (LPARAM)hIcon2);
			HWND hList = GetDlgItem(hwnd, IDC_LISTBOX_MUSIC);
			int index = (int)SendMessage(hList, LB_GETCURSEL, 0, 0);
			if (index != LB_ERR)
			{
				char selectedText[256];
				SendMessage(hList, LB_GETTEXT, index, (LPARAM)selectedText);

                //Заполнили структуру данными новой песни
				g_currentSong.songName = selectedText; 
				g_currentSong.songText = GetSongText(selectedText);
				g_currentSong.filePath = GetSongFilePath(selectedText);

				CreateSongWindow(hwnd);//Создали окно с текстом песни
			}
			else
			{
				MessageBox(hwnd, "Вы ничего не выбрали", "Info", MB_OK | MB_ICONINFORMATION);
			}
		}
		break;
        case IDC_PLAY_BUTTON:
            if (g_currentSong.filePath != NULL)
            {
                //Если путь к файлу существует - взвали функцию воспроизведения
                PlayMusicFile(g_currentSong.filePath);
                MessageBox(hwnd, "Воспроизведение начато", "Info", MB_OK | MB_ICONINFORMATION);
            }
            else
            {
                MessageBox(hwnd, "Файл не найден", "Ошибка", MB_OK | MB_ICONERROR);
            }
            break;
		case IDC_STOP_BUTTON:
			StopMusic();//Остановка воспроизведения
			MessageBox(hwnd, "Воспроизведение остановлено", "Info", MB_OK | MB_ICONINFORMATION);
			break;

		case IDC_BACK_BUTTON:
			if (g_hCurrentSongDialog)
			{
				StopMusic();
				DestroyWindow(g_hCurrentSongDialog);//Уничтожаем окно
				g_hCurrentSongDialog = NULL;    //Сбрасываем рукоятку
			}
			break;

		case IDCANCEL:
			StopMusic();
			if (g_hCurrentSongDialog)
				DestroyWindow(g_hCurrentSongDialog);
			EndDialog(hwnd, 0);//Выход из приложения
			break;
		}
	}
	break;
	case WM_CLOSE:
		StopMusic();
		if (g_hCurrentSongDialog)
			DestroyWindow(g_hCurrentSongDialog);
		EndDialog(hwnd, 0);
		break;
	}
	return FALSE;
}

void CreateSongWindow(HWND hParent)
{
    if (g_hCurrentSongDialog)
    {
        DestroyWindow(g_hCurrentSongDialog);    //Если окно существует - уничтожаем его
    }

    // Получаю дополнительную информацию об окне и рукоятку приложения
    HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hParent, GWLP_HINSTANCE);

    // Создаю немодальное диалоговое окно, без блокировки родительского окна
    g_hCurrentSongDialog = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_SONG_DIALOG),
        hParent, (DLGPROC)DlgProc, 0);

    if (g_hCurrentSongDialog)
    {
        // Устанавливаю заголовок окна
        SetWindowText(g_hCurrentSongDialog, g_currentSong.songName);

        // Устанавливаю текст песни в Edit Control
        HWND hEdit = GetDlgItem(g_hCurrentSongDialog, IDC_SONG_TEXT);
        SetWindowText(hEdit, g_currentSong.songText);

        // Проверяю доступность воспроизведения
        HWND hPlayBtn = GetDlgItem(g_hCurrentSongDialog, IDC_PLAY_BUTTON);
        if (g_currentSong.filePath == NULL)
        {
            EnableWindow(hPlayBtn, FALSE);
            SetWindowText(hPlayBtn, "Файл не найден");
        }

        // Показываю окно
        ShowWindow(g_hCurrentSongDialog, SW_SHOW);  //SW_SHOW - нормальное отображение(ShowWindow)
    }
}

// Функция для получения текста песен
const char* GetSongText(const char* songName)
{
    if (strcmp(songName, "Decalius - Loneliness") == 0) //Strcmp - Сравнивает строки и возвращает 0, если равны
    {
        return "Walking through the empty streets\r\n"  //r/n - Символы для переноса строк в WinAPI
               "Shadows dancing in the night\r\n"
               "Loneliness is my only friend\r\n"
               "In this never-ending fight\r\n"
               "Whispering winds call my name\r\n"
               "Echoes of a forgotten dream\r\n"
               "Silent tears fall like rain\r\n"
               "Nothing is quite as it seems\r\n"
               "Loneliness, my constant guide\r\n"
               "Nowhere left to run and hide\r\n"
               "In the silence I reside\r\n"
               "With nothing but my pride\r\n"
               "Memories of brighter days\r\n"
               "Lost in a hazy maze\r\n"
               "Searching for a guiding light\r\n"
               "Through the endless night\r\n"
               "Alone with my thoughts again\r\n"
               "Trying to break this chain\r\n"
               "But loneliness remains the same\r\n"
                "Playing its endless game";
    }
    else if (strcmp(songName, "P.O.D. - Youth of the Nation") == 0)
    {
        return "Last day of the rest of my life\r\n"
               "I wish I would've known\r\n"
               "Cause I didn't kiss my mama goodbye\r\n"
               "I didn't tell her that I loved her\r\n"
               "How I cared\r\n"
               "Thank you for giving me life\r\n"
               "Thank you for raising me right\r\n"
               "Thank you for teaching me how to fight\r\n"
               "Now I've got to say goodbye\r\n"
               "We are, we are the youth of the nation\r\n"
               "We are, we are the youth of the nation\r\n"
               "We are, we are the youth of the nation\r\n"
               "We are, we are the youth of the nation\r\n"
               "Little Suzy, she was only twelve\r\n"
               "She was given the world\r\n"
               "Every day she had to fight\r\n"
               "For her place in this world\r\n"
               "Thirty years of innocence\r\n"
               "Pushed aside so violent\r\n"
               "Nothing could save her now\r\n"
               "No one could help her out\r\n"
               "We are, we are the youth of the nation\r\n"
               "We are, we are the youth of the nation\r\n"
               "We are, we are the youth of the nation\r\n"
               "We are, we are the youth of the nation\r\n"
               "Johnny boy, he always played the fool\r\n"
               "He broke all the rules\r\n"
               "So you would think he was cool\r\n"
               "He was never really one of the guys\r\n"
               "No matter how hard he tried\r\n"
               "Often thought of suicide\r\n"
               "It's kind of hard when you ain't got no friends\r\n"
               "He put his life to an end\r\n"
               "They might remember him then\r\n"
               "We are, we are the youth of the nation\r\n"
               "We are, we are the youth of the nation\r\n"
               "We are, we are the youth of the nation\r\n"
                "We are, we are the youth of the nation";
    }
    else if (strcmp(songName, "Reka — Life Long Tragedy") == 0)
    {
        return "Another day another struggle\r\n"
            "Fighting battles I can't win\r\n"
            "This constant state of trouble\r\n"
            "Is where my life begins\r\n"
            "Broken dreams and shattered hopes\r\n"
            "Climbing up a endless slope\r\n"
            "Trying hard to just survive\r\n"
            "In this tragedy I'm alive\r\n"
            "Life long tragedy\r\n"
            "This is my reality\r\n"
            "Life long tragedy\r\n"
            "Chains of misery\r\n"
            "Walking down this lonely road\r\n"
            "Carrying a heavy load\r\n"
            "Memories of better times\r\n"
            "Fading like the morning chimes\r\n"
            "Every step feels like the last\r\n"
            "Living in the shadow cast\r\n"
            "By the person I used to be\r\n"
            "Now just a distant memory\r\n"
            "Life long tragedy\r\n"
            "This is my reality\r\n"
            "Life long tragedy\r\n"
            "Chains of misery\r\n"
            "I scream but there's no sound\r\n"
            "I'm lost but can't be found\r\n"
            "Running circles in my mind\r\n"
            "Leaving everything behind\r\n"
            "The walls are closing in\r\n"
            "This battle I can't win\r\n"
            "Trapped inside this endless night\r\n"
            "With no will left to fight\r\n"
            "Life long tragedy\r\n"
            "This is my reality\r\n"
            "Life long tragedy\r\n"
            "Chains of misery\r\n"
            "Maybe someday I'll break free\r\n"
            "From this life long tragedy";
    }
    else if (strcmp(songName, "takizava cold mental") == 0)
    {
        return "Frozen thoughts inside my head\r\n"
           "Ice cold whispers, things unsaid\r\n"
           "Walking through this mental winter\r\n"
           "Feeling myself slowly splinter\r\n"
           "Numb emotions, frozen time\r\n"
           "Trapped inside this mind of mine\r\n"
           "Every breath feels like the last\r\n"
           "Living in a frozen past\r\n"
           "Cold mental, breaking down\r\n"
           "Lost in silence, no one around\r\n"
           "Cold mental, can't be found\r\n"
           "In this emptiness I'm bound\r\n"
           "Windows to a soul that's faded\r\n"
           "Dreams and hopes have been abated\r\n"
           "Building walls so high and thick\r\n"
           "Every stone another trick\r\n"
           "To keep the world outside away\r\n"
           "Another night, another day\r\n"
           "Surviving in this hollow space\r\n"
           "Can't remember my own face\r\n"
           "Cold mental, breaking down\r\n"
           "Lost in silence, no one around\r\n"
           "Cold mental, can't be found\r\n"
           "In this emptiness I'm bound\r\n"
           "The chill sets deep into the bone\r\n"
           "Facing everything alone\r\n"
           "Echoes in an empty hall\r\n"
           "Waiting for the final fall\r\n"
           "No warmth left to feel the pain\r\n"
           "Just the freezing mental rain\r\n"
           "Drowning in this icy stream\r\n"
           "Living in a broken dream\r\n"
           "Cold mental, breaking down\r\n"
           "Lost in silence, no one around\r\n"
           "Cold mental, can't be found\r\n"
            "In this emptiness I'm bound";
    }
    else if (strcmp(songName, "Deer Death — Melt Into You") == 0)
    {
        return "Whispers in the fading light\r\n"
           "Slowly losing all my fight\r\n"
           "Your presence like a gentle tide\r\n"
           "Nowhere left for me to hide\r\n"
           "Burning embers, dying flame\r\n"
           "Calling out your sacred name\r\n"
           "Every boundary starts to blur\r\n"
           "Lost in what I can't deter\r\n"
           "Melt into you, become the same\r\n"
           "Set my weary soul aflame\r\n"
           "Melt into you, fade from view\r\n"
           "Until I'm nothing but with you\r\n"
           "Fingers tracing ancient lines\r\n"
           "Where your heartbeat merges with mine\r\n"
           "Breathing in your very essence\r\n"
           "Losing all my past presence\r\n"
           "The world outside begins to dim\r\n"
           "As I surrender every limb\r\n"
           "To the warmth that you provide\r\n"
           "In your current I will glide\r\n"
           "Melt into you, become the same\r\n"
           "Set my weary soul aflame\r\n"
           "Melt into you, fade from view\r\n"
           "Until I'm nothing but with you\r\n"
           "Skin and bone and memory\r\n"
           "All dissolve eternally\r\n"
           "No more you and no more me\r\n"
           "Just what we were meant to be\r\n"
           "Ocean deep and endless blue\r\n"
           "Finally becoming you\r\n"
           "All my edges soft and worn\r\n"
           "Like the breaking of the dawn\r\n"
           "Melt into you, become the same\r\n"
           "Set my weary soul aflame\r\n"
           "Melt into you, fade from view\r\n"
            "Until I'm nothing but with you";
    }
    else if (strcmp(songName, "Никто Не Против - Курю с призраками") == 0)
    {
        return "Сигаретный дым клубится в темноте\r\n"
           "Призраки прошлого шепчутся во мгле\r\n"
           "Одинокий вечер, тишина в квартире\r\n"
           "Вспоминаю тех, кого больше нет в мире\r\n"
           "Курю с призраками, разговариваю с тенями\r\n"
           "Ошибки молодости, мы стали чужими\r\n"
           "Курю с призраками, в дыму растворяюсь\r\n"
           "С каждым вдохом с прошлым прощаюсь\r\n"
           "Пепел опадает на старый паркет\r\n"
           "В памяти оживают былые сюжеты\r\n"
           "Тени улыбаются, жесты знакомые\r\n"
           "Разговоры о чём-то важном и простом\r\n"
           "Курю с призраками, разговариваю с тенями\r\n"
           "Ошибки молодости, мы стали чужими\r\n"
           "Курю с призраками, в дыму растворяюсь\r\n"
           "С каждым вдохом с прошлым прощаюсь\r\n"
           "Ночь длится бесконечно, время остановилось\r\n"
           "С каждым призраком стало так легко и горько\r\n"
           "Обещания, которые мы не сдержали\r\n"
           "Мечты, что навсегда в прошлом остались\r\n"
           "Курю с призраками, разговариваю с тенями\r\n"
           "Ошибки молодости, мы стали чужими\r\n"
           "Курю с призраками, в дыму растворяюсь\r\n"
           "С каждым вдохом с прошлым прощаюсь\r\n"
           "Рассвет приближается, призраки тают\r\n"
           "Вместе с сигаретным дымом исчезают\r\n"
           "Но знаю, что завтра снова наступят сумерки\r\n"
            "И мы продолжим наши тихие беседы";
    }

    return "Текст песни недоступен";
}

// Функция для получения пути к файлу
const char* GetSongFilePath(const char* songName)
{
    if (strcmp(songName, "Decalius - Loneliness") == 0)
    {
        const char* path = "E:\\Музыка\\Decalius - Loneliness.mp3";
        DWORD attr = GetFileAttributes(path);//GetFileAttributes - Проверяет существование файла
        if (attr == INVALID_FILE_ATTRIBUTES)    //Если файла не существует, возвращает - Invalid_file_attributes 
        {
            MessageBox(NULL, "Файл не найден!", "Ошибка", MB_OK | MB_ICONERROR);
            return NULL;
        }
        return path;
    }
    else if (strcmp(songName, "P.O.D. - Youth of the Nation") == 0) // Исправлено название
    {
        const char* path = "E:\\Музыка\\POD Youth_Of_The_Nation.mp3";
        DWORD attr = GetFileAttributes(path);
        if (attr == INVALID_FILE_ATTRIBUTES)
        {
            MessageBox(NULL, "Файл не найден!", "Ошибка", MB_OK | MB_ICONERROR);
            return NULL;
        }
        return path;
    }
    else if (strcmp(songName, "Reka — Life Long Tragedy") == 0) // Исправлено название
    {
        const char* path = "E:\\Музыка\\Reka — Life Long Tragedy.mp3";
        DWORD attr = GetFileAttributes(path);
        if (attr == INVALID_FILE_ATTRIBUTES)
        {
            MessageBox(NULL, "Файл не найден!", "Ошибка", MB_OK | MB_ICONERROR);
            return NULL;
        }
        return path;
    }
    else if (strcmp(songName, "takizava cold mental") == 0) // Исправлено название
    {
        const char* path = "E:\\Музыка\\takizava cold mental.mp3";
        DWORD attr = GetFileAttributes(path);
        if (attr == INVALID_FILE_ATTRIBUTES)
        {
            MessageBox(NULL, "Файл не найден!", "Ошибка", MB_OK | MB_ICONERROR);
            return NULL;
        }
        return path;
    }
    else if (strcmp(songName, "Deer Death — Melt Into You") == 0) // Исправлено название
    {
        const char* path = "E:\\Музыка\\Deer Death — Melt Into You.mp3";
        DWORD attr = GetFileAttributes(path);
        if (attr == INVALID_FILE_ATTRIBUTES)
        {
            MessageBox(NULL, "Файл не найден!", "Ошибка", MB_OK | MB_ICONERROR);
            return NULL;
        }
        return path;
    }
    else if (strcmp(songName, "Никто Не Против - Курю с призраками") == 0) // Исправлено название
    {
        const char* path = "E:\\Музыка\\Никто Не Против - Курю с призраками.mp3";
        DWORD attr = GetFileAttributes(path);
        if (attr == INVALID_FILE_ATTRIBUTES)
        {
            MessageBox(NULL, "Файл не найден!", "Ошибка", MB_OK | MB_ICONERROR);
            return NULL;
        }
        return path;
    }

    MessageBox(NULL, "Песня не найдена в базе!", "Ошибка", MB_OK | MB_ICONERROR);
    return NULL;
}

// Функция воспроизведения музыки
void PlayMusicFile(const char* filePath)
{
    // Определяю расширение файла
    const char* ext = strrchr(filePath, '.');   //strrchr - ищет последнее вхождение символа '.' (расширение файла)
    char type[32] = "mp3"; // по умолчанию для mp3
    if (ext != NULL)
    {
        if (_stricmp(ext, ".wav") == 0) //_stricmp - сравнение строк без учета регистра
            strcpy(type, "waveaudio");
        else if (_stricmp(ext, ".mp3") == 0 || _stricmp(ext, ".mpeg") == 0)
            strcpy(type, "mpegvideo");
        else if (_stricmp(ext, ".avi") == 0 || _stricmp(ext, ".wmv") == 0)
            strcpy(type, "avivideo");
    }
    char command[512];
    wsprintf(command, "open \"%s\" type %s alias mysong", filePath, type);  //wsprintf - форматирует строку (аналог sprintf для Windows)

    MCIERROR error = mciSendString(command, NULL, 0, NULL); //mciSendStringA - отправляет команду MCI (Media Control Interface)
    //https://learn.microsoft.com/ru-ru/windows/win32/multimedia/mci-reference
    if (error == 0)
    {
        error = mciSendString("play mysong", NULL, 0, NULL);
        if (error == 0)
        {
            MessageBox(NULL, "Воспроизведение начато!", "Успех", MB_OK | MB_ICONINFORMATION);
            return;
        }
    }
}

// Функция остановки музыки
void StopMusic()
{
    PlaySound(NULL, NULL, 0);
}