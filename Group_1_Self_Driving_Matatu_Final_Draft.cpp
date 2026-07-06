#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>

using namespace sf;
using namespace std;

// =====================
// SETTINGS
// =====================
const int WINDOW_WIDTH  = 900;
const int WINDOW_HEIGHT = 900;

const int LANE_COUNT   = 3;
const int LANE_WIDTH   = 170;
const int ROAD_LEFT    = 210;           
const int ROAD_RIGHT   = ROAD_LEFT + LANE_COUNT * LANE_WIDTH;

const float CAR_WIDTH   = 66.f;
const float CAR_HEIGHT  = 99.f;
const float PLAYER_Y    = WINDOW_HEIGHT - 130.f;   
const float MIN_SPEED = 0.f;
const float MAX_SPEED = (7.5f);
const float SPEED_STEP = 1.f;
                     
const float SPAWN_GAP_Y = 300.f;
const float ZEBRA_HEIGHT = 80.f;
float zebraY = -1200.f;  
bool zebraActive = true;

bool braking = false;
float gameSpeed = (7.5f);
const float FULL_GAME_DURATION = 600.0f;
bool timeOver = false;

const float AI_REACT_DISTANCE = 250.f;
const float LANE_CLEAR_GAP    = 180.f;

float laneCenterX(int lane)
{
    return ROAD_LEFT + lane * LANE_WIDTH + LANE_WIDTH / 2.f;
}

// =====================
// MATATU (PLAYER)
// =====================
class Matatu
{
private:
    int lane;
    float y;
    RectangleShape shape;   
    Sprite sprite;          
    bool useSprite = false;

public:
    Matatu()
    {
        lane = 1;
        y = PLAYER_Y;
        shape.setSize(Vector2f(CAR_WIDTH, CAR_HEIGHT));
        shape.setFillColor(Color::Green);
        shape.setOrigin(CAR_WIDTH / 2.f, CAR_HEIGHT / 2.f);
    }

    void changeLane(const string &direction)
    {
        if (direction == "left" && lane > 0)
        {
            lane--;
        }
        else if (direction == "right" && lane < LANE_COUNT - 1)
        {
            lane++;
        }
    }

    int getLane() const { return lane; }
    float getY() const { return y; }

    FloatRect getBounds() const
    {
        return FloatRect(laneCenterX(lane) - CAR_WIDTH / 2.f, y - CAR_HEIGHT / 2.f, CAR_WIDTH, CAR_HEIGHT);
    }

    void setTexture(Texture &tex)
    {
        sprite.setTexture(tex);
        sprite.setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);

        float scaleX = CAR_WIDTH  / tex.getSize().x;
        float scaleY = CAR_HEIGHT / tex.getSize().y;
        sprite.setScale(scaleX, scaleY);

        useSprite = true;
    }

    void draw(RenderWindow &window)
    {
        if (useSprite)
        {
            sprite.setPosition(laneCenterX(lane), y);
            window.draw(sprite);
        }
        else
        {
            shape.setPosition(laneCenterX(lane), y);
            window.draw(shape);
        }
    }
};

// =====================
// OBSTACLES
// =====================
const int MAX_OBSTACLES = 3;
int   enemyLane[MAX_OBSTACLES];
float enemyY[MAX_OBSTACLES];
bool  active[MAX_OBSTACLES] = { false, false, false };

RectangleShape enemyShape[MAX_OBSTACLES];    
Sprite enemySprite[MAX_OBSTACLES];          
bool enemyUseSprite = false;

