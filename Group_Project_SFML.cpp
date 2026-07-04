#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>

using namespace sf;
using namespace std;

// =====================
// SETTINGS
// =====================
const int WINDOW_WIDTH  = 800;
const int WINDOW_HEIGHT = 800;

const int LANE_COUNT   = 3;
const int LANE_WIDTH   = 150;
const int ROAD_LEFT    = 175;              // left edge of road
const int ROAD_RIGHT   = ROAD_LEFT + LANE_COUNT * LANE_WIDTH;

const float CAR_WIDTH   = 60.f;
const float CAR_HEIGHT  = 90.f;
const float PLAYER_Y    = WINDOW_HEIGHT - 130.f;   // fixed screen y for player
const float MIN_SPEED = 0.f;
const float MAX_SPEED = 6.f;
const float SPEED_STEP = 1.f;
                     
const float SPAWN_GAP_Y = 300.f;                    // min vertical gap between obstacles when spawning
const float ZEBRA_HEIGHT = 80.f;
float zebraY = -1200.f;   // starts off-screen
bool zebraActive = true;
bool zbcrossingStop = false;
bool braking = false;
bool playerBraking = false;
float gameSpeed = 5.f;                            // pixels/frame obstacles fall
float movementFactor = 1.0f; 

// AI reaction tuning:
// AI_REACT_DISTANCE - how far ahead the AI "sees" an obstacle and starts to react
// LANE_CLEAR_GAP     - how much clear space is required in a lane before the AI will swerve into it
// LANE_CLEAR_GAP is intentionally smaller than AI_REACT_DISTANCE so the AI reacts
// with room to spare, instead of only reacting once already at the minimum safe gap
const float AI_REACT_DISTANCE = 250.f;
const float LANE_CLEAR_GAP    = 180.f;
// returns the x pixel center of a lane
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
    RectangleShape shape;   // fallback rectangle, used if no texture is loaded
    Sprite sprite;          // used when a texture is available
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
            lane--;
        else if (direction == "right" && lane < LANE_COUNT - 1)
            lane++;
    }

    int getLane() const { return lane; }
    float getY() const { return y; }

    FloatRect getBounds() const
    {
        return FloatRect(laneCenterX(lane) - CAR_WIDTH / 2.f,
                          y - CAR_HEIGHT / 2.f,
                          CAR_WIDTH, CAR_HEIGHT);
    }

    // links this Matatu to a loaded texture (e.g. matatu.png) and switches
    // draw() over to using the sprite instead of the plain rectangle
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

RectangleShape enemyShape[MAX_OBSTACLES];   // fallback rectangles
Sprite enemySprite[MAX_OBSTACLES];          // used when a texture is available
bool enemyUseSprite = false;

// sets up the fallback rectangles, and -- if tex loaded successfully --
// also sets up sprites for every obstacle slot
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

            enemySprite[i].setOrigin(
                enemyTexture[i].getSize().x / 2.f,
                enemyTexture[i].getSize().y / 2.f
            );

            enemySprite[i].setScale(
                CAR_WIDTH / enemyTexture[i].getSize().x,
                CAR_HEIGHT / enemyTexture[i].getSize().y
            );
        }
    }

    enemyUseSprite = true;
}

bool yTooClose;
float newY;

// spawns obstacle i, avoiding lane overlap and vertical clustering
void genEnemy(int i)
{
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
        newY = -(rand() % 800 + 100);   // spawn above the visible window

        for (int j = 0; j < MAX_OBSTACLES; j++)
        {
            if (j != i && active[j] && fabs(newY - enemyY[j]) < SPAWN_GAP_Y)
            {
                yTooClose = true;
                break;
            }
        }
        attempts++;
    } while (yTooClose && attempts < 100);

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

    enemyY[i] += gameSpeed;

    if (enemyY[i] > WINDOW_HEIGHT + CAR_HEIGHT)
        active[i] = false;
}

