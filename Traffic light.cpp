#include <iostream>
#include <string>
using namespace std;

void genZebCrossing(){

 string zebra_crossing;
 string red_light="Red light";
 string green_light="Green light";
 string current_light= green_light;
 
 float car_speed=80.00;
 int safe_distance=5;
 bool is_car_moving = true; 

 if (is_car_moving) {
        current_light = green_light;
        cout << "Current light is: " << green_light << endl;
        cout << "Car is moving." << endl;
    } else {
        current_light = red_light;
        cout << "Current light is: " << red_light << endl;
        cout << "Car has stopped." << endl;
    }

    if (current_light == red_light) {
        cout << "Car must stop at the zebra crossing." << endl;
    } else if (current_light == green_light) {
        cout << "Car can proceed through the zebra crossing." << endl;
    }

if (safe_distance < 5 && current_light == red_light ) {
    cout<<"Consider decreasing your safe distance first."<<endl;
    while (safe_distance > 5) {
        safe_distance -= 1;
    }
    // bool is_car_moving = false;
        cout << "Safe distance is: " << safe_distance << " meters" << endl;
        cout << "Car is too close to the zebra crossing. Slow down and prepare to stop." << endl;
    } else {
        cout << "Safe distance is: " << safe_distance << " meters" << endl;
        cout << "Car is at a safe distance from the zebra crossing." << endl;
    }

    cout<<"The speed of the car is: "<<car_speed<<" km/h"<<endl;
    if (safe_distance < 5 && car_speed >= 80.00){
    car_speed -= 10.00;
    cout<<"Car speed is: "<<car_speed<<" km/h"<<endl;
    }

    // if (!is_car_moving && current_light == red_light) {
    //     cout << "Car has stopped at the zebra crossing." << endl;
    // }

}