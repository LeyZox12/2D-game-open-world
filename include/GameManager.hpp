
#pragma once

#include "Player.h"
#include "globals.hpp"

#include <chrono>
#include <iostream>
#include <thread>

struct GameManager
{



    Texture lightChunks;
    Texture bakedTexture;
    vec2 lights[100];
    RectangleShape rect;
    Vector3f lightColors[100];
    vec2 water[10000];
    int lightCount = 0;
    int waterCount = 0;
    GameManager()
    {

    }
    int circleRectCollision(Rect<float> rect, CircleShape circle)
    {
        float bx = circle.getPosition().x;
        float by = circle.getPosition().y;
        float br = circle.getRadius();
        float rx = rect.position.x;
        float ry = rect.position.y;
        float sx = rect.size.x;
        float sy = rect.size.y;
        int collision = -1;
        vector<float> distances = {
                                    abs(by + br - ry),//TOP
                                    abs(by - (ry + sy)), // BOTTOM
                                    abs(bx + br - rx), // LEFT
                                    abs(bx - br -(rx + sx)) // RIGHT
                                  };
        int collisionIndex = distance(distances.begin(), min_element(distances.begin(), distances.end()));
        if(distances[collisionIndex] < br && bx + br > rx && bx - br < rx + sx && by + br > ry && by - br < ry + sy)
            collision = collisionIndex;
        return collision;
    }
    int rectRectCollision(RectangleShape r1, RectangleShape r2)
    {
        vec2 p1 = r1.getPosition();
        vec2 s1 = r1.getSize();
        vec2 p2 = r2.getPosition();
        vec2 s2 = r2.getSize();
        if(p1.x + s1.x > p2.x && p1.x < p2.x + s2.x && p1.y + s1.y > p2.y && p1.y < p2.y + s2.y)
        {
            vector<float> distances =
            {
                abs(p1.x + s1.x - p2.x), //Left
                abs(p2.x + s2.x - p1.x), //Right
                abs(p1.y + s1.y - p2.y), //Up
                abs(p2.y + s2.y - p1.y) //Down
            };
            auto it = min_element(distances.begin(), distances.end());
            int index = distance(distances.begin(), it);
            return index;
        }
        return -1;
    }

    void updateLightCache(vec2 pos)
    {
        static RenderTexture rt(Vector2u(CHUNK_SIZE * BLOCK_SIZE * LIGHT_CHUNK_DRAW_COUNT, CHUNK_SIZE * BLOCK_SIZE * LIGHT_CHUNK_DRAW_COUNT));
        rect.setSize({CHUNK_SIZE * BLOCK_SIZE * LIGHT_CHUNK_DRAW_COUNT, CHUNK_SIZE * BLOCK_SIZE * LIGHT_CHUNK_DRAW_COUNT});
        lightBakeShader.setUniform("resolution", vec2(rect.getSize()));
        lightBakeShader.setUniformArray("lightPos", lights, 100);
        //calcul du coin haut gauche du chunk le plus a gauche affiché
        vec2 chunkPos = vec2(floor(pos.x / ((float)CHUNK_SIZE * (float)BLOCK_SIZE)) * CHUNK_SIZE * BLOCK_SIZE,
                             floor(pos.y / ((float)CHUNK_SIZE * (float)BLOCK_SIZE)) * CHUNK_SIZE * BLOCK_SIZE);
        vec2 currentChunkOffset = pos - chunkPos;
        int half = floor(LIGHT_CHUNK_DRAW_COUNT / 2.0f);
        int pixelChunkSize = BLOCK_SIZE * CHUNK_SIZE;
        vec2 totalChunkOffset = -currentChunkOffset - vec2(pixelChunkSize, pixelChunkSize) * (float)half;
        lightBakeShader.setUniform("topLeftPos", pos+totalChunkOffset);
        rect.setPosition({0, 0});
        rt.clear();
        rt.draw(rect, &lightBakeShader);
        rt.display();

        rect.setPosition(pos + totalChunkOffset);
        bakedTexture = rt.getTexture();
        rect.setTexture(&bakedTexture);
        drawLightShader.setUniform("lightChunks", bakedTexture);

    }