void initEnemyShapes(Texture enemyTexture[])
{
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        enemyShape[i].setSize(Vector2f(CAR_WIDTH, CAR_HEIGHT));
        enemyShape[i].setFillColor(Color::Red);
        enemyShape[i].setOrigin(CAR_WIDTH / 2.f, CAR_HEIGHT / 2.f);
    }

    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        if (enemyTexture[i].getSize().x > 0)
        {
            enemySprite[i].setTexture(enemyTexture[i]);

            enemySprite[i].setOrigin(enemyTexture[i].getSize().x / 2.f, enemyTexture[i].getSize().y / 2.f);

            enemySprite[i].setScale( CAR_WIDTH / enemyTexture[i].getSize().x, CAR_HEIGHT / enemyTexture[i].getSize().y);
        }
    }
    enemyUseSprite = true;
}
void genEnemy(int i)
{
    bool yTooClose;
    float newY;
    int newLane;
    bool laneTaken;

    
    do
    {
        newLane = rand() % LANE_COUNT;
        laneTaken = false;

        
        for (int j = 0; j < MAX_OBSTACLES; j++)
        {
            if (j != i && active[j] && enemyLane[j] == newLane)
            {
                laneTaken = true;
                break;
            }
        }
    } while (laneTaken);

    enemyLane[i] = newLane;
    
    int attempts = 0;

    do
    {
        yTooClose = false;
        newY = -(rand() % 800 + 100);   

        for (int j = 0; j < MAX_OBSTACLES; j++)
        {
            if (j != i && active[j] && fabs(newY - enemyY[j]) < SPAWN_GAP_Y)
            {
                yTooClose = true;
                break;
            }
        }
        attempts++;
    } 
    while (yTooClose && attempts < 100);

    enemyY[i] = newY;
    active[i] = true;

    if (fabs(newY - zebraY) < 200.f)
    {
        yTooClose = true;
    }

}
void updateEnemy(int i)
{
    if (!active[i]) return;
    {
    enemyY[i] += gameSpeed;
    }
    if (enemyY[i] > WINDOW_HEIGHT + CAR_HEIGHT)
    {
        active[i] = false;
    }
}
void drawEnemy(int i, RenderWindow &window)
{
    if (!active[i]) return;
    {
        if (enemyUseSprite)
        {
            enemySprite[i].setPosition(laneCenterX(enemyLane[i]), enemyY[i]);
            window.draw(enemySprite[i]);
        }
        else
        {
            enemyShape[i].setPosition(laneCenterX(enemyLane[i]), enemyY[i]);
            window.draw(enemyShape[i]);
        }
    }
}
FloatRect enemyBounds(int i)
{
    return FloatRect(laneCenterX(enemyLane[i]) - CAR_WIDTH / 2.f, enemyY[i] - CAR_HEIGHT / 2.f, CAR_WIDTH, CAR_HEIGHT);
}

// =====================
// ZEBRA CROSSING & TRAFFIC LIGHTS
// =====================

void updateZebraCrossing()
{
    zebraY += gameSpeed;

    if (zebraY > WINDOW_HEIGHT)
    {
        zebraY = -(rand() % 3000 + 1500);
    }
}
void drawZebraCrossing(RenderWindow& window)
{
    for (float x = ROAD_LEFT; x < ROAD_RIGHT; x += 25)
    {
        RectangleShape stripe(Vector2f(15.f, ZEBRA_HEIGHT));
        stripe.setPosition(x, zebraY);
        stripe.setFillColor(Color::White);
        window.draw(stripe);
    }
}
FloatRect zbBounds(int zebraLane)
{
    return FloatRect(ROAD_LEFT, zebraY, ROAD_RIGHT - ROAD_LEFT, ZEBRA_HEIGHT);
}
enum LightState
{
    GREEN,
    AMBER,
    RED
};

Clock zebraClock;

bool zebraWaiting = false;
bool zebraCrossed = false;

const float stopRedTime = 3.5f; 

LightState trafficLight = GREEN;