void drawEnemy(int i, RenderWindow &window)
{
    if (!active[i]) return;

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

FloatRect enemyBounds(int i)
{
    return FloatRect(laneCenterX(enemyLane[i]) - CAR_WIDTH / 2.f,
                      enemyY[i] - CAR_HEIGHT / 2.f,
                      CAR_WIDTH, CAR_HEIGHT);
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

void drawZebraCrossing(RenderWindow& window
)
{
    for (float x = ROAD_LEFT; x < ROAD_RIGHT; x += 25)
    {
        RectangleShape stripe(Vector2f(15.f, ZEBRA_HEIGHT));

        stripe.setPosition(x, zebraY);

        stripe.setFillColor(Color::White);

        window.draw(stripe);
    }
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

const float ZEBRA_WAIT_TIME = 3.5f; // seconds

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
    if (distanceToCrossing > -190 && !zebraWaiting && !zebraCrossed)
    {
        zebraWaiting = true;
        zebraClock.restart();
        trafficLight = RED;
    }
    if (zebraWaiting)
    {
        if (zebraClock.getElapsedTime().asSeconds() > ZEBRA_WAIT_TIME)
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

    light[0].setPosition(700, 40);
    light[1].setPosition(700, 80);
    light[2].setPosition(700, 120);

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
        if (active[i] &&
            enemyLane[i] == m.getLane() &&
            fabs(enemyY[i] - m.getY()) < AI_REACT_DISTANCE)
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
                nearestObstacle = dist;
        }
    }

    return nearestObstacle;
}

bool checkLaneAvailability(const string &direction, Matatu &m)
{
    int targetLane = m.getLane();

    if (direction == "left") targetLane--;
    if (direction == "right") targetLane++;

    if (targetLane < 0 || targetLane >= LANE_COUNT)
        return false;

    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        // block if ANY active obstacle in the target lane is within the
        // safety gap, whether it's still approaching (above the player)
        // or already passing/exiting (below the player) but not fully clear yet
        if (active[i] &&
            enemyLane[i] == targetLane &&
            fabs(enemyY[i] - m.getY()) < LANE_CLEAR_GAP)
            return false; // blocked
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
        return "STRAIGHT";

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

    if (leftFree)  return "LEFT";
    if (rightFree) return "RIGHT";

    return "BRAKE";
}

// =====================
// COLLISION (rectangle intersection instead of exact coordinate match,
// since pixel positions move continuously instead of in console cells)
// =====================
bool checkCollision(Matatu &m)
{
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        if (active[i] && m.getBounds().intersects(enemyBounds(i)))
            return true;
    }
    return false;
}

// =====================
// RESTART
// =====================
// clears all active obstacles and puts the player back to the starting lane/position
void resetGame(Matatu &m, Texture &playerTexture)
{
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        active[i] = false;
        enemyY[i] = 0;
        enemyLane[i] = 0;
    }

    m = Matatu();   // re-run the constructor to reset lane/position

    // re-apply the texture, since the fresh Matatu() above resets useSprite to false
    if (playerTexture.getSize().x > 0)
        m.setTexture(playerTexture);
}

// =====================
// TEXT SETTINGS
// =====================

