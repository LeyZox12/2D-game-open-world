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
View camera(FloatRect({0, 0}, {512.f, 512.f}));
vector<Keyboard::Key> heldKeys;


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
    void placeBlock(vec2 pos)
    {
        pos = vec2(floor(pos.x / (float)BLOCK_SIZE), floor(pos.y / (float)BLOCK_SIZE));
        terrain[pos.y][pos.x] = 1;
        data[4000 * pos.y + pos.x * 4] = 255;
        data[4000 * pos.y + pos.x * 4 + 1] = 255;
        data[4000 * pos.y + pos.x * 4 + 2] = 255;
        data[4000 * pos.y + pos.x * 4 + 3] = 255;
        asTexture.update(data.data());
    }

    void removeBlock(vec2 pos)
    {
        pos = vec2(floor(pos.x / (float)BLOCK_SIZE), floor(pos.y / (float)BLOCK_SIZE));
        terrain[pos.y][pos.x] = 1;
        data[4000 * pos.y + pos.x * 4] = 0;
        data[4000 * pos.y + pos.x * 4 + 1] = 0;
        data[4000 * pos.y + pos.x * 4 + 2] = 0;
        data[4000 * pos.y + pos.x * 4 + 3] = 0;
        asTexture.update(data.data());
    }

    Texture &getAsTex()
    {
        asTexture.update(data.data());
        return asTexture;
    }
};



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
    //vec2[100] lights;
    int lightCount;
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

Player player = Player();
GameManager gm;
Planet testPlanet;
Texture spriteSheet;

void inputManager(optional<Event> e);
void applyInputs();

void start()
{
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(60);
    player.init();
    player.setWorldPos(vec2(500, 450));
    renderShader.loadFromFile("res/Shaders/OnPlanetRender.frag", Shader::Type::Fragment);
    renderShader.setUniform("resolution", vec2(512, 512));
    testPlanet.init();
    testPlanet.generateWorld(0);
    renderShader.setUniform("BLOCK_SIZE", (float)BLOCK_SIZE);
    renderShader.setUniform("planet", testPlanet.getAsTex());
    spriteSheet.loadFromFile("res/spriteSheet.png");
    renderShader.setUniform("spriteSheet", spriteSheet);
    renderShader.setUniform("spriteSheetRes", vec2(spriteSheet.getSize().x, spriteSheet.getSize().y));
    renderShader.setUniform("spriteSheetUnit", vec2(16, 16));
    renderShader.setUniform("WORLD_SIZE", (float)1000);
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
            window.setTitle("Alone... FPS:" + to_string(int(1.0f / dt)));
        while(optional<Event> e = window.pollEvent())
        {
            if(e->is<Event::Closed>()) window.close();
            if(e->is<Event::Resized>())
            {
                Vector2u size = e->getIf<Event::Resized>() -> size;
                camera.setSize({size.x,size.y});
                bg.setSize({size.x, size.y});
                renderShader.setUniform("resolution", vec2(size.x, size.y));
            }
            if(e->is<Event::MouseButtonPressed>() && e->getIf<Event::MouseButtonPressed>() -> button == Mouse::Button::Left)
                testPlanet.placeBlock(mousepos);
            if(e->is<Event::MouseButtonPressed>() && e->getIf<Event::MouseButtonPressed>() -> button == Mouse::Button::Right)
                testPlanet.removeBlock(mousepos);
            inputManager(e);
        }
        applyInputs();
        camera.setCenter(player.getHead().getPosition());
        window.setView(camera);
        player.update(dt, mousepos);
        gm.applyCollision(player, testPlanet.terrain);
        bg.setPosition(window.mapPixelToCoords(vec2i(0,0)));
        renderShader.setUniform("topLeftPos", bg.getPosition());
        vec2 lights[100];
        lights[0] = vec2(50005.3, 49820.5);
        renderShader.setUniformArray("lightPos", lights, 100);
        renderShader.setUniform("lightCount", 1);
        renderShader.setUniform("mousepos", mousepos);
        renderShader.setUniform("playerPos", player.getBody().getPosition());
        renderShader.setUniform("headPos", player.getHead().getPosition());
        renderShader.setUniform("facePos", player.getFace().getPosition());
        window.clear();
        window.draw(bg, &renderShader);
        window.display();
    }
    return 0;
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
        if(count(heldKeys.begin(), heldKeys.end(), key) == 1)
            heldKeys.erase(find(heldKeys.begin(), heldKeys.end(), key));
    }
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