void updateTrafficLight()
{
    float distanceToCrossing = zebraY - PLAYER_Y;

    if (distanceToCrossing < -500)
    {
        trafficLight = GREEN;
    }
    else if (distanceToCrossing < -230)
    {
        trafficLight = AMBER;
    }
    if (distanceToCrossing > -200 && !zebraWaiting && !zebraCrossed)
    {
        zebraWaiting = true;
        zebraClock.restart();
        trafficLight = RED;
    }
    if (zebraWaiting)
    {
        if (zebraClock.getElapsedTime().asSeconds() > stopRedTime)
        {
            zebraWaiting = false;
            zebraCrossed = true;
            trafficLight = GREEN;

        }
    }
}
void drawTrafficLight(RenderWindow& window, LightState state)
{
    CircleShape light[3];
    
    for (int i = 0; i < 3; i++)
    {
        light[i].setRadius(20.f);
    }

    light[0].setPosition(800, 40);
    light[1].setPosition(800, 80);
    light[2].setPosition(800, 120);

    switch(state)
    {
        case RED:
            light[0].setFillColor(Color::Red);
            light[0].setOutlineColor(Color::Black);
            light[0].setOutlineThickness(2.f);
            break;

        case AMBER:
            light[1].setFillColor(Color(255,165,0));
            light[1].setOutlineColor(Color::Black);
            light[1].setOutlineThickness(2.f);
            break;

        case GREEN:
            light[2].setFillColor(Color::Green);
            light[2].setOutlineColor(Color::Black);
            light[2].setOutlineThickness(2.f);
            break;
    }

    for (int i = 0; i < 3; i++)
    {
        window.draw(light[i]);
    }

}

// =====================
// AI DECISION-MAKING
// =====================

bool detectObstacleAhead(Matatu &m)
{
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        if (active[i] && enemyLane[i] == m.getLane() && fabs(enemyY[i] - m.getY()) < AI_REACT_DISTANCE)
        {
            return true;
        }
    }
    return false;
}
float laneSafetyScore(int lane)
{
    float nearestObstacle = 100000.f;

    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        if (active[i] && enemyLane[i] == lane)
        {
            float dist = PLAYER_Y - enemyY[i];

            if (dist > 0 && dist < nearestObstacle)
            {
                nearestObstacle = dist;
            }
        }
    }

    return nearestObstacle;
}
bool checkLaneAvailability(const string &direction, Matatu &m)
{
    int targetLane = m.getLane();

    if (direction == "left")
    {
        targetLane--;
    }
    if (direction == "right") 
    {
        targetLane++;
    }
    if (targetLane < 0 || targetLane >= LANE_COUNT)
    {
        return false;
    }
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        if (active[i] && enemyLane[i] == targetLane && fabs(enemyY[i] - m.getY()) < LANE_CLEAR_GAP)
        {
            return false;
        }
    }

    return true;
}
string autoDecision(Matatu &m)
{
    if (trafficLight == RED)
    {
        return "BRAKE";
    }

    bool obstacleAhead = detectObstacleAhead(m);
    bool leftFree  = checkLaneAvailability("left", m);
    bool rightFree = checkLaneAvailability("right", m);

    if (!obstacleAhead)
    {
        return "STRAIGHT";
    }
    if (leftFree && rightFree)
    {
        float leftScore  = laneSafetyScore(m.getLane() - 1);
        float rightScore = laneSafetyScore(m.getLane() + 1);
        
        if (leftScore == rightScore)
        {
            return (rand() % 2 == 0) ? "LEFT" : "RIGHT";
        }
    return (leftScore > rightScore) ? "LEFT" : "RIGHT";    
    }
    if (leftFree)  
    {
        return "LEFT";
    }
    else if (rightFree) 
    {
        return "RIGHT";
    }
    else 
    {
        return "BRAKE";
    }
}

// =====================
// COLLISION DETECTION
// =====================
bool checkCollision(Matatu &m, int zebraLane = 1)
{
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        if (active[i] && m.getBounds().intersects(enemyBounds(i)))
        {
            return true;
        }
    }
    if (trafficLight == RED && m.getBounds().intersects(zbBounds(zebraLane)))
    {
        return true;
    }
    return false;
}

// =====================
// RESET GAME  
// =====================

void resetGame(Matatu &m, Texture &playerTexture)
{
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        active[i] = false;
        enemyY[i] = -1000;
        enemyLane[i] = 0;
    }
    m = Matatu();
    if (playerTexture.getSize().x > 0)
    {
        m.setTexture(playerTexture);
    }

    zebraY = -1200.f;
    zebraWaiting = false;
    zebraCrossed = false;
    LightState trafficLight = GREEN;
    gameSpeed = 7.5f;
}

