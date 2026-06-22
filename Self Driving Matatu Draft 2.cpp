#include <iostream>
using namespace std;
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


