#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <conio.h>

using namespace std;

// =====================
// SETTINGS
// =====================
#define SCREEN_TOP 3
#define SCREEN_HEIGHT 23
#define SCREEN_WIDTH 90
#define WIN_WIDTH 50

int laneX[3] = {20, 32, 44};
int spawnCounter = 0;
bool active[3] = {false, false,false};


// =====================
// GOTXOY (DECLARE FIRST)
// =====================
void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// =====================
// DISPLAY SYSTEM
// =====================

//hide the cursor
void hideCursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;  

    SetConsoleCursorInfo(consoleHandle, &info);
}


// draw lanes
void drawRoad()
{
    for (int i = 3; i < 25; i++)
    {
        gotoxy(18, i); cout << "|";
        gotoxy(30, i); cout << "|";
        gotoxy(42, i); cout << "|";
        gotoxy(54, i); cout << "|";
    }
}

//draws border
void drawBoundary(){  
	for(int i=3; i<SCREEN_HEIGHT; i++){
		for(int j=0; j<17; j++){
			gotoxy(0+j,i); cout<<"�";
			gotoxy(SCREEN_WIDTH-j,i); cout<<"�";
		}
	} 
	for(int i=3; i<24; i++){
		gotoxy(SCREEN_WIDTH , i); cout<<"�";
	} 
}

// =====================
// MATATU CLASS (CLEAN)
// =====================
class Matatu
{
private:
    int lane;   // 0 left, 1 middle, 2 right
    int y;

public:
    Matatu()
    {
        lane = 1;
        y = 23;
    }

    void changeLane(string direction)
    {
        if (direction == "left" && lane > 0)
            lane--;
        else if (direction == "right" && lane < 2)
            lane++;
    }

    int getLane() { return lane; }
    int getY() { return y; }

    void draw()
    {
        gotoxy(laneX[lane], y);
        cout << "🚐";
    }

    void erase()
    {
        gotoxy(laneX[lane], y);
        cout << " ";
    }
};

// =====================
// OBSTACLES
// =====================
int enemyLane[3];
int enemyY[3];

//generates an obstacle randomly
void genEnemy(int i)
{
    int newLane;
    bool laneTaken;

    // prevent same lane overlap
    do
    {
        newLane = rand() % 3;
        laneTaken = false;

        for (int j = 0; j < 3; j++)
        {
            if (j != i && active[j] && enemyLane[j] == newLane)
            {
                laneTaken = true;
                break;
            }
        }
    }
    while (laneTaken);

    enemyLane[i] = newLane;

    // prevent same Y overlap
    bool yTooClose;

    do
    {
        yTooClose = false;

        int newY = -(rand() % 10 + 5);

        for (int j = 0; j < 3; j++)
        {
            if (j != i && active[j] && abs(newY - enemyY[j]) < 5)
            {
                yTooClose = true;
                break;
            }
        }

        if (!yTooClose)
        {
            enemyY[i] = newY;
        }

    } while (yTooClose);

    active[i] = true;
}	

//draws the enemy
void drawEnemy(int i)
{
    if (!active[i] || enemyY[i] < SCREEN_TOP) return;

    gotoxy(laneX[enemyLane[i]], enemyY[i]);
    cout << "🚘";
}

//erases the enemy after it moves down in the lane to avoid a trail
void eraseEnemy(int i)
{
    if (!active[i]) return;

    gotoxy(laneX[enemyLane[i]], enemyY[i]);
    cout << " ";
}


void updateEnemy(int i)
{
    if (!active[i]) return;

    enemyY[i]++;

    if (enemyY[i] > SCREEN_HEIGHT)
    {
        active[i] = false;  
    }
}

// =====================
// INPUT
// =====================

//If user interface, allows movement of the matatu manually
void userInput(char key, Matatu &m)
{
    if (key == 'a') m.changeLane("left");
    if (key == 'd') m.changeLane("right");
    
}

// =====================
// AI
// =====================
bool detectObstacleAhead(Matatu &m)
{
    for (int i = 0; i < 3; i++)
    {
        if (active[i] &&
            enemyLane[i] == m.getLane() &&
            abs(enemyY[i] - m.getY()) < 5)
        {
            return true;
        }
    }
    return false;
}


int laneSafetyScore(int lane)
{
    int minDist = 1000;

    for (int i = 0; i < 3; i++)
    {
        if (active[i] && enemyLane[i] == lane)
        {
            int dist = enemyY[i];
            if (dist < minDist)
                minDist = dist;
        }
    }

    return minDist;
}


bool checkLaneAvailability(string direction, Matatu &m)
{
    int targetLane = m.getLane();

    if (direction == "left") targetLane--;
    if (direction == "right") targetLane++;

    if (targetLane < 0 || targetLane > 2)
        return false;

    for (int i = 0; i < 3; i++)
    {
        if (active[i] &&
		    enemyLane[i] == targetLane &&
			enemyY[i] < m.getY() &&
			(m.getY() - enemyY[i]) < 5)

            return false; // blocked
        
    }

    return true;
}

string autoDecision(Matatu &m)
{
    bool obstacleAhead = detectObstacleAhead(m);
    bool leftFree = checkLaneAvailability("left", m);
    bool rightFree = checkLaneAvailability("right", m);

    if (!obstacleAhead)
        return "STRAIGHT";

    
// ? BOTH lanes free ? choose randomly
    
if (leftFree && rightFree)
{
    int leftScore = laneSafetyScore(m.getLane() - 1);
    int rightScore = laneSafetyScore(m.getLane() + 1);

    if (leftScore > rightScore)
        return "LEFT";
    else
        return "RIGHT";
}

if (leftFree) 
{
	return "LEFT";
}

if (rightFree){
	return "RIGHT";
	
}
    return "BRAKE";
}



// =====================
// COLLISION
// =====================
bool checkCollision(Matatu &m)
{
    for (int i = 0; i < 3; i++)
    {
        if (enemyLane[i] == m.getLane() && enemyY[i] == m.getY())
            return true;
    }
    return false;
}

// =====================
// MAIN
// =====================
int main()
{
    system("cls");	
    SetConsoleOutputCP(65001);
    srand(time(0));
    hideCursor();
    drawRoad();
    drawBoundary();
    Matatu matatu;
    

  

    while (true)
    {
    	
    	
        // erase previous frame
        for (int i = 0; i < 3; i++)
            eraseEnemy(i);

        matatu.erase();
		
		spawnCounter++;
		// spawn first obstacle immediately
		if (!active[0])
		{
		    genEnemy(0);
			
		}
		
		if (!active[1] && enemyY[0] > 10)
		{
		    genEnemy(1);
		    
		}


        // input
        if (_kbhit())
{
    char key = _getch();

    if (key == 27)   // ESC key
    {
    	system("cls");
        gotoxy(30, 10);
        cout << "GAME OVER";
        break;
    }

    userInput(key, matatu);
}

string decision = autoDecision(matatu);

if (decision == "LEFT")
{
    matatu.changeLane("left");
}
else if (decision == "RIGHT")
{
    matatu.changeLane("right");
}

            
        // update
        for (int i = 0; i < 3; i++)
            updateEnemy(i);

        // draw
        for (int i = 0; i < 3; i++)
            drawEnemy(i);

        matatu.draw();

        // collision
        if (checkCollision(matatu))
        {
        	system("cls");	
            gotoxy(30, 10);
            cout << "💥💥 CRASH! GAME OVER";
            break;
        }

        Sleep(100);
    }

    return 0;
}
