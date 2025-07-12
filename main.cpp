#include <iostream>
#include <SFML/Graphics.hpp>
#include <math.h>
#include "../../class/UIutils.h"
#define pi 3.1415926
#define rad pi/180
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <future>
#include <Blocks.hpp>
#include "BlockContext.hpp"
#include "GameManager.hpp"
#include "globals.hpp"
///TODO FIX WEIRD DISPLAY FOR WATER
using namespace std;
using namespace sf;

typedef Vector2f vec2;
typedef Vector2i vec2i;


sf::Clock deltaClock;


RectangleShape bg({512, 512});

View camera(FloatRect({0, 0}, {512.f, 512.f}));
vector<Keyboard::Key> heldKeys;
bool lights = true;
Font font;
RectangleShape warningDisplayRect;
Text warningDisplayText(font, "", 30);
vec2 mousepos;

struct Planet
{
    vector<int> row;
    vector<vector<int>> terrain;
    Texture asTexture;
    vector<uint8_t> data;
    void init()
    {
        asTexture = Texture({1000, 1000});
        data = vector<uint8_t>(1000 * 1000 * 4);
        for(int i = 0; i < 1000; i++)
            row.push_back(0);
        for(int i = 0; i < 1000; i++)
            terrain.push_back(row);
    }
    void generateWorld(int seed)
    {
        srand(seed);
        int randOffset = rand();
        Block stone = registry.getBlockByName("Stone");
        for(int i = 0; i < 1000; i++)
        {
            int y = 0;
            for(int j = 0; j < 1000 - 500 + y; j++)
            {
                terrain[500+y+j][i] = 1;
                data[4000 * 500 + j + i * 4] = 1;
                data[4000 * 500 + j + i * 4 + 1] = 0;
                data[4000 * 500 + j + i * 4 + 2] = 0;
                data[4000 * 500 + j + i * 4 + 3] = 255;

            }
        }
        asTexture.update(data.data());
    }
    void placeBlock(vec2 pos, Block b, GameManager& gm, Player& player)
    {
        pos = vec2(floor(pos.x / (float)BLOCK_SIZE), floor(pos.y / (float)BLOCK_SIZE));

        if(b.lit)
        {
            gm.lights[gm.lightCount] = pos * (float)BLOCK_SIZE + b.lightPos * (float)BLOCK_SIZE + vec2(BLOCK_SIZE / 2.0f, BLOCK_SIZE / 2.0f );
            gm.lightColors[gm.lightCount] = b.lightColor;
            gm.lightCount++;
        }
        else
        {
            terrain[pos.y][pos.x] = b.getId();
            data[4000 * pos.y + pos.x * 4] = b.mapColor.x;
            data[4000 * pos.y + pos.x * 4 + 1] = b.mapColor.y;
            data[4000 * pos.y + pos.x * 4 + 2] = b.mapColor.z;
            data[4000 * pos.y + pos.x * 4 + 3] = 255;
            asTexture.update(data.data());
        }
        //Baking light with new block
        lightBakeShader.setUniformArray("lightPos", gm.lights, 100);
        lightBakeShader.setUniformArray("lightColors", gm.lightColors, 100);
        lightBakeShader.setUniform("lightCount", gm.lightCount);
        gm.updateLightCache(pos * (float)BLOCK_SIZE);
        b.onPlace(BlockContext(window, player, b, mousepos, b.args, gm, *this, pos));
    }

    void removeBlock(vec2 pos, GameManager& gm)
    {
        pos = vec2(floor(pos.x / (float)BLOCK_SIZE), floor(pos.y / (float)BLOCK_SIZE));
        terrain[pos.y][pos.x] = 0;
        data[4000 * pos.y + pos.x * 4] = 0;
        data[4000 * pos.y + pos.x * 4 + 1] = 0;
        data[4000 * pos.y + pos.x * 4 + 2] = 0;
        data[4000 * pos.y + pos.x * 4 + 3] = 0;
        asTexture.update(data.data());
        //Baking light with removed block
        lightBakeShader.setUniformArray("lightPos", gm.lights, 100);
        lightBakeShader.setUniformArray("lightColors", gm.lightColors, 100);
        lightBakeShader.setUniform("lightCount", gm.lightCount);
        gm.updateLightCache(pos * (float)BLOCK_SIZE);
    }

