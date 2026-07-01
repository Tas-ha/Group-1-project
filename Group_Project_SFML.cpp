#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>

using namespace sf;
using namespace std;

// =====================
// SETTINGS
// =====================
const int WINDOW_WIDTH  = 700;
const int WINDOW_HEIGHT = 700;

const int LANE_COUNT   = 3;
const int LANE_WIDTH   = 120;
const int ROAD_LEFT    = 170;              // left edge of road
const int ROAD_RIGHT   = ROAD_LEFT + LANE_COUNT * LANE_WIDTH;

const float CAR_WIDTH   = 60.f;
const float CAR_HEIGHT  = 90.f;
const float PLAYER_Y    = WINDOW_HEIGHT - 130.f;   // fixed screen y for player
const float SPEED       = 5.f;                     // pixels/frame obstacles fall
const float SPAWN_GAP_Y = 310.f;                    // min vertical gap between obstacles when spawning

// AI reaction tuning:
// AI_REACT_DISTANCE - how far ahead the AI "sees" an obstacle and starts to react
// LANE_CLEAR_GAP     - how much clear space is required in a lane before the AI will swerve into it
// LANE_CLEAR_GAP is intentionally smaller than AI_REACT_DISTANCE so the AI reacts
// with room to spare, instead of only reacting once already at the minimum safe gap
const float AI_REACT_DISTANCE = 300.f;
const float LANE_CLEAR_GAP    = 220.f;

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
void initEnemyShapes(Texture &tex)
{
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        enemyShape[i].setSize(Vector2f(CAR_WIDTH, CAR_HEIGHT));
        enemyShape[i].setFillColor(Color::Red);
        enemyShape[i].setOrigin(CAR_WIDTH / 2.f, CAR_HEIGHT / 2.f);
    }

    if (tex.getSize().x > 0)   // loadFromFile succeeded
    {
        for (int i = 0; i < MAX_OBSTACLES; i++)
        {
            enemySprite[i].setTexture(tex);
            enemySprite[i].setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);
            enemySprite[i].setScale(CAR_WIDTH / tex.getSize().x,
                                     CAR_HEIGHT / tex.getSize().y);
        }
        enemyUseSprite = true;
    }
}

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

    bool yTooClose;
    float newY;

    do
    {
        yTooClose = false;
        newY = -(rand() % 150 + 60);   // spawn above the visible window

        for (int j = 0; j < MAX_OBSTACLES; j++)
        {
            if (j != i && active[j] && fabs(newY - enemyY[j]) < SPAWN_GAP_Y)
            {
                yTooClose = true;
                break;
            }
        }
    } while (yTooClose);

    enemyY[i] = newY;
    active[i] = true;
}

void updateEnemy(int i)
{
    if (!active[i]) return;

    enemyY[i] += SPEED;

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
// AI (ported straight from the console version)
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
    float minDist = 100000.f;

    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        if (active[i] && enemyLane[i] == lane)
        {
            float dist = enemyY[i];
            if (dist < minDist)
                minDist = dist;
        }
    }
    return minDist;
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
    bool obstacleAhead = detectObstacleAhead(m);
    bool leftFree  = checkLaneAvailability("left", m);
    bool rightFree = checkLaneAvailability("right", m);

    if (!obstacleAhead)
        return "STRAIGHT";

    if (leftFree && rightFree)
    {
        float leftScore  = laneSafetyScore(m.getLane() - 1);
        float rightScore = laneSafetyScore(m.getLane() + 1);
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
        active[i] = false;

    m = Matatu();   // re-run the constructor to reset lane/position

    // re-apply the texture, since the fresh Matatu() above resets useSprite to false
    if (playerTexture.getSize().x > 0)
        m.setTexture(playerTexture);
}

