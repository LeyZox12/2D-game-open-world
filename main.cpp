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
#include <Blocks.hpp>

using namespace std;
using namespace sf;

typedef Vector2f vec2;
typedef Vector2i vec2i;

RenderWindow window(VideoMode({512, 512}), "Alone...");

sf::Clock deltaClock;

const int BLOCK_SIZE = 100;
const int PLAYER_SPEED = 2;
RectangleShape bg({512, 512});
Shader renderShader;
Shader lightBakeShader;
Shader drawLightShader;
View camera(FloatRect({0, 0}, {512.f, 512.f}));
vector<Keyboard::Key> heldKeys;
bool lights = true;
Font font;
RectangleShape warningDisplayRect;
Text warningDisplayText(font, "", 30);
BlockRegistry registry;




class Player
{
    public:
        player()
        {

        }
        void init()
        {
            spriteSheet.loadFromFile("res/spriteSheet.png");
            head.setTexture(&spriteSheet);
            face.setTexture(&spriteSheet);
            body.setTexture(&spriteSheet);
            head.setTextureRect(IntRect({32, 32}, {16, 16}));
            face.setTextureRect(IntRect({48, 32}, {6, 3}));
            body.setTextureRect(IntRect({32, 32}, {16, 16}));
            cout << "init";
            head.setSize(vec2(BLOCK_SIZE, BLOCK_SIZE));
            body.setSize(vec2(BLOCK_SIZE, BLOCK_SIZE * 2));
            face.setSize(vec2(BLOCK_SIZE * 0.375, BLOCK_SIZE * 0.1875));
        }
        void setWorldPos(vec2 pos)
        {
            this -> pos = pos * (float)BLOCK_SIZE;
        }
        void setPosition(vec2 pos)
        {
            this -> pos = pos;
        }
        vec2 getPosition()
        {
            return pos;
        }
        void move(vec2 offset)
        {
            this -> pos += offset;
        }
        void setVel(vec2 vel)
        {
            this -> vel = vel;
        }
        void addVel(vec2 vel)
        {
            this -> vel += vel;
        }
        vec2 getVel()
        {
            return vel;
        }
        RectangleShape getHead()
        {
            return head;
        }

        RectangleShape getFace()
        {
            return face;
        }

        RectangleShape getBody()
        {
            return body;
        }

        vec2 getWorldPos()
        {
            return vec2(floor(pos.x / (float)BLOCK_SIZE), floor(pos.y / (float)BLOCK_SIZE));
        }
        void update(float dt, vec2 mousepos)
        {
            pos += vel;
            addVel(vec2(0, 9.8 * dt));
            body.setPosition(pos);
            head.move(vec2((body.getPosition().x - head.getPosition().x) / 10,
                           (body.getPosition().y - head.getPosition().y - head.getSize().y * 1.2) / 10));
            face.setPosition(head.getPosition() + vec2(head.getSize().x / 2 - face.getSize().x / 2,
                                                       head.getSize().y / 2 - face.getSize().y / 2));
            vec2 diff = mousepos - face.getPosition();
            float dist = sqrt(diff.x * diff.x + diff.y * diff.y);
            float mult = dist * 0.01 > 8 ? 8 : dist * 0.01;
            face.move(vec2(diff.x / dist * mult , diff.y / dist * mult));
        }
        void display(RenderWindow& window)
        {
            window.draw(body);
            window.draw(head);
            window.draw(face);
        }

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

struct GameManager
{
    const int CHUNK_SIZE = 16;
    const int LIGHT_CHUNK_DRAW_COUNT = 4;
    Texture lightChunks;
    Texture bakedTexture;
    vec2 lights[100];
    RectangleShape rect;
    Vector3f lightColors[100];
    int lightCount = 0;
    ///TODO DYNAMICALLY ASSIGN COLOR AND POSITIONS TO LIT OBJECTS
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
                   planetTerrain[check.y][check.x] != 0)
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
        for(int i = 0; i < 1000; i++)
        {
            int y = 0;
            terrain[500 + y][i] = 1;
            for(int j = 0; j < 1000 - 500 + y; j++)
            {
                terrain[500+y+j][i] = 1;
                data[4000 * 500 + j + i * 4] = 255;
                data[4000 * 500 + j + i * 4 + 1] = 255;
                data[4000 * 500 + j + i * 4 + 2] = 255;
                data[4000 * 500 + j + i * 4 + 3] = 255;

            }
        }
        asTexture.update(data.data());
    }
    void placeBlock(vec2 pos, Block b, GameManager& gm)
    {
        pos = vec2(floor(pos.x / (float)BLOCK_SIZE), floor(pos.y / (float)BLOCK_SIZE));
        terrain[pos.y][pos.x] = 1;
        data[4000 * pos.y + pos.x * 4] = b.mapColor.x;
        data[4000 * pos.y + pos.x * 4 + 1] = b.mapColor.y;
        data[4000 * pos.y + pos.x * 4 + 2] = b.mapColor.z;
        data[4000 * pos.y + pos.x * 4 + 3] = 255;
        asTexture.update(data.data());
        if(b.lit)
        {
            gm.lights[gm.lightCount] = pos * (float)BLOCK_SIZE + b.lightPos * (float)BLOCK_SIZE + vec2(BLOCK_SIZE / 2.0f, BLOCK_SIZE / 2.0f );
            gm.lightColors[gm.lightCount] = b.lightColor;
            gm.lightCount++;
        }
        //Baking light with new block
        lightBakeShader.setUniformArray("lightPos", gm.lights, 100);
        lightBakeShader.setUniformArray("lightColors", gm.lightColors, 100);
        lightBakeShader.setUniform("lightCount", gm.lightCount);
        gm.updateLightCache(pos * (float)BLOCK_SIZE);
    }

    void removeBlock(vec2 pos, GameManager& gm)
    {
        pos = vec2(floor(pos.x / (float)BLOCK_SIZE), floor(pos.y / (float)BLOCK_SIZE));
        terrain[pos.y][pos.x] = 1;
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
    drawLightShader.setUniform("CHUNK_SIZE", (float)gm.CHUNK_SIZE);

    lightBakeShader.setUniform("BLOCK_SIZE", (float)BLOCK_SIZE);
    lightBakeShader.setUniform("WORLD_SIZE", (float)1000.0f);
    renderShader.setUniform("WORLD_SIZE", (float)1000.0f);
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
    //setWarningMessage("max torch count\nachieved");
    import(registry);

}

int main()
{
    static int f = 0;
    start();
    while(window.isOpen())
    {

        vec2 mousepos = window.mapPixelToCoords(Mouse::getPosition(window));
        float dt = deltaClock.restart().asSeconds();

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
                testPlanet.placeBlock(mousepos, registry.blocks[currentItem], gm);
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
