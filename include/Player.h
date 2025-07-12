#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "globals.hpp"
#include <math.h>

using namespace std;
using namespace sf;

typedef Vector2f vec2;


class Player
{
    public:
        Player();
        void init();
        void setWorldPos(vec2 pos);
        void setPosition(vec2 pos);
        vec2 getPosition();
        void move(vec2 offset);
        void setVel(vec2 vel);
        void addVel(vec2 vel);
        vec2 getVel();
        RectangleShape getHead();
        RectangleShape getFace();

        RectangleShape getBody();
        vec2 getWorldPos();
        void update(float dt, vec2 mousepos);
        void display(RenderWindow& window);

        private:
            Texture spriteSheet;
            bool grounded;
            int inventoryIndex = 0;
            vec2 pos;
            vec2 vel;
            vector<int> inventory;
            vector<string> voiceLines;
            RectangleShape body;
            RectangleShape head;
            RectangleShape face;
};

#endif // PLAYER_H
