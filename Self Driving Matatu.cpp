#include <iostream>
#include <string>
using namespace std;

int main() {
    string state;
    string obstacle;
    string red_light;
    float obstacle_speed=50.0;
    float my_speed;
    int passengers;
    int maximum_passengers=40;
    float total_fare;
    float price;
    int count=passengers;
    string lane_1;
    string lane_2;

    string starting_lane = lane_1;
    
    
   
    cout << "Which state is your car in:" << endl;
    cin>>state;
    if (state == "D") {
        cout << "Continue Driving or Stop"<<endl;
    }
    else if (state == "S") {
        cout << "Start the Vehicle"<<endl;
        return 0;
    }
    else {
        cout << "Stop the car"<<endl;
        return 0;
    }
    cout<<"What obstacle do you see?"<<endl;
    cin>>obstacle;
    if(obstacle == "ZebraCrossing"){	
    	cout<<"Stop the vehicle"<<endl;	
    	return 0;
	}
	else if(obstacle=="redlight"){	
		cout<<"Stop the vehicle and wait for the green light."<<endl;
		
			
	}
	else if(obstacle == "OtherVehicles"){	
		cout<<"Stop the vehicle or Change the lane"<<endl;	
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
   
   
   
   cout<<"What are total number of passengers"<<endl;
   cin>>passengers;
   
   if(passengers>40){
   	cout<<"The passengers should not exceed the limit of 40"<<endl;
   
   
   while(passengers>maximum_passengers){
   	cout<<"Current Passengers are : "<<passengers<<"The Excess passengers are : "<<passengers-maximum_passengers<<endl;
   	passengers--;
   } 
}
   	else{
   	cout<<"Proceed to add passengers till the limit is reached."<<endl;	
	   }

   cout<<"What was the fare for each passenger?"<<endl;
   cin>>price;
   
   total_fare=passengers*price;
   cout<<"The total number of passengers were::"<<passengers<<"and the total fare collected was::"<<total_fare<<endl;
   
   
    return 0;
}