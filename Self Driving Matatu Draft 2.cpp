#include <iostream>
using namespace std;
#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <ctime>

using namespace std;

// declare obstacle variables
int enemyX[3];
int enemyY[3];
int enemyFlag[3];

// These values come from the main project
#define SCREEN_HEIGHT 26

// This function already exists in the main project.

void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Generate a random position for an obstacle
void genEnemy(int index)
{
    enemyX[index] = 17 + rand() % 33;
}

// Drawing an obstacle
void drawEnemy(int index)
{
    if(enemyFlag[index])
    {
        gotoxy(enemyX[index], enemyY[index]);     cout << "####";
        gotoxy(enemyX[index], enemyY[index] + 1); cout << " ## ";
        gotoxy(enemyX[index], enemyY[index] + 2); cout << "####";
        gotoxy(enemyX[index], enemyY[index] + 3); cout << " ## ";
    }
}

// Erase an obstacle
void eraseEnemy(int index)
{
    if(enemyFlag[index])
    {
        gotoxy(enemyX[index], enemyY[index]);     cout << "    ";
        gotoxy(enemyX[index], enemyY[index] + 1); cout << "    ";
        gotoxy(enemyX[index], enemyY[index] + 2); cout << "    ";
        gotoxy(enemyX[index], enemyY[index] + 3); cout << "    ";
    }
}

// Move the obstacle down
void moveEnemy(int index)
{
    if(enemyFlag[index])
    {
        enemyY[index]++;
    }
}

// Reset the obstacle when it leaves the screen
void resetEnemy(int index)
{
    eraseEnemy(index);
    enemyY[index] = 1;
    genEnemy(index);
}

// Check if the obstacle has reached the bottom
void updateEnemy(int index)
{
    moveEnemy(index);

    if(enemyY[index] > SCREEN_HEIGHT - 4)
    {
        resetEnemy(index);
    }
}

int main()
{
    srand(time(0));

    enemyFlag[0] = 1;
    enemyY[0] = 1;

    genEnemy(0);

    while (true)
    {
        eraseEnemy(0);
        updateEnemy(0);
        drawEnemy(0);

        Sleep(100);
    }

    return 0;
}

void changeLane(string direction){
}

void speedUp() {
}

void brakeMatatu() {


void userInput (char input) {
  switch (input)
    case 'a':
        changeLane("left");
        break;
    case 'd':
        changeLane("right");
        break;
    case 'w':
        speedUp();
        break;
    case 's':
        brakeMatatu();
        break;
    default:
        break;
}


