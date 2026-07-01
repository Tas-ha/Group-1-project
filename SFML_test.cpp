#include <SFML/Graphics.hpp>
#include <cstdlib>
using namespace sf;

int main()
{
    RenderWindow window(VideoMode(800, 600), "Matatu Game");
    Font font;
    font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf");
    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(50);
    gameOverText.setFillColor(Color::White);
    gameOverText.setPosition(250, 250);
    bool gameOver = false;
    

    // ✅ lanes
    float laneX[3] = {250, 370, 490};
    int currentLane = 1;

    // ✅ matatu
    RectangleShape matatu(Vector2f(60, 30));
    matatu.setFillColor(Color::Green);
    float matY = 500;

    // ✅ enemies
    const int MAX_ENEMIES = 3;
    RectangleShape enemies[MAX_ENEMIES];
    int enemyLane[MAX_ENEMIES];
    float enemyY[MAX_ENEMIES];

    // ✅ initialize enemies ONCE
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i] = RectangleShape(Vector2f(60, 30));
        enemies[i].setFillColor(Color::Red);

        enemyLane[i] = rand() % 3;

        // ✅ proper spacing
        enemyY[i] = -i * 200;
    }

    while (window.isOpen())
    {
        // ✅ INPUT
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::A && currentLane > 0)
                    currentLane--;

                if (event.key.code == Keyboard::D && currentLane < 2)
                    currentLane++;
            }
        }

        // ✅ UPDATE enemy (movement)
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            enemyY[i] += 3;
            window.setFramerateLimit(60);

            enemies[i].setPosition(laneX[enemyLane[i]], enemyY[i]);

            // ✅ reset when off screen
            if (enemyY[i] > 600)
            {
                enemyY[i] = -200;
                enemyLane[i] = rand() % 3;
            }
        }
        // collision
        bool crashed = false;
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemyLane[i] == currentLane &&
                abs(enemyY[i] - matY) < 40)   // collision distance
                {
                    crashed = true;
                }
            }
            if (crashed)
            {
                window.close();
            }



        // ✅ update matatu position
        matatu.setPosition(laneX[currentLane], matY);

        // ✅ DRAW
        window.clear();

        window.draw(matatu);

        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            window.draw(enemies[i]);
        }

if (crashed)
{
    gameOver = true;
}

if (!gameOver)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemyY[i] += 4;

        enemies[i].setPosition(laneX[enemyLane[i]], enemyY[i]);

        if (enemyY[i] > 600)
        {
            enemyY[i] = -200;
            enemyLane[i] = rand() % 3;
        }
    }

    matatu.setPosition(laneX[currentLane], matY);
}

        if (gameOver)
{
    window.draw(gameOverText);
}
        window.display();

        
    }

    return 0;
}