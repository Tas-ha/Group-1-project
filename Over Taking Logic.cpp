#include <iostream>
#include <string>
using namespace std;

 void overtake(){
    float my_speed = 80.0;
    float obstacle_speed = 50.0;
    int safe_distance = 4;
    int over_taking_distance = 4;
    string lane_1 = "Left lane";
    string lane_2 = "Right lane";
    string current_lane = lane_1;

    cout << "My current speed is: " << my_speed << " km/h" << endl;
    cout << "The obstacle speed is: " << obstacle_speed << " km/h" << endl;
    cout << "The safe distance is: " << safe_distance << " meters" << endl;
    cout << "The overtaking distance is: " << over_taking_distance << " meters" << endl;
    cout << "Current lane is: " << current_lane << endl;

    if (my_speed <= obstacle_speed) {
        cout << "Cannot overtake, consider increasing your speed first." << endl;
        return;
    } else {
        cout << "Speed check passed. You can overtake." << endl;
    }
     if (safe_distance <= 10){
        cout<<"Get closer to obstacle and prepare to overtake"<<endl;
     }
     else{
        while(safe_distance > 10){
            cout<<"Consider attaining the safe distance first before overtaking"<<endl;
            safe_distance -= 5;
        }
     }
     if (over_taking_distance <= 5){
        cout<<"You are ready to overtake"<<endl;
     }
     else{
        while(over_taking_distance > 5){
            cout<<"Consider attaining the overtaking distance first before overtaking"<<endl;
            over_taking_distance -= 5;
        }
     }
     cout<<"Over taking in progress..."<<endl;
     if (over_taking_distance <= 4) {
        cout << "Overtaking distance reached." << endl;
        cout << "Switching from " << lane_1 << " to " << lane_2 << endl;
        current_lane = lane_2;
        cout << "Overtaking...." << endl;
        cout << "Overtaking successful. Returning to " << lane_1 << endl;
        current_lane = lane_1;
    } else {
        cout << "Overtake aborted: Conditions not met." << endl;
    }
}

 int main() {
    overtake();
    return 0;
}