    void updateWater(float dt, Player player, vector<vector<int>> currentPlanet)
    {
        static vec2 waterVel[1000];
        for(int i = 0; i < waterCount; i++)
        {
            vec2 diff = player.getBody().getPosition() - water[i];
            float dist = hypot(diff.x, diff.y);
            if(dist < BLOCK_SIZE * CHUNK_SIZE * LIGHT_CHUNK_DRAW_COUNT)
            {

            }
            for(int w = 0; w < waterCount; w++)
            {
                if(w != i)
                {
                    vec2 diff = water[i] - water[w];
                    float dist = hypot(diff.x, diff.y);
                    if(dist > 0.0)
                    {
                        diff /= dist;
                        waterVel[i] += diff * 0.5f/(dist);
                    }
                }
            }
            vec2 indexPos = vec2(floor(water[i].x / (float)BLOCK_SIZE), floor(water[i].y / (float)BLOCK_SIZE));
            for(int y = -3; y < 3; y++)
            {
                for(int x = -3; x < 3; x++)
                {
                    vec2 fixedIndexPos = indexPos + vec2(x, y);
                    if(currentPlanet[fixedIndexPos.y][fixedIndexPos.x] != 0)
                    {
                        CircleShape circle(WATER_PARTICLE_RADIUS);
                        circle.setPosition(water[i]);
                        circle.setOrigin({WATER_PARTICLE_RADIUS, WATER_PARTICLE_RADIUS});


                        int currentCollision = circleRectCollision(FloatRect({fixedIndexPos * (float)BLOCK_SIZE}, {BLOCK_SIZE, BLOCK_SIZE}), circle);
                        switch(currentCollision)
                        {
                            case(0):
                                waterVel[i].y = 0;
                                waterVel[i].x *= 0.9;
                                water[i].y = fixedIndexPos.y * BLOCK_SIZE - WATER_PARTICLE_RADIUS;
                            break;
                            case(2):
                                if(waterVel[i].x > 0.0)
                                    waterVel[i].x *= -0.2;
                                water[i].x = fixedIndexPos.x * BLOCK_SIZE - WATER_PARTICLE_RADIUS;
                            break;
                            case(3):
                                if(waterVel[i].x < 0.0)
                                waterVel[i].x *= -0.2;
                                water[i].x = fixedIndexPos.x * BLOCK_SIZE + BLOCK_SIZE + WATER_PARTICLE_RADIUS;
                            break;
                        }
                    }
                }
            }
            waterVel[i].y += 4.8 * dt;
            water[i] += waterVel[i];
        }

        renderShader.setUniformArray("waterDrops", water, 1000);
        renderShader.setUniform("waterCount", waterCount);
        renderShader.setUniform("waterRadius", WATER_PARTICLE_RADIUS);
    }

    void applyCollision(Player& player, vector<vector<int>> planetTerrain)
    {
        vec2 r1 = player.getBody().getPosition();
        vec2 s1 = player.getBody().getSize();
        vec2 worldPos = player.getWorldPos();
        RectangleShape block;
        block.setSize({BLOCK_SIZE, BLOCK_SIZE});
        for(int i = -8; i < 8; i++)
        {
            for(int j = -8; j < 8; j++)
            {
                vec2 check = vec2(worldPos.x + i, worldPos.y + j);
                if(check.x < planetTerrain.size() && check.x >= 0 && check.y < planetTerrain.size() && check.y >= 0 &&
                   !registry.getBlockById(planetTerrain[check.y][check.x]).hasCollision)
                {
                    block.setPosition({check.x * BLOCK_SIZE, check.y * BLOCK_SIZE});

                    int edge = rectRectCollision(player.getBody(), block);
                    switch(edge)
                    {
                        case(2):
                            player.setVel(vec2(player.getVel().x * 0.95f, 0));
                            player.setPosition(vec2(player.getPosition().x, player.getWorldPos().y * BLOCK_SIZE));
                        break;
                    }

                }
            }
        }
    }
};
