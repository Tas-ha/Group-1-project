#include <iostream>
#include <string>
using namespace std;
//bool one_road_way=true;
char getUserInput(){
    char command;

    cout << "Enter command (F=Forward, L=Left, R=Right, S=Stop): ";
    cin >> command;

    return command;
}
//bool one_road_way=true;
int main() {
    string state;
    string obstacle;
    string red_light;
    string speed;
    float obstacle_speed=50.0;
    float my_speed;
    int distance_covered;
    int passengers=0;
    int maximum_passengers=40;
    float total_fare;
    float price=50.00;
    distance_covered=0;
    
   
    //cout << "Which state is your car in:" << endl;
	//cin>>state;
    //if (state == "D") {
        //cout << "Continue Driving or Stop"<<endl;
    //}
    //else if (state == "S") {
       // cout << "Start the Vehicle"<<endl;
       // return 0;
    //}
    //else {
        //cout << "Stop the car"<<endl;
        //return 0;
    //}
    char command;
    command = getUserInput();
    cout<< "You entered: " <<command <<endl;
    
    cout<<"What obstacle do you see?"<<endl;
    cin>>obstacle;
    if(obstacle == "ZebraCrossing"){	
    	cout<<"Stop the vehicle"<<endl;	
	}
	else if(obstacle == "OtherVehicles"){	
		cout<<"Stop the vehicle or Change the lane"<<endl;	
	}
	else if(obstacle == "ZebraCrossing" && obstacle =="OtherVehicles" || obstacle=="redlight"){	
		cout<<"Stop the vehicle"<<endl;	
	}
	else{
		cout<<"Continue Driving"<<endl;	
	}
	
	if (obstacle=="OtherVehicles"){
	cout<<"	What is your current speed?"<<endl;
	cin>>my_speed;	
	}
   if(my_speed>=obstacle_speed){
   	cout<<"You can over take the car"<<endl;
   }
   else{
   	cout<<"Consider increasing your speed first"<<endl;
   }
   
   
   
   cout<<"What are the total number of passengers?"<<endl;
   cin>>passengers;
   cout<<"What was the fare for each passenger?"<<endl;
   cin>>price;
   
   while(passengers<maximum_passengers){
   	passengers++;
   	total_fare=passengers*price;
   }
   cout<<"The total number of passengers were"<<passengers<<"and the total fare collected was"<<total_fare<<endl;
   
   
    return 0;
}
