#include<windows.h>
#include<vector>
#include<ctime>

#define CELL 20
#define WIDTH 30
#define HEIGHT 20

struct Point { int x, y; };
std::vector<Point> snake;
Point food;
int dir = 1;
int score = 0;
bool dead = false;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInst;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszClassName = "Snake";
}