    Texture &getAsTex()
    {
        asTexture.update(data.data());
        return asTexture;
    }
};

Player player = Player();
GameManager gm;
Planet testPlanet;
Texture spriteSheet;
int currentItem = 0;

void inputManager(optional<Event> e);
void applyInputs();
void setWarningMessage(string message);
void onResize(Vector2u size);

void start()
{
    import(registry);
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(60);
    player.init();
    player.setWorldPos(vec2(500, 450));
    renderShader.loadFromFile("res/Shaders/OnPlanetRender.frag", Shader::Type::Fragment);
    lightBakeShader.loadFromFile("res/Shaders/LightBake.frag", Shader::Type::Fragment);
    drawLightShader.loadFromFile("res/Shaders/LightDraw.frag", Shader::Type::Fragment);
    testPlanet.init();
    testPlanet.generateWorld(0);

    spriteSheet.loadFromFile("res/spriteSheet.png");

    renderShader.setUniform("resolution", vec2(512, 512));
    renderShader.setUniform("BLOCK_SIZE", (float)BLOCK_SIZE);
    renderShader.setUniform("planet", testPlanet.getAsTex());
    renderShader.setUniform("spriteSheet", spriteSheet);
    renderShader.setUniform("spriteSheetRes", vec2(spriteSheet.getSize().x, spriteSheet.getSize().y));
    renderShader.setUniform("spriteSheetUnit", vec2(16, 16));
    drawLightShader.setUniform("resolution", vec2(512, 512));
    drawLightShader.setUniform("BLOCK_SIZE", (float)BLOCK_SIZE);
    drawLightShader.setUniform("CHUNK_SIZE", (float)CHUNK_SIZE);

    lightBakeShader.setUniform("BLOCK_SIZE", (float)BLOCK_SIZE);
    lightBakeShader.setUniform("WORLD_SIZE", (float)WORLD_SIZE);
    renderShader.setUniform("WORLD_SIZE", (float)WORLD_SIZE);
    lightBakeShader.setUniform("planet", testPlanet.getAsTex());
    lightBakeShader.setUniform("spriteSheet", spriteSheet);
    lightBakeShader.setUniform("spriteSheetRes", vec2(spriteSheet.getSize().x, spriteSheet.getSize().y));
    lightBakeShader.setUniform("spriteSheetUnit", vec2(16, 16));
    font.openFromFile("res/font.ttf");
    warningDisplayText.setFont(font);
    warningDisplayText.setString("testText");
    warningDisplayText.setFillColor(Color::Transparent);
    warningDisplayRect.setFillColor(Color::Transparent);
    warningDisplayRect.setTexture(&spriteSheet);
    warningDisplayRect.setTextureRect(IntRect({0, 64}, {32, 16}));
    onResize(Vector2u(512, 512));

}