// =====================
// TEXT SETTINGS
// =====================

Text crashedText(Font &font)
{
    Text text;
    text.setFont(font);
    text.setString ("\t\t\tYOU CRASHED! TRY AGAIN\nPRESS R TO RESTART OR ESC TO EXIT");
    text.setCharacterSize(35);
    text.setFillColor(Color::White);
    text.setPosition(WINDOW_WIDTH / 2.f -320 , WINDOW_HEIGHT / 2.f - 50.f);

    return text;
}
Text GameOverText(Font &font)
{
    Text text;
    text.setFont(font);
    text.setString ("\t\t\t YOU ENDED THE GAME\nPRESS R TO RESTART OR ESC TO EXIT");
    text.setCharacterSize(35);
    text.setFillColor(Color::White);
    text.setPosition(WINDOW_WIDTH / 2.f -320 , WINDOW_HEIGHT / 2.f - 50.f);

    return text;
}
Text TimeOverText(Font &font)
{
    Text text;
    text.setFont(font);
    text.setString ("\t\t\t TIME'S UP!\nPRESS R TO RESTART OR ESC TO EXIT");
    text.setCharacterSize(35);
    text.setFillColor(Color::White);
    text.setPosition(WINDOW_WIDTH / 2.f -320 , WINDOW_HEIGHT / 2.f - 50.f);

    return text;
}
Text StartText(Font &font)
{
    Text text;
    text.setFont(font);
    text.setString(
        "\t\t\t------------------------------\n"
        "\t\t\t| Self Driving Matatu |\n"
        "\t\t\t------------------------------\n"
        "        Press SPACE to Start Game\n"
        "        Press 1. For instructions\n"
        "You have 10 minutes then the game resets"
            );

    text.setCharacterSize(35);
    text.setPosition(150, WINDOW_HEIGHT / 2 - 140);

    return text;
}
Text instructionsText(Font &font)
{
    Text text;
    text.setFont(font);
    text.setString(
        "Instructions:\n"
        "1. Avoid obstacles by changing lanes.\n"
        "2. Press A or Left Arrow to move left.\n"
        "3. Press D or Right Arrow to move right.\n"
        "4. Press W or Up Arrow to increase speed.\n"
        "5. Press S or Down Arrow to decrease speed.\n"
        "6. Press P to turn the AI control on/off.\n"
        "7. Press R to restart the game.\n"
        "8. Press ESC to exit the game.\n"
        "Press ESC to return to the main menu."

    );

    text.setCharacterSize(25);
    text.setFillColor(Color::White);
    text.setPosition(WINDOW_WIDTH / 2 - 300, WINDOW_HEIGHT / 2 - 150);

    return text;
}
Text MonitorText(Font& font, bool aiEnabled, float speed, int lane, LightState light, float gameSpeed, float timeRemaining)
{
    Text text; 
    text.setFont(font);
    string mode = aiEnabled ? "AI" : "MANUAL";
    string lightText;
    string timeText;
    string currspeed;
    stringstream ss;
    stringstream SS;
    ss << fixed << setprecision(1) << timeRemaining;
    timeText = ss.str() + " min";
    SS << fixed << setprecision(1) << speed;
    currspeed = SS.str();
    

    if (light == GREEN) 
    {
        lightText = "GREEN";
    }
    else if (light == AMBER)
    {
        lightText = "AMBER";
    }
    else
    {
        lightText = "RED";
    }

    string status;

    if (gameSpeed > 0.0f)
        status = "MOVING";
    else
        status = "STOPPED";

    text.setString(
        "MODE: " + mode +
        "\nSPEED: " + currspeed +
        "\nLANE: " + to_string(lane + 1) +
        "\nLIGHT: " + lightText +
        "\nSTATUS: " + status +
        "\nTIME REMAINING: " + timeText
    );

    text.setCharacterSize(17);
    text.setFillColor(Color::White);
    text.setPosition(8.f, 10.f);

    return text;
}

