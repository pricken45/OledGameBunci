
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

class Enemy
{
public:
    float x;
    int y;
    float direction;
    byte speed;
    bool alive;
    Adafruit_SSD1306 *disp;
    void newPos()
    {
        if (random(0.0f, 3.0f) > 1.5f)
        {
            this->direction = 1;
            this->x = -5 - random(10, 45);
        }
        else
        {
            this->direction = -1;
            this->x = 128 + random(10, 45);
        }

        this->y = random(64);
    }
    Enemy(Adafruit_SSD1306 *display, int spd, bool alv)
    {
        this->disp = display;
        this->speed = spd;

        this->alive = alv;
        x = 0;
        y = 0;
        direction = 0;
        newPos();
    };

    void render()
    {
        if (!this->alive)
            return;
        disp->drawLine(this->x, this->y, this->x + this->direction * -1 * 5, this->y, WHITE);
    };

    void update()
    {
        if (!this->alive)
            return;
        x += direction * speed;

        if (x > 128 + 10 || x < -15)
        {
            this->newPos();
        }
    }

    bool colliding(float px, float py, float lx, float ly)
    {
        bool coll = false;

        if (py > ly)
        {
            for (float i = ly; i < py; i += 0.1)
            {
                if (i > y - 2 && i < y + 3)
                {
                    if (px >= this->x - 1 && px <= this->x + 4)
                    {
                        coll = true;
                    }
                }
            }
        }
        else
        {
            for (float i = py; i < ly; i += 0.1)
            {
                if (i > y - 2.0f && i < y + 3.0f)
                {
                    if (px >= this->x - 1 && px <= this->x + 4)
                    {
                        coll = true;
                    }
                }
            }
        }

        return coll;
    }
};