#include <iostream>
using namespace std;
string autoDecision() {
    bool obstacle = detectObstacleAhead();
    bool leftFree = checkLaneAvailability("left");
    bool rightFree = checkLaneAvailability("right");

    if (obstacle == false) {
        return "STRAIGHT";
    }

    if (leftFree == true) {
        return "LEFT";
    }
    else if (rightFree == true) {
        return "RIGHT";
    }
    else {
        return "BRAKE";
    }
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