Text GameOverText(Font &font)
{
    Text text;
    text.setFont(font);
    text.setString ("\t\t\t\t\t GAME OVER\nPRESS R TO RESTART OR ESC TO EXIT");
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
        "\t------------------------------\n"
        "\t| Self Driving Matatu |\n"
        "\t------------------------------\n"
        "Press SPACE to Start Game\n"
        "Press 1. For instructions"
            );

    text.setCharacterSize(35);
    text.setPosition(WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT / 2 - 140);

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
      // default full speed

    Matatu matatu;

    // Textures for player and obstacles.
    // Place matatu.png and obstacle.png in the same folder as the .exe,
    // or change these paths to point at images on your system.
    // If either file fails to load, the game falls back to plain rectangles
    // automatically, so it will still run without the images.
    Texture playerTexture;
    Texture enemyTexture[3];

    bool playerTexOk = playerTexture.loadFromFile("matatu.png");
    bool enemyTexOk =
        enemyTexture[0].loadFromFile("obstacle.png") &&
        enemyTexture[1].loadFromFile("obstacle1.png") &&
        enemyTexture[2].loadFromFile("obstacle2.png");

    bool paused = false;
    bool gameStarted = false;
    float roadOffset = 0.f;
    bool nightMode = false;
    bool showInstructions = false;
    Clock modeClock;
    Clock changeLaneClock;



    initEnemyShapes(enemyTexture);

    if (playerTexOk)
        matatu.setTexture(playerTexture);

    // Font for GAME OVER text.
    // Place a .ttf file (e.g. arial.ttf) in the same folder as the .exe,
    // or change this path to point at one on your system.
    Font font;
    bool hasFont = font.loadFromFile("arial.ttf");
    bool gameOver = false;
    bool aiEnabled = true;   // starts in AI mode; player can press P to toggle to manual

    while (window.isOpen())
    {
         sleep(milliseconds(1));
        // ---- EVENTS / INPUT ----
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Escape)
                {
                    if (showInstructions)
                    {
                        showInstructions = false;
                    }
                    else if (gameOver == true)
                        window.close();     // ESC on the game-over screen exits
                    else
                        gameOver = true;    // ESC mid-game ends the round early
                }
                if (event.key.code == Keyboard::Space)
                {
                    if (!gameStarted && !showInstructions)
                    {                         
                        gameStarted = true;   // start game ✅
                    }
                    else if (!gameOver)
                    {
                        paused = !paused;     // pause after game starts ✅
                    }
                }
                if (event.key.code == Keyboard::Num1)
                {
                    showInstructions = true;
                }
                if (event.key.code == Keyboard::R)
                {
                    resetGame(matatu, playerTexture);
                    for (int i = 0; i < MAX_OBSTACLES; i++)
                    active[i] = false;
                    
                    gameOver = false;
                    paused = false;
                    gameStarted = false;   // ✅ important
                    gameSpeed = 5.f;            // reset speed to default
                }


                if (event.key.code == Keyboard::P)
                {
                    aiEnabled = !aiEnabled;   // toggle between AI-driven and manual control
                }

                if (!gameOver && !aiEnabled)
                {
                    if (event.key.code == Keyboard::A || event.key.code == Keyboard::Left)
                        matatu.changeLane("left");
                    if (event.key.code == Keyboard::D || event.key.code == Keyboard::Right)
                        matatu.changeLane("right");
                    if (event.key.code == Keyboard::W || event.key.code == Keyboard::Up)
                    {
                        gameSpeed += SPEED_STEP;
                        if (gameSpeed > MAX_SPEED)
                        gameSpeed = MAX_SPEED;
                    }
                    
                    if (event.key.code == Keyboard::S || event.key.code == Keyboard::Down)
                    {
                        gameSpeed -= SPEED_STEP;
                        if (gameSpeed < MIN_SPEED)
                        gameSpeed = MIN_SPEED;
                    }

                }
            }
        }

        // ---- UPDATE ----
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

                if (changeLaneClock.getElapsedTime().asMilliseconds() > 500)  // .5 seconds cooldown between lane changes
                {
                    if (decision == "LEFT") {
                        matatu.changeLane("left");
                        changeLaneClock.restart();
                        }
                    else if (decision == "RIGHT") {
                        matatu.changeLane("right");
                        changeLaneClock.restart();
                    }
                    else if (decision == "BRAKE") {
                        braking = true;
                    }
                }
            }
            // Update traffic light first
            updateTrafficLight();
                
            // Determine movement speed
            float targetSpeed = 1.0f;
            braking = false;

            if (aiEnabled)
            {
                if (trafficLight == AMBER)
                {
                    targetSpeed = 0.5f;    // slow down
                }
                else if (trafficLight == RED)
                {
                    targetSpeed = 0.0f;    // complete stop
                }
                if (movementFactor < targetSpeed)
                {
                    movementFactor += 0.01f;
                    if (movementFactor > targetSpeed)
                    {
                        movementFactor = targetSpeed;
                    }
                }
                if (movementFactor > targetSpeed)
                {
                    movementFactor -= 0.01f;
                    if (movementFactor < targetSpeed)
                    {
                        movementFactor = targetSpeed;
                    }
                }
            }

            
            // Move road
            
            roadOffset += gameSpeed * movementFactor;

            if (roadOffset > 50.f)
            {
                roadOffset = 0.f;
            }
            
            // Move zebra crossing
            zebraY += gameSpeed * movementFactor;
            if (zebraY > WINDOW_HEIGHT)
            {
                zebraY = -(rand() % 3000 + 1500);
                zebraCrossed = false;
            }
                
            // Move enemies
            for (int i = 0; i < MAX_OBSTACLES; i++)
            {
                if (active[i])
                {
                    enemyY[i] += gameSpeed * movementFactor;
                    if (enemyY[i] > WINDOW_HEIGHT + CAR_HEIGHT)
                    {
                        active[i] = false;
                    }
                }
            }
        }
            
        
        if (checkCollision(matatu))
        {
            gameOver = true;
        }
      
    // ---- DRAW ----

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
