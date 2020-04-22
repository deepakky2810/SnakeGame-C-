#include<iostream>
#include<Windows.h>
#include<chrono>
#include<thread>
#include<set>
#include<list>
#include<time.h>


using namespace std;

int nScreenWidth = 100;
int nScreenHeight = 30;
int nFieldWidth = 80;
int nFieldHeight = 26;
int nXOffset = 2;
int nYOffset = 2;
wchar_t wcSnakeHead = 0x25BA;
wchar_t wcSnakeBody = 0x25A0;

typedef struct
{
	int position;
	char movementDir;
}locater;

bool isSnakeOut(int nCurrentSnakePos, set<int>* sSnake)
{
	if (sSnake->count(nCurrentSnakePos) != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void snakeLengthIncrementer(list<locater>* mSnake,int nSnakeLastBlockPos, char cSnakeLastBlockMovementDir, set<int>* sSnake)
{
	locater newLocater = { nSnakeLastBlockPos, cSnakeLastBlockMovementDir };
	mSnake->push_back(newLocater);
	sSnake->insert(nSnakeLastBlockPos);
}

void snakePositionUpdater(list<locater>* mSnake, int nCurrentSnakePosition, char cCurrenMovementDir, set<int>* sSnake, int* nSnakeLastBlockPos, char* cSnakeLastBlockMovementDir)
{
	*nSnakeLastBlockPos = mSnake->rbegin()->position;
	int temp = mSnake->rbegin()->position;
	*cSnakeLastBlockMovementDir = mSnake->rbegin()->movementDir;
	for (auto itr1 = next(mSnake->rbegin(), 1), itr2 = mSnake->rbegin(); itr1 != mSnake->rend(); itr1++, itr2++)
	{
		itr2->position = itr1->position;
		itr2->movementDir = itr1->movementDir;
	}
	mSnake->begin()->position = nCurrentSnakePosition;
	mSnake->begin()->movementDir = cCurrenMovementDir;

	sSnake->erase(temp);
	sSnake->insert(nCurrentSnakePosition);
}

int foodGenerator(set<int>* sSnake, bool* bFoodEaten)
{
	int nFoodPosX = rand() % (nFieldWidth - 2);
	int nFoodPosY = rand() % (nFieldHeight - 2);
	int nFoodPosOnScreen = (nFoodPosY + nYOffset + 1) * nScreenWidth + (nFoodPosX + nXOffset + 1);
	while (sSnake->count(nFoodPosOnScreen) != 0)
	{
		nFoodPosX = rand() % (nFieldWidth - 2);
		nFoodPosY = rand() % (nFieldHeight - 2);
		nFoodPosOnScreen = (nFoodPosY + nYOffset + 1) * nScreenWidth + (nFoodPosX + nXOffset + 1);
	}

	*bFoodEaten = false;
	return nFoodPosOnScreen;
}


void changeDirAccToInput(char* cCurrentMovementDir, float* fpSnakeSpeedX, float* fpSnakeSpeedY)
{
	if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
	{
		*cCurrentMovementDir = 'R';
		wcSnakeHead = 0x25BA;
	}
	if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
	{
		*cCurrentMovementDir = 'L';
		wcSnakeHead = 0x25C4;
	}
	if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
	{
		*cCurrentMovementDir = 'U';
		wcSnakeHead = 0x25B2;
	}
	if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
	{
		*cCurrentMovementDir = 'D';
		wcSnakeHead = 0x25BC;
	}

	float speed = 2.0f;
	switch (*cCurrentMovementDir)
	{
	case 'R':*fpSnakeSpeedX = speed; *fpSnakeSpeedY = 0.0f; break;
	case 'L':*fpSnakeSpeedX = -speed; *fpSnakeSpeedY = 0.0f; break;
	case 'U':*fpSnakeSpeedX = 0.0f; *fpSnakeSpeedY = -speed; break;
	case 'D':*fpSnakeSpeedX = 0.0f; *fpSnakeSpeedY = speed; break;
	//default :*fpSnakeSpeedX = 0.0f; *fpSnakeSpeedY = 0.0f; break;
	}
}
int main()
{
	srand(time(0));

	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	 

	bool bGameOver = false;
	int nSnakeLength = 0;
	float fSnakeX = nXOffset + nFieldWidth / 2;
	float fSnakeY = nYOffset + nFieldHeight / 2;
	char cCurrentMovementDir = '\0';
	int nCurrentSnakePos = fSnakeY * nScreenWidth + fSnakeX;
	int nPrevSnakePos = nCurrentSnakePos;
	float fSnakeSpeedX = 0.0f;
	float fSnakeSpeedY = 0.0f;
	list<locater> mSnake;
	set<int> sSnake;
	bool bFoodEaten = true;
	int nFoodPosOnScreen;
	int nSnakeLastBlockPos = nCurrentSnakePos;
	char cSnakeLastBlockMovementDir = cCurrentMovementDir;
	int nScore = 0;


	snakeLengthIncrementer(&mSnake, nSnakeLastBlockPos, cSnakeLastBlockMovementDir, &sSnake);

	//Time stamps
	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();
	while (!bGameOver)
	{
		for (int i = 0; i < nScreenHeight; i++)
		{
			for (int j = 0; j < nScreenWidth; j++)
			{
				screen[i * nScreenWidth + j] = L' ';
			}
		}

		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();

		changeDirAccToInput(&cCurrentMovementDir, &fSnakeSpeedX, &fSnakeSpeedY);

		fSnakeX += fSnakeSpeedX * 6.2f * fElapsedTime;
		fSnakeY += fSnakeSpeedY * 2.2f * fElapsedTime;

		if ((int)fSnakeX >= nFieldWidth + nXOffset - 1)
		{
			fSnakeX = nXOffset + 1;
		}
		if ((int)fSnakeX <= nXOffset)
		{
			fSnakeX = nFieldWidth + nXOffset - 2;
		}

		if ((int)fSnakeY <= nYOffset)
		{
			fSnakeY = nFieldHeight + nYOffset - 2;
		}
		if ((int)fSnakeY >= nFieldHeight + nYOffset - 1)
		{
			fSnakeY = nYOffset + 1;
		}
		

		nCurrentSnakePos = (int)fSnakeY * nScreenWidth + (int)fSnakeX;

		if (nCurrentSnakePos != nPrevSnakePos)
		{
			if (isSnakeOut(nCurrentSnakePos, &sSnake))
			{
				bGameOver = true;
			}
			//updates the position of each block of the snake.
			snakePositionUpdater(&mSnake, nCurrentSnakePos, cCurrentMovementDir, &sSnake, &nSnakeLastBlockPos, &cSnakeLastBlockMovementDir);
		}
		

		nPrevSnakePos = nCurrentSnakePos;
		
		if (bFoodEaten == true)
		{
			nFoodPosOnScreen = foodGenerator(&sSnake, &bFoodEaten);
		}
		if (nFoodPosOnScreen == nCurrentSnakePos)//Snake has eaten the food
		{										 //foodEaten should be set, and length of snake 
			bFoodEaten = true;					 //should increase.
			nScore++;
			snakeLengthIncrementer(&mSnake,nSnakeLastBlockPos, cSnakeLastBlockMovementDir, &sSnake);
		}

		
		//Rendering the food
		screen[nFoodPosOnScreen] = 0x25CF;

		//Rendering the snake's body
		for (auto itr = mSnake.begin(); itr != mSnake.end(); itr++)
		{
			if (itr == mSnake.begin())
			{
				screen[itr->position] = wcSnakeHead;
			}
			else
			{
				screen[itr->position] = L'#';//wcSnakeBody;
			}
		}

		//Rendering the score
		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + nXOffset + 4], 12, L"SCORE: %4d", nScore);
		//rendering the boundary of the playing area or field
		for (int y = 0; y < nFieldHeight; y++)
		{
			for (int x = 0; x < nFieldWidth; x++)
			{
				if(x == 0 || x == nFieldWidth - 1 || y == 0 || y == nFieldHeight - 1)
				screen[(y + nYOffset) * nScreenWidth + (x + nXOffset)] = 0x2588;
			}
		}

		screen[nScreenWidth * nScreenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

		if (bGameOver == true)
		{
			int time_seconds = 5;
			this_thread::sleep_for(chrono::seconds(time_seconds));
		}
	}

	cout << "Your Score: " << nScore<<"\n\n\n";
	
}