// =====================
// ROAD DRAWING
// =====================
void drawRoad(RenderWindow &window)
{
    RectangleShape roadBg(Vector2f(ROAD_RIGHT - ROAD_LEFT, WINDOW_HEIGHT));
    roadBg.setPosition(ROAD_LEFT, 0);
    roadBg.setFillColor(Color(40, 40, 40));
    window.draw(roadBg);

    for (int lane = 0; lane <= LANE_COUNT; lane++)
    {
        float x = ROAD_LEFT + lane * LANE_WIDTH;
        RectangleShape line(Vector2f(3.f, WINDOW_HEIGHT));
        line.setPosition(x, 0);
        line.setFillColor(Color::White);
        window.draw(line);
    }
}

// =====================
// MAIN
// =====================
int main()
{
    srand((unsigned int)time(0));

    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Matatu Dodge - SFML");
    window.setFramerateLimit(60);

    Matatu matatu;

    // Textures for player and obstacles.
    // Place matatu.png and obstacle.png in the same folder as the .exe,
    // or change these paths to point at images on your system.
    // If either file fails to load, the game falls back to plain rectangles
    // automatically, so it will still run without the images.
    Texture playerTexture;
    Texture enemyTexture;

    bool playerTexOk = playerTexture.loadFromFile("matatu.png");
    bool enemyTexOk  = enemyTexture.loadFromFile("obstacle.png");
    bool paused = false;


    initEnemyShapes(enemyTexture);

    if (playerTexOk)
        matatu.setTexture(playerTexture);

    // Font for GAME OVER text.
    // Place a .ttf file (e.g. arial.ttf) in the same folder as the .exe,
    // or change this path to point at one on your system.
    Font font;
    bool hasFont = font.loadFromFile("arial.ttf");

    Text gameOverText;
    if (hasFont)
    {
        gameOverText.setFont(font);
        gameOverText.setString("GAME OVER\nPRESS R TO RESTART OR ESC TO EXIT");
        gameOverText.setCharacterSize(36);
        gameOverText.setFillColor(Color::White);
        FloatRect tb = gameOverText.getLocalBounds();
        gameOverText.setOrigin(tb.width / 2.f, tb.height / 2.f);
        gameOverText.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);
    }

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
                    if (gameOver == true)
                        window.close();     // ESC on the game-over screen exits
                    else
                        gameOver = true;    // ESC mid-game ends the round early
                }

                if (event.key.code == Keyboard::Space && !gameOver)
                {
                    paused = !paused;   // toggle pause ✅
                }

                if (event.key.code == Keyboard::R)
                {
                    resetGame(matatu, playerTexture);
                    gameOver = false;
                }

                if (event.key.code == Keyboard::P)
                {
                    aiEnabled = !aiEnabled;   // toggle between AI-driven and manual control
                }

                if (!gameOver && !aiEnabled)
                {
                    if (event.key.code == Keyboard::A)
                        matatu.changeLane("left");
                    if (event.key.code == Keyboard::D)
                        matatu.changeLane("right");
                }
            }
        }

        // ---- UPDATE ----
        if (!gameOver && !paused)
        {
            if (!active[0])
                genEnemy(0);

            if (!active[1] && enemyY[0] > 100.f)
                genEnemy(1);

            if (!active[2] && enemyY[1] > 100.f)
                genEnemy(2);

            if (aiEnabled)
            {
                string decision = autoDecision(matatu);
                if (decision == "LEFT")  matatu.changeLane("left");
                if (decision == "RIGHT") matatu.changeLane("right");
            }

            for (int i = 0; i < MAX_OBSTACLES; i++)
                updateEnemy(i);

            if (checkCollision(matatu))
                gameOver = true;
        }

        // ---- DRAW ----
        window.clear(Color::Black);

        if (gameOver)
        {
            // game-over screen: nothing else is drawn, only the message
            if (hasFont)
                window.draw(gameOverText);
        }
        else
        {
            drawRoad(window);

            for (int i = 0; i < MAX_OBSTACLES; i++)
                drawEnemy(i, window);

            matatu.draw(window);
        }

        window.display();
    }

    return 0;
}