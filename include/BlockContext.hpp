#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <functional>
#include <any>
#include <vector>


class Player;
struct GameManager;
struct Block;
struct Planet;

using namespace std;
using namespace sf;

typedef Vector2f vec2;

struct BlockContext
{
    BlockContext(RenderWindow& window, Player& player, Block& thisBlock, vec2 mousepos, vector<any> args, GameManager& gm, Planet& currentPlanet, vec2 blockPosAsIndex)
    :window(window), player(player), thisBlock(thisBlock), mousepos(mousepos), args(args), gm(gm), currentPlanet(currentPlanet), blockPosAsIndex(blockPosAsIndex) {};
    RenderWindow& window;
    Player& player;
    Block& thisBlock;
    vec2 mousepos;
    vector<any> args;
    GameManager& gm;
    Planet& currentPlanet;
    vec2 blockPosAsIndex;
};
