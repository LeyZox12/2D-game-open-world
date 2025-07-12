#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <functional>
#include <any>
#include <vector>
#include "BlockContext.hpp"

using namespace std;
using namespace sf;

typedef Vector2f vec2;

struct Block
{

    Block(string name, Vector3i mapColor, vec2 spriteSheetPos, function<vector<any>(BlockContext ctx)> onInteract, function<vector<any>(BlockContext ctx)> onPlace, bool hasCollision)
    :name(name), mapColor(mapColor), onInteract(onInteract), onPlace(onPlace), hasCollision(hasCollision) {vector<vec2> vec; vec.push_back(spriteSheetPos); this->spriteSheetPos = vec; lit = false;};
    Block(string name, Vector3i mapColor, vector<vec2> spriteSheetPos, function<vector<any>(BlockContext ctx)> onInteract, function<vector<any>(BlockContext ctx)> onPlace, bool hasCollision)
    :name(name), mapColor(mapColor), spriteSheetPos(spriteSheetPos), onInteract(onInteract), onPlace(onPlace), hasCollision(hasCollision) {lit = false;};
    Block(string name, Vector3i mapColor, vec2 spriteSheetPos, function<vector<any>(BlockContext ctx)> onInteract, function<vector<any>(BlockContext ctx)> onPlace, bool hasCollision, vec2 lightPos/*as % e.g 0.5*/, Vector3f lightColor)
    :name(name), mapColor(mapColor), onInteract(onInteract), onPlace(onPlace), hasCollision(hasCollision), lightPos(lightPos), lightColor(lightColor) {vector<vec2> vec; vec.push_back(spriteSheetPos); this->spriteSheetPos = vec; lit = true;};
    Block(string name, Vector3i mapColor, vector<vec2> spriteSheetPos, function<vector<any>(BlockContext ctx)> onInteract, function<vector<any>(BlockContext ctx)> onPlace, bool hasCollision, vec2 lightPos/*as % e.g 0.5*/, Vector3f lightColor)
    :name(name), mapColor(mapColor), spriteSheetPos(spriteSheetPos), onInteract(onInteract), onPlace(onPlace), hasCollision(hasCollision), lightPos(lightPos), lightColor(lightColor) {lit = true;};

    int getId()
    {
        return mapColor.x + mapColor.y + mapColor.z;
    }

    vector<any> args;
    function<vector<any>(BlockContext ctx)> onInteract;
    function<vector<any>(BlockContext ctx)> onPlace;
    Vector3i mapColor;
    vector<vec2> spriteSheetPos;
    bool hasCollision;
    bool lit;
    vec2 lightPos;
    Vector3f lightColor;
    string name;
};
