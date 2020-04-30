#include<iostream>
#include<Windows.h>
#include<chrono>
#include<thread>
#include<set>
#include<list>
#include<time.h>

/*
	IMPORTANT!!!!
	~~~~~~~~~~~~~
	READ ME BEFORE RUNNING!!! This program expects the console dimensions to be set to
	100 Columns by 30 Rows. I recommend a small font "Consolas" at size 16. You can do this
	by running the program, and right clicking on the console title bar, and specifying
	the properties. You can also choose to default to them in the future.

	Controls: A / Left Arrow = Left, D / Right Arrow = Right, W / Up Arrow = Up, S / Down Arrow = Down
	CONTACT:
	Email : deepakky2810@gmail.com
	LinkedIn : https://www.linkedin.com/in/deepak-kumar-yadav-149932169
	Last updated on : 22/04/2020
*/


using namespace std;

//-------------GLOBAL VARIABLE DECLARATIONS------------------------//
int nScreenWidth = 100; //Width of the window in character cells
int nScreenHeight = 30; //Height of the window in character cells
int nFieldWidth = 80; //Width of the playing area(field) in character cells
int nFieldHeight = 26; //Height of the playing area(field) in character cells
int nXOffset = 2; //Offset in x - direction from the left edge of the screen
int nYOffset = 2; //Offset in y - direction from the top edge of the screen
wchar_t wcSnakeHead = 0x25BA; //Unicode character which will represent the head of the snake
wchar_t wcSnakeBody = L'#'; //Unicode character which will represent rest of the body of snake


//Structure which stores the position and movement direction of each cell of snake's body
typedef struct
{
	int position;
	char movementDir;
}snakePositionLocater;

//Method to determine whether snake has bitten itself(is out or not)
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

//Increments the length of the snake(or adds a new cell to the snake body)
void snakeLengthIncrementer(list<snakePositionLocater>* mSnake, int nSnakeLastBlockPos, char cSnakeLastBlockMovementDir, set<int>* sSnake, int* nSnakeCurrentLength)
{
	snakePositionLocater newsnakePositionLocater = { nSnakeLastBlockPos, cSnakeLastBlockMovementDir };
	mSnake->push_back(newsnakePositionLocater); //Add a new block at the end of snake's body
	sSnake->insert(nSnakeLastBlockPos); //Adds that position to the s
	(*nSnakeCurrentLength)++; //Increment the length
}

//Updates the position and movement direction of each of the cell of snake after each time the position of the snake's head changes 
void snakePositionUpdater(list<snakePositionLocater>* mSnake, int nCurrentSnakePosition, char cCurrenMovementDir, set<int>* sSnake, int* nSnakeLastBlockPos, char* cSnakeLastBlockMovementDir)
{
	*nSnakeLastBlockPos = mSnake->rbegin()->position; //Store the last block's position will be used in the snake length incrementer() while adding a new block at the end
	*cSnakeLastBlockMovementDir = mSnake->rbegin()->movementDir; //Same as above
	for (auto itr1 = next(mSnake->rbegin(), 1), itr2 = mSnake->rbegin(); itr1 != mSnake->rend(); itr1++, itr2++)
	{
		itr2->position = itr1->position;
		itr2->movementDir = itr1->movementDir;
	}	//Update the position and direction of each block with the block next to it.
	mSnake->begin()->position = nCurrentSnakePosition;
	mSnake->begin()->movementDir = cCurrenMovementDir; //Update the position and direction of first block(snake's head) with the new block that the snake has moved to

	sSnake->erase(*nSnakeLastBlockPos); //Delete the last block position from the position storing set
	sSnake->insert(nCurrentSnakePosition); //Insert the position of new block that the snake has moved to
}

//Generates the food for the snake, if the generated food lies on the snake body it regenerates the food until the food position doesn't lie on the snake body
int foodGenerator(set<int>* sSnake, bool* bFoodEaten)
{
	int nFoodPosX = rand() % (nFieldWidth - 2);
	int nFoodPosY = rand() % (nFieldHeight - 2);
	int nFoodPosOnScreen = (nFoodPosY + nYOffset + 1) * nScreenWidth + (nFoodPosX + nXOffset + 1); //Generate the food
	while (sSnake->count(nFoodPosOnScreen) != 0) //Generate food until the position of food does not lie on the snake's body
	{
		nFoodPosX = rand() % (nFieldWidth - 2);
		nFoodPosY = rand() % (nFieldHeight - 2);
		nFoodPosOnScreen = (nFoodPosY + nYOffset + 1) * nScreenWidth + (nFoodPosX + nXOffset + 1);
	}

	*bFoodEaten = false; //When new food is created foodEaten should become false
	return nFoodPosOnScreen;
}

