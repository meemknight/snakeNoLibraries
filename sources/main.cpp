#include <console.h>
#include <types.h>
#include <bufferedVector.h>
#include <randomNumbers.h>
#include <font8x8_basic.h>

#define NOMINMAX
#include <Windows.h>

bool running = true;

int windowW;
int windowH;

void *windowBuffer = 0;
BITMAPINFO bitmapInfo = {};

constexpr int gameSizeX = 10;
constexpr int gameSizeY = 10;

int blockSize = 70;

struct Point
{
	int x = 0;
	int y = 0;
};


void resizeWindowBuffer(HWND wind)
{

	RECT rect = {};
	GetClientRect(wind, &rect);

	windowW = rect.right;
	windowH = rect.bottom;

	if (windowBuffer)
	{
		VirtualFree(windowBuffer, 0, MEM_RELEASE);
	}

	windowBuffer = VirtualAlloc(0, 4 * windowW * windowH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFO);
	bitmapInfo.bmiHeader.biWidth = windowW;
	bitmapInfo.bmiHeader.biHeight = -windowH;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 32;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;
}

LRESULT windProc(HWND wind, UINT msg, WPARAM wp, LPARAM lp)
{
	LRESULT rez = 0;


	switch (msg)
	{

	case WM_CLOSE:
		running = false;
		break;

	case WM_SIZE:
		resizeWindowBuffer(wind);
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT Paint = {};
		HDC DeviceContext = BeginPaint(wind, &Paint);

		HDC hdc = GetDC(wind);

		StretchDIBits(hdc,
			0, 0, windowW, windowH,
			0, 0, windowW, windowH,
			windowBuffer,
			&bitmapInfo,
			DIB_RGB_COLORS,
			SRCCOPY
		);

		ReleaseDC(wind, hdc);

		EndPaint(wind, &Paint);

	}break;

	default:
		rez = DefWindowProc(wind, msg, wp, lp);
	break;
	}

	return rez;
}


int main()
{
	
	//consoleWrite("hello");

#pragma region create window

	WNDCLASS wc = {};

	HINSTANCE h = GetModuleHandle(0);

	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hInstance = h;
	wc.lpfnWndProc = windProc;
	wc.lpszClassName = "MainWindowClass";
	wc.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wc);

	HWND wind = CreateWindow(
		wc.lpszClassName,
		"Snak",
		WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		gameSizeX * blockSize,
		gameSizeY * blockSize + 20,
		0, 0, h, 0
	);

#pragma endregion

	resizeWindowBuffer(wind);

	ShowWindow(wind, SW_SHOW);


	auto drawPixel = [&](int x, int y,
		unsigned char r, unsigned char g, unsigned char b
		)
	{
		if (x >= 0 && y >= 0 && x < windowW && y < windowH)
		{
			((char *)windowBuffer)[(y * windowW + x) * 4 + 0] = b;
			((char *)windowBuffer)[(y * windowW + x) * 4 + 1] = g;
			((char *)windowBuffer)[(y * windowW + x) * 4 + 2] = r;
		}
	};
	
	auto drawRect = [&](int x, int y, int w, int h,
		unsigned char r, unsigned char g, unsigned char b)
	{
		for (int j = y; j < h+y; j++)
			for (int i = x; i < w+x; i++)
			{
				drawPixel(i, j, r, g, b);
			}
	};

	auto drawGliph = [&](int x, int y, char c, int scale,
		unsigned char r, unsigned char g, unsigned char b)
	{
		if (c >= 128)
		{
			return;
		}

		char letter[8][8] = {};
		char *font = font8x8_basic[c];

		for (int j = 0; j < 8; j++)
		{
			char line = font[j];
			
			for (int i = 0; i < 8; i++)
			{
				if ((line & 0b1) == 1)
				{
					letter[i][j] = 1;
				}
				line >>= 1;
			}
		}

		for(int yy=0; yy<8; yy++)
			for (int xx = 0; xx < 8; xx++)
			{
				if(letter[xx][yy])
				drawRect(xx*scale+x, yy*scale+y, scale, scale, r, g, b);

			}

	};

	auto drawText = [&](int x, int y, char *c, int scale,
		unsigned char r, unsigned char g, unsigned char b)
	{
		while (*c != 0)
		{
			drawGliph(x, y, *c, scale, r, g, b);

			x += scale * 10;
			c++;
		}
	};


	LARGE_INTEGER time1 = {};
	LARGE_INTEGER time2 = {};
	QueryPerformanceCounter(&time1);

	uint32_t ticks = 0;
	const uint32_t tickSpeed = 2'000'000;

