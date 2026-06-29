#include<iostream>
#include<string>
using namespace std;

class Matatu
{
	private:
		int lane;
		int speed;
		int position;
		
	public:
	Matatu()
{
	lane = 2;
	speed = 5;
	position = 0;
}

void initializeMatatu()
{
	lane = 2;
	speed = 5;
	position = 0;
	
	cout << "\nAUTONOMOUS MATATU SYSTEM\n";
	cout << "Matatu initialized successfully\n";
	cout << "Lane      : Middle\n";
	cout << "speed     :" << speed << endl;
	cout << "position  :" << position << endl;
}

void moveMatatu()
{
	position= position+speed;
	
	cout << "Matatu is moving" << endl;
	cout << "current position :" << position << endl;
	cout << "current speed :" << speed <<endl;
}

void changeLane(string direction)
{
	if (direction == "left"  || direction == "LEFT")
{
	if (lane>1)
{
	lane=lane-1;
	
	cout << "\nswitched over to the left lane" << endl;
}
    else
	{
    cout << "\nNow in the LEFT lane" << endl;	
	}
}
    else if(direction == "right"  || direction == "RIGHT")
    {
    	if(lane<3)
		{
			lane=lane+1;
	cout << "\nswitched to the right lane" << endl;
		}
	else
	{
	cout << "\ncurrently in the RIGHT lane" << endl;
	}
	}
	else
	{
	cout << "\nwrong direction" << endl;
	}
displayLane();
}


void brakeMatatu()
{
	if(speed>0)
{
	speed--;
	
	cout << "\nBraking" << endl;
	cout << "current speed  :" << endl;
}
     else
	 {
     cout << "\nMatatu has already stopped"	<< endl;
	 }
}


void accelerate()
{
	speed++;
	
	cout << "\nAccelerating"  << endl;
	cout << "Current speed :" << endl;
}


void displayLane()
{
	cout << "current lane";
	
	if(lane==1)
	{
		cout << "left";
	}
	else if(lane==2)
	{
		cout  << "middle";
	}
	else
	{
		cout << "Right";
	}
	cout << endl;
}


void displayStatus()
{
	cout << "\n========= VEHICLE STATUS =========\n";
	cout << "lane  :";
	
	if(lane==1)
	{
		cout << "Left";
	}
	else if(lane==2)
	{
		cout << "middle";
	}
	else
	{
		cout << "Right";
	}
	cout << endl;
	
	cout << "speed" << speed << endl;
	cout << "position" << position << endl;
	cout << "======================================\n";}

int getLane(){return lane;}
int getSpeed(){return speed;}
int getPosition(){return position;}

};


int main()
{
	Matatu matatu;
	matatu.initializeMatatu();
	
	int choice;
	bool active=true;
	
	while (active)
	{
		cout << "\n======================================\n";
		cout << "            SELECT AN ACTION           \n";
		cout << "1. Move Matatu\n";
		cout << "2. Accelerate\n";
		cout << "3. Brake\n";
		cout << "4. Switch lane to the left\n";
		cout << "5. Switch lane to the right\n";
		cout << "6. View status of the vehicle\n";
		cout << "7. Exit the system\n";
		cout << "Enter your choice(1-7)";
		cin >> choice;
		switch (choice){
		case 1:
			matatu.moveMatatu();
			break;
		case 2:
			matatu.accelerate();
			break;
		case 3:
			matatu.brakeMatatu();
			break;
		case 4:
			matatu.changeLane("left");
			break;
		case 5:
			matatu.changeLane("right");
			break;
		case 6:
			matatu.displayStatus();
			break;
		case 7:
			cout  << "\nExiting Autonomous Matatu System. Goodbyee!" << endl;
			active = false;
		default:
			cout << "\nInvalid input! Please enter a number between 1 and 7" << endl;
			break;
		}
}
return 0;
}