int main()
{
    static int f = 0;
    start();
    while(window.isOpen())
    {


        float dt = deltaClock.restart().asSeconds();
        mousepos = window.mapPixelToCoords(Mouse::getPosition(window));
        if(f++%100 == 0)
            window.setTitle("Silly Game FPS:" + to_string(int(1.0f / dt)));
        while(optional<Event> e = window.pollEvent())
        {
            if(e->is<Event::Closed>()) window.close();
            if(e->is<Event::Resized>())
            {
                Vector2u size = e->getIf<Event::Resized>() -> size;
                onResize(size);
            }
            if(e->is<Event::MouseWheelScrolled>())
            {
                currentItem = (currentItem + 1) % registry.blocks.size();
                setWarningMessage(registry.blocks[currentItem].name);
            }
            if(e->is<Event::MouseButtonPressed>() && e->getIf<Event::MouseButtonPressed>() -> button == Mouse::Button::Left)
                testPlanet.placeBlock(mousepos, registry.blocks[currentItem], gm, player );
            if(e->is<Event::MouseButtonPressed>() && e->getIf<Event::MouseButtonPressed>() -> button == Mouse::Button::Right)
                testPlanet.removeBlock(mousepos, gm);
            inputManager(e);
        }
        Color col = warningDisplayRect.getFillColor();

        warningDisplayRect.setFillColor(Color(255, 255, 255, col.a - dt * 10.0f));
        warningDisplayText.setFillColor(Color(255, 255, 255, col.a - dt * 10.0f));
        applyInputs();
        warningDisplayRect.setPosition(bg.getPosition() + vec2(window.getSize().x / 2.0f, window.getSize().y * 0.75f));
        warningDisplayText.setPosition(warningDisplayRect.getPosition());
        camera.setCenter(player.getHead().getPosition());
        window.setView(camera);
        player.update(dt, mousepos);
        gm.updateWater(dt, player, testPlanet.terrain);
        gm.applyCollision(player, testPlanet.terrain);
        bg.setPosition(window.mapPixelToCoords(vec2i(0,0)));
        renderShader.setUniform("topLeftPos", bg.getPosition());
        drawLightShader.setUniform("topLeftPos", bg.getPosition());
        drawLightShader.setUniform("texturePosStart", gm.rect.getPosition());
        drawLightShader.setUniform("texturePosEnd", gm.rect.getPosition() + gm.rect.getSize());

        renderShader.setUniform("mousepos", mousepos);
        renderShader.setUniform("playerPos", player.getBody().getPosition());
        renderShader.setUniform("headPos", player.getHead().getPosition());
        renderShader.setUniform("facePos", player.getFace().getPosition());
        window.clear();

        window.draw(bg, &drawLightShader);
        window.draw(bg, &renderShader);
        window.draw(warningDisplayRect);
        window.draw(warningDisplayText);
        window.display();
    }
    return 0;
}

void setWarningMessage(string message)
{

    warningDisplayRect.setFillColor(Color(255, 255, 255, 255));
    warningDisplayText.setFillColor(Color(255, 255, 255, 255));
    warningDisplayText.setString(message);
}

void inputManager(optional<Event> e)
{
    if(e->is<Event::KeyPressed>())
    {
        Keyboard::Key key = e->getIf<Event::KeyPressed>() -> code;
        if(count(heldKeys.begin(), heldKeys.end(), key) == 0) heldKeys.push_back(key);
    }
    if(e->is<Event::KeyReleased>())
    {
        Keyboard::Key key = e->getIf<Event::KeyReleased>() -> code;
        if(key == Keyboard::Key::Space) lights = ! lights;
        if(count(heldKeys.begin(), heldKeys.end(), key) == 1)
            heldKeys.erase(find(heldKeys.begin(), heldKeys.end(), key));
    }
}

void onResize(Vector2u size)
{
    camera.setSize({size.x,size.y});
    bg.setSize({size.x, size.y});
    renderShader.setUniform("resolution", vec2(size.x, size.y));
    drawLightShader.setUniform("resolution", vec2(size.x, size.y));
    warningDisplayRect.setSize({window.getSize().x * 0.3f, window.getSize().y * 0.15f});
    warningDisplayRect.setOrigin(warningDisplayRect.getSize() / 2.0f);
    warningDisplayText.setOrigin(warningDisplayRect.getSize() / 2.35f);
    warningDisplayText.setCharacterSize(window.getSize().x * 0.02);
}

void applyInputs()
{
    for(auto& key : heldKeys)
    {
        switch(key)
        {
            case(Keyboard::Key::Q):
                player.addVel(vec2(-PLAYER_SPEED, 0));
            break;
            case(Keyboard::Key::D):
                player.addVel(vec2(PLAYER_SPEED, 0));
            break;
        }

    }
}