#pragma region game logic

	BufferedVector<Point, gameSizeX *gameSizeY> snake;
	Point apple = {};

	snake.push_back({5, 5});


#pragma endregion

	int dirX = 0;
	int dirY = 0;

	int lastDirX = 0;
	int lastDirY = 0;


	auto spawnApple = [&]()
	{
		while (true)
		{
			apple.x = getRandomNumber(0, gameSizeX);
			apple.y = getRandomNumber(0, gameSizeY);

			bool good = true;

			for (int i = 0; i < snake.size; i++)
			{
				if (apple.x == snake.get(i).x
					&& apple.y == snake.get(i).y
					)
				{
					good = false;
					break;
				}
			}

			if (good)
			{
				break;
			}
		}

	};

	spawnApple();

	while (running)
	{
	#pragma region time
		bool tick = false;
		{
			QueryPerformanceCounter(&time2);
			LARGE_INTEGER deltaTimeInteger;
			deltaTimeInteger.QuadPart = time2.QuadPart - time1.QuadPart;
			QueryPerformanceCounter(&time1);
		
			ticks += deltaTimeInteger.QuadPart;

			if (ticks >= tickSpeed)
			{
				ticks -= tickSpeed;
				tick = true;
			}
		}
	#pragma endregion


		if (tick)
		{
			auto oldSnakPos = snake.last();
			
			for (int i = 0; i < snake.size-1; i++)
			{
				snake.get(i) = snake.get(i+1);
			}

			snake.last().x += dirX;
			snake.last().y += dirY;

			lastDirX = dirX;
			lastDirY = dirY; 

			if (snake.last().x >= gameSizeX)
			{
				snake.last().x = 0;
			}

			if (snake.last().y >= gameSizeX)
			{
				snake.last().y = 0;
			}

			if (snake.last().x < 0)
			{
				snake.last().x = gameSizeX - 1;
			}

			if (snake.last().y < 0)
			{
				snake.last().y = gameSizeY - 1;
			}

			for (int i = 0; i < snake.size - 1; i++)
			{
				if (
					snake.get(i).x == snake.last().x &&
					snake.get(i).y == snake.last().y
					)
				{
					//game over
					return 0;
				}
			}

			if (apple.x == snake.last().x
				&& apple.y == snake.last().y
				)
			{
				spawnApple();
				snake.push_back({});

				for (int i = snake.size - 1; i > 0; i--)
				{
					snake.get(i) = snake.get(i - 1);
				}

				snake.get(0) = snake.get(1);
			}

		}

		if (GetAsyncKeyState(VK_UP) && lastDirY != 1)
		{
			dirX = 0;
			dirY = -1;
		}
		else if (GetAsyncKeyState(VK_LEFT) && lastDirX != 1)
		{
			dirX = -1;
			dirY = 0;
		}
		else if (GetAsyncKeyState(VK_RIGHT) && lastDirX != -1)
		{
			dirX = 1;
			dirY = 0;
		}
		else if (GetAsyncKeyState(VK_DOWN) && lastDirY != -1)
		{
			dirX = 0;
			dirY = 1;
		}


	#pragma region clear screen
		for (int x = 0; x < windowH * windowW * 4; x++)
		{
			((char *)windowBuffer)[x] = 0;
		}
	#pragma endregion

		for (int i = 0; i < snake.size; i++)
		{
			auto [x, y] = snake.get(i);

			if ((x + y) % 2)
			{
				drawRect(
					x * blockSize, y * blockSize,
					blockSize, blockSize, 70, 200, 25);
			}
			else
			{
				drawRect(
					x * blockSize, y * blockSize,
					blockSize, blockSize, 25, 200, 70);
			}
		}

		drawRect(
			apple.x * blockSize, apple.y * blockSize,
			blockSize, blockSize, 200, 70, 25);

		char s = '0';
		s += snake.size - 1;

		drawText(windowW >> 1, 20, "Score:", 4, 90, 82, 70);
		drawGliph(windowW -40, 20, s, 4, 90, 82, 70);

		//drawGliph(windowW >> 1, 20, 'S', 1, 90, 82, 70);

		//win state
		if (snake.size >= 10)
		{
			return 0;
		}


		MSG msg = {};
		while (PeekMessage(&msg, wind, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//draw screen
		{
			RECT r;
			GetClientRect(wind, &r);
			int w = r.left;
			int h = r.bottom;
			HDC hdc = GetDC(wind);
			PatBlt(hdc, 0, 0, w, h, BLACKNESS);
			ReleaseDC(wind, hdc);
			SendMessage(wind, WM_PAINT, 0, 0);
		}


	}

	CloseWindow(wind);

	
}