//It changes the direction of the snake's head each time a valid key is pressed
void changeDirAccToInput(char* cCurrentSnakeMovementDir, float* fpSnakeSpeedX, float* fpSnakeSpeedY, int nSnakeCurrentLength)
{
	wchar_t wcPrevSnakeHead = wcSnakeHead;
	char cPrevSnakeMovementDir = *cCurrentSnakeMovementDir;
	
	//Check for inputs from keyboard
	if (GetAsyncKeyState((unsigned short)'D') & 0x8000 || GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		*cCurrentSnakeMovementDir = 'R';
		wcSnakeHead = 0x25BA;
	}
	if (GetAsyncKeyState((unsigned short)'A') & 0x8000 || GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		*cCurrentSnakeMovementDir = 'L';
		wcSnakeHead = 0x25C4;
	}
	if (GetAsyncKeyState((unsigned short)'W') & 0x8000 || GetAsyncKeyState(VK_UP) & 0x8000)
	{
		*cCurrentSnakeMovementDir = 'U';
		wcSnakeHead = 0x25B2;
	}
	if (GetAsyncKeyState((unsigned short)'S') & 0x8000 || GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		*cCurrentSnakeMovementDir = 'D';
		wcSnakeHead = 0x25BC;
	}

	if (((*cCurrentSnakeMovementDir == 'R' && cPrevSnakeMovementDir == 'L') || (*cCurrentSnakeMovementDir == 'L' && cPrevSnakeMovementDir == 'R') || (*cCurrentSnakeMovementDir == 'U' && cPrevSnakeMovementDir == 'D') || (*cCurrentSnakeMovementDir == 'D' && cPrevSnakeMovementDir == 'U')) && nSnakeCurrentLength > 1) //If snake has more than one block and it tries to move in the strict reverse of the current direction doesn't allow it, else it will bite itself and also in real world it is not possible
	{
		*cCurrentSnakeMovementDir = cPrevSnakeMovementDir;
		wcSnakeHead = wcPrevSnakeHead;
	}
	else //Else update the movement of direction according to input
	{
		float speed = 2.0f;
		switch (*cCurrentSnakeMovementDir)
		{
		case 'R':*fpSnakeSpeedX = speed; *fpSnakeSpeedY = 0.0f; break;
		case 'L':*fpSnakeSpeedX = -speed; *fpSnakeSpeedY = 0.0f; break;
		case 'U':*fpSnakeSpeedX = 0.0f; *fpSnakeSpeedY = -speed; break;
		case 'D':*fpSnakeSpeedX = 0.0f; *fpSnakeSpeedY = speed; break;
		}
	}
}
int main()
{
	srand(time(0)); //It seeds the random generator with fresh values


	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight]; //The 1D array which represents our 2D screen, it emulates a screen of nScreenWidth columns, and nScreenHeight rows
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL); //Creates the screen buffer and generates a handle to it(Windows console API function)
	SetConsoleActiveScreenBuffer(hConsole); //Sets the specified screen buffer to be the currently displayed console screen buffer(Windows console API function)


	DWORD dwBytesWritten = 0;// Required later in another console API function


	//Initial values of variables concerned with snake and gameplay
	bool bGameOver = false; //Game is over or not
	int nSnakeCurrentLength = 0; //Current length of the snake
	float fSnakeX = nXOffset + nFieldWidth / 2; //X position of snake's head
	float fSnakeY = nYOffset + nFieldHeight / 2; //Y position of snake's head
	char cCurrentSnakeMovementDir = '\0'; //Current movement direction of the snake's head
	int nCurrentSnakePos = fSnakeY * nScreenWidth + fSnakeX; //Current position(the index) of the snake on the screen(in screen array)
	int nPrevSnakePos = nCurrentSnakePos; //Previous position(the index) of the snake on the screen(in screen array)
	float fSnakeSpeedX = 0.0f; //Speed of snake in X direction
	float fSnakeSpeedY = 0.0f; //Speed of snake in Y direction
	list<snakePositionLocater> mSnake; //Doubly linked list which holds the position and movement direction of each cell of the snake's body
	set<int> sSnake; //Set to store the position of each cell of the snake's body(This will be used at the time of food generation, to check whether the generated food lies on the body or not)
	bool bFoodEaten = true; //Whether food has been eaten or not
	int nFoodPosOnScreen; //Position(the index) of food on screen(in screen array)
	int nSnakeLastBlockPos = nCurrentSnakePos; //Position(the index) of last block of snake on screen(in screen array)
	char cSnakeLastBlockMovementDir = cCurrentSnakeMovementDir; //Movement direction of last block of snake.
	int nScore = 0; //Game score
	int nLevel = 1; //Game level
	int nPointsReqToIncLevel = 5; //Point required to increase the level

	//To make the snake's head
	snakeLengthIncrementer(&mSnake, nSnakeLastBlockPos, cSnakeLastBlockMovementDir, &sSnake, &nSnakeCurrentLength);

	//Time points for calculating the elapsed time of each iteration of the game loop
	auto currentTime = chrono::system_clock::now();
	auto prevTime = chrono::system_clock::now();

	//Game loop
	while (!bGameOver)
	{
		//Make the whole screen blank(initialize the whole screen array with space)
		for (int i = 0; i < nScreenHeight; i++)
		{
			for (int j = 0; j < nScreenWidth; j++)
			{
				screen[i * nScreenWidth + j] = L' ';
			}
		}

		currentTime = chrono::system_clock::now(); //Take the current time
		chrono::duration<float> elapsedTime = currentTime - prevTime; //Find the elapsed time
		prevTime = currentTime;//Update the previous time with the current time(which will bw the previous time for the next iteration)
		float fElapsedTime = elapsedTime.count(); //Getting the elapsed time in float value

		//------------------------------------------------LOGIC PART----------------------------------------------//

		//Update the direction of snake according to the user input
		changeDirAccToInput(&cCurrentSnakeMovementDir, &fSnakeSpeedX, &fSnakeSpeedY, nSnakeCurrentLength);

		//Position = speed * time * (constant value) //In physics constant value is 1, but here to make the rate of change of position in both directions look equal, it is maually adjusted, it can be changed as neccessary.
		fSnakeX += fSnakeSpeedX * 6.2f * fElapsedTime; //Calculating the X - position of the snake
		fSnakeY += fSnakeSpeedY * 2.2f * fElapsedTime; //Calculating the Y - position of the snake

		//Checking if the X - position of the snake has gone out of bounds(i.e beyond field width)
		if ((int)fSnakeX >= nFieldWidth + nXOffset - 1)
		{
			fSnakeX = nXOffset + 1;
		}
		if ((int)fSnakeX <= nXOffset)
		{
			fSnakeX = nFieldWidth + nXOffset - 2;
		}

		//Checking if the Y - position of the snake has gone out of bounds(i.e beyond field height)
		if ((int)fSnakeY <= nYOffset)
		{
			fSnakeY = nFieldHeight + nYOffset - 2;
		}
		if ((int)fSnakeY >= nFieldHeight + nYOffset - 1)
		{
			fSnakeY = nYOffset + 1;
		}

		//Calculating the current snake position(in the array) on the screen(in screen array) from the X and Y positions of the screen
		nCurrentSnakePos = (int)fSnakeY * nScreenWidth + (int)fSnakeX;

		//Checking if the snake has moved or not, if it has moved than it's position should be updated, also it should be checked at this time that whether it has bitten itself or not(whether the new movement has caused thes snake's head to overlap with it's body)
		if (nCurrentSnakePos != nPrevSnakePos)
		{
			if (isSnakeOut(nCurrentSnakePos, &sSnake)) //Check if snake is out or not
			{
				bGameOver = true;
			}
			//updates the position of each block of the snake.
			snakePositionUpdater(&mSnake, nCurrentSnakePos, cCurrentSnakeMovementDir, &sSnake, &nSnakeLastBlockPos, &cSnakeLastBlockMovementDir); //Update the position of the snake on screen
		}

		//Updating the previous snake position(the current snake position in this iteration will be the prev snake position in the next iteration)
		nPrevSnakePos = nCurrentSnakePos;

		//If food has been eaten, generate the new food
		if (bFoodEaten == true)
		{
			nFoodPosOnScreen = foodGenerator(&sSnake, &bFoodEaten);
		}
		//If the current snake position overlaps with the food position, means the food has been eaten
		if (nFoodPosOnScreen == nCurrentSnakePos)//If snake has eaten the food
		{
			bFoodEaten = true; // foodEaten should be set
			nScore++; //Score should increase
			if (nScore % nPointsReqToIncLevel == 0) //When score increases by nPointsReqToIncLevel points increase the level.
			{
				nLevel++;
			}
			snakeLengthIncrementer(&mSnake, nSnakeLastBlockPos, cSnakeLastBlockMovementDir, &sSnake, &nSnakeCurrentLength); //Length of snake should increase
		}

		//------------------------------------------------RENDERING PART--------------------------------------//
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
				screen[itr->position] = wcSnakeBody;
			}
		}
		if (bGameOver == true)
		{
			screen[nCurrentSnakePos] = wcSnakeHead;
			swprintf_s(&screen[34], 34, L"GAME OVER: NEXT TIME TRY HARDER!!");
		}

		//Rendering the score
		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + nXOffset + 4], 12, L"SCORE: %4d", nScore);

		//Rendering the level
		swprintf_s(&screen[(2 + 1) * nScreenWidth + nFieldWidth + nXOffset + 4], 12, L"LEVEL: %4d", nLevel);

		//Rendering the boundary of the playing area or field
		for (int y = 0; y < nFieldHeight; y++)
		{
			for (int x = 0; x < nFieldWidth; x++)
			{
				if (x == 0 || x == nFieldWidth - 1 || y == 0 || y == nFieldHeight - 1)
					screen[(y + nYOffset) * nScreenWidth + (x + nXOffset)] = 0x2588;
			}
		}

		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten); //Copies a number of characters(in this case the screen[] array) to consecutive cells of a console screen buffer, beginning at a specified location((0, 0) here) (Windows API console functions)

		//If game is over then pause for five seconds and return to main function
		if (bGameOver == true)
		{
			int time_seconds = 5;
			this_thread::sleep_for(chrono::seconds(time_seconds));
		}
	}

	//Display score and level
	cout << "\t\t\t\t\tYour Score: " << nScore << "\n";
	cout << "\t\t\t\t\tYour Level: " << nLevel << "\n\n\n\n";

	return 0;
}

