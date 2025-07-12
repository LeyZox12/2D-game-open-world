#pragma once

#include <SFML/Graphics.hpp>
#include "BlockRegistry.hpp"

using namespace sf;

extern const int BLOCK_SIZE;
extern const int PLAYER_SPEED;
extern const int WORLD_SIZE;
extern const int CHUNK_SIZE;
extern const int LIGHT_CHUNK_DRAW_COUNT;
extern const int LIQUID_PER_BOX_ROW;
extern const float WATER_PARTICLE_RADIUS;
extern RenderWindow window;
extern Shader renderShader;
extern Shader lightBakeShader;
extern Shader drawLightShader;
extern BlockRegistry registry;