// =====================
// ROAD DRAWING
// =====================
void drawRoad(RenderWindow &window, float roadOffset)
{
    RectangleShape roadBg(Vector2f(ROAD_RIGHT - ROAD_LEFT, WINDOW_HEIGHT));
    roadBg.setPosition(ROAD_LEFT, 0);
    roadBg.setFillColor(Color(40, 40, 40));
    window.draw(roadBg);

    for (int lane = 1; lane < LANE_COUNT; lane++)
    {
        float x = ROAD_LEFT + lane * LANE_WIDTH;

        float dashHeight = 30.f;
        float gapHeight  = 20.f;

        for (float y = roadOffset; y < WINDOW_HEIGHT; y += dashHeight + gapHeight)
        {
            RectangleShape dash(Vector2f(4.f, dashHeight));
            dash.setPosition(x, y);
            dash.setFillColor(Color::White);

            window.draw(dash);
        }
    }
}

// =====================
// MAIN
// =====================
int main()
{
    srand((unsigned int)time(0));

    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Self Driving Matatu");
    window.setFramerateLimit(60);

    Matatu matatu;

    Texture playerTexture;
    Texture enemyTexture[3];

    bool playerTexOk = playerTexture.loadFromFile("matatu.png");
    bool enemyTexOk =
        enemyTexture[0].loadFromFile("obstacle.png") &&
        enemyTexture[1].loadFromFile("obstacle1.png") &&
        enemyTexture[2].loadFromFile("obstacle2.png");

    Clock modeClock;
    Clock changeLaneClock;
    Clock gameTimer;

    bool paused = false;
    bool gameStarted = false;
    float roadOffset = 0.f;
    bool nightMode = false;
    bool showInstructions = false;
    float timeRemaining = ((FULL_GAME_DURATION - gameTimer.getElapsedTime().asSeconds()) / 60.f);

    initEnemyShapes(enemyTexture);

    if (playerTexOk)
        matatu.setTexture(playerTexture);

    Font font;
    bool hasFont = font.loadFromFile("arial.ttf");
    bool gameOver = false;
    bool crashed = false;
    bool aiEnabled = true;

    while (window.isOpen())
    {
        // =====================
        // USER INPUT
        // =====================
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Escape)
                {
                    if (showInstructions)
                    {
                        showInstructions = false;
                    }
                    else if (gameOver == true)
                    {
                        window.close();
                    }
                    else
                    {
                        gameOver = true;
                    }
                }
                if (event.key.code == Keyboard::Space)
                {
                    if (!gameStarted && !showInstructions)
                    {                         
                        gameStarted = true;
                        gameTimer.restart();
                    }
                    else if (!gameOver && !crashed)
                    {
                        paused = !paused;
                    }
                }
                if (event.key.code == Keyboard::Num1)
                {
                    showInstructions = true;
                }
                if (event.key.code == Keyboard::R)
                {
                    resetGame(matatu, playerTexture);
                    gameOver = false;
                    paused = false;
                    gameStarted = false;  
                    gameSpeed = (7.5f);
                    crashed = false;
                }
                if (event.key.code == Keyboard::P)
                {
                    aiEnabled = !aiEnabled;   
                }
                if (!gameOver && !aiEnabled)
                {
                    if (event.key.code == Keyboard::A || event.key.code == Keyboard::Left)
                    {
                        matatu.changeLane("left");
                    }
                    if (event.key.code == Keyboard::D || event.key.code == Keyboard::Right)
                    {
                        matatu.changeLane("right");
                    }
                }
                if (event.key.code == Keyboard::W || event.key.code == Keyboard::Up)
                {
                        gameSpeed += SPEED_STEP;
                        if (gameSpeed > MAX_SPEED)
                        {
                        gameSpeed = MAX_SPEED;
                        }
                }
                if (event.key.code == Keyboard::S || event.key.code == Keyboard::Down)
                {
                    gameSpeed -= SPEED_STEP;
                    if (gameSpeed < MIN_SPEED)
                    {
                       gameSpeed = MIN_SPEED;
                    }
                }
            }
        }

        // =====================
        // UPDATE
        // =====================
        if (gameTimer.getElapsedTime().asSeconds() >= FULL_GAME_DURATION)
                {
                    timeOver = true;
                    gameOver = true;
                }
        if (gameStarted && !gameOver && !paused)
        {
            for (int i = 0; i < MAX_OBSTACLES; i++)
            {
                if (!active[i])
                {
                    genEnemy(i);
                }
            }
            if (aiEnabled)
            {
                string decision = autoDecision(matatu);

                if (changeLaneClock.getElapsedTime().asMilliseconds() > 500)
                {
                    if (decision == "LEFT") 
                    {
                        matatu.changeLane("left");
                        changeLaneClock.restart();
                    }
                    else if (decision == "RIGHT") 
                    {
                        matatu.changeLane("right");
                        changeLaneClock.restart();
                    }
                    else if (decision == "BRAKE") 
                    {
                        braking = true;
                    }
                }
            }
            
            updateTrafficLight();
            
            float targetSpeed;
            braking = false;

            if (aiEnabled)
            {   
                if (trafficLight == AMBER)
                {
                    targetSpeed = 3.5f;
                }
                if (trafficLight == RED)
                {
                    targetSpeed = 0.f;                    
                }
                if (trafficLight == GREEN)
                {
                    targetSpeed = 7.5f;
                }
                if (gameSpeed < targetSpeed)
                {
                    gameSpeed += 0.1f;
                    if (gameSpeed > targetSpeed)
                    {
                        gameSpeed = targetSpeed;
                    }
                }
                if (gameSpeed > targetSpeed)
                {
                    gameSpeed -= 0.1f;
                    if (gameSpeed < targetSpeed)
                    {
                        gameSpeed = targetSpeed;
                    }
                }
            }
            
            roadOffset += gameSpeed;

            if (roadOffset > 50.f)
            {
                roadOffset = 0.f;
            }
            
            zebraY += gameSpeed;
            if (zebraY > WINDOW_HEIGHT)
            {
                zebraY = -(rand() % 3000 + 1500);
                zebraCrossed = false;
            }
            
            for (int i = 0; i < MAX_OBSTACLES; i++)
            {
                if (active[i])
                {
                    enemyY[i] += gameSpeed;
                    if (enemyY[i] > WINDOW_HEIGHT + CAR_HEIGHT)
                    {
                        active[i] = false;
                    }
                }
            }
        }
        if (checkCollision(matatu))
        {
            crashed = true;
        }
    
        // =====================
        // DRAWING
        // =====================
        window.clear(Color::Black);
        if (showInstructions)
        {
            if (hasFont)
            {
                Text instrText = instructionsText(font);
                window.draw(instrText);
            }
        }
        else if (!gameStarted)
        {
            if (hasFont)
            {
                Text startText = StartText(font);
                window.draw(startText);
            }
        }
        else if (gameOver)
        {
            if (hasFont)
            {
                Text gameOverText = GameOverText(font);
                window.draw(gameOverText);
            }
        }
        else if (crashed)
        {
            if (hasFont)
            {
                Text CrashedText = crashedText(font);
                window.draw(CrashedText);
            }
        }
        else if (timeOver)
        {
            if (hasFont)
            {
                Text timeOverText = TimeOverText(font);
                window.draw(timeOverText);
            }
        }
        else
        {
            if (modeClock.getElapsedTime().asSeconds() > 30)
            {
                nightMode = !nightMode;
                modeClock.restart();
            }
            if (nightMode)
            {
                window.clear(Color(1,50,32));
            }
            else
            {
                window.clear(Color(76, 115, 57));
            }

            Text monitorText = MonitorText(font, aiEnabled, gameSpeed, matatu.getLane(), trafficLight, gameSpeed, timeRemaining);
            window.draw(monitorText);
            drawRoad(window, roadOffset);
            drawZebraCrossing(window);
            drawTrafficLight(window, trafficLight);

            for (int i = 0; i < MAX_OBSTACLES; i++)
            {
                drawEnemy(i, window);
            }
            matatu.draw(window);
        }
        window.display();
    }
    return 0;
}
