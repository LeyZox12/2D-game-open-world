#include "globals.hpp"

const int BLOCK_SIZE = 100;
const int PLAYER_SPEED = 2;
const int WORLD_SIZE = 1000;
const int CHUNK_SIZE = 16;
const int LIGHT_CHUNK_DRAW_COUNT = 4;
const int LIQUID_PER_BOX_ROW = 2;
const float WATER_PARTICLE_RADIUS = 10.0f;
RenderWindow window(VideoMode({512, 512}), "Silly Game...");
Shader renderShader;
Shader lightBakeShader;
Shader drawLightShader;
BlockRegistry registry;
