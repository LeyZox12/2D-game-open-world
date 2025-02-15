#include <iostream>
#include <SFML/Graphics.hpp>
#include <math.h>
#define pi 3.1415926
#define rad pi/180

using namespace std;
using namespace sf;

typedef Vector2f vec2;

class player
{
    public:
        player()
        {

        }
        player(vec2 pos)
        {
            this -> pos = pos;
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
        void updatePos()
        {
            this -> pos += vel;
        }
        void display(RenderWindow& window, vec2 mousepos, Texture spriteSheet)
        {

            body.setTexture(&spriteSheet);
            head.setTexture(&spriteSheet);
            face.setTexture(&spriteSheet);
            body.setTextureRect({82, 0, 16, 16});
            head.setTextureRect({82, 0, 16, 16});
            face.setTextureRect({98, 0, 6, 3});
            face.setSize(vec2(30, 15));
            head.setSize(vec2(60, 60));
            body.setSize(vec2(60, 120));
            body.setPosition(pos);
            head.move(vec2((body.getPosition().x - head.getPosition().x) / 20,
                           (body.getPosition().y - head.getPosition().y - head.getSize().y * 1.2) / 20));
            face.setPosition(head.getPosition() + vec2(head.getSize().x / 2 - face.getSize().x / 2,
                                                       head.getSize().y / 2 - face.getSize().y / 2));
            vec2 diff = mousepos - face.getPosition();
            float dist = sqrt(diff.x * diff.x + diff.y * diff.y);
            float mult = dist * 0.01 > 8 ? 8 : dist * 0.01;
            face.move(vec2(diff.x / dist * mult , diff.y / dist * mult));
            window.draw(body);
            window.draw(head);
            window.draw(face);
        }
    private:
        vec2 pos;
        vec2 vel;
        vector<int> inventory;
        vector<string> voiceLines;
        RectangleShape body;
        RectangleShape head;
        RectangleShape face;
};


vector<vector<int>> generateWorld(int worldSize)
{
    vector<int> blocksY;

    for(int i = 0; i < worldSize; i++)
    {
        blocksY.push_back(worldSize / 2 + floor(sin(2 * i * rad) + sin(pi * i * rad) * 2));
        //blocksY.push_back(worldSize / 2);
    }
    cout << "done";
    vector<vector<int>> world;
    vector<int> row;
    for(int i = 0; i < worldSize; i++)
        row.push_back(0);
    for(int i = 0; i < worldSize; i++)
        world.push_back(row);
    int i = 0;
    for(auto& y : blocksY)
    {
        world[y][i] = 1;
        i++;
    }
    return world;
}

RenderWindow window(VideoMode::getDesktopMode(), "...", Style::Fullscreen);
Event e;
View camera(vec2(0, 0), vec2(1920, 1080));
vector<vector<int>> world;
Texture spriteSheet;
int worldSize = 100;
int blockSizeDisplay = 60;
player p(vec2(worldSize * blockSizeDisplay / 2, worldSize * blockSizeDisplay / 2));
vec2 mousepos;

void start()
{
    spriteSheet.loadFromFile("res/spriteSheet.png");
    world = generateWorld(worldSize);
}

void displayWorld()
{
    RectangleShape block;
    block.setTexture(&spriteSheet);
    block.setSize(vec2(blockSizeDisplay, blockSizeDisplay));
    int chunkPosX = floor(p.getPosition().x / blockSizeDisplay);
    int chunkPosY = floor(p.getPosition().y / blockSizeDisplay);

    for(int i = -2; i < 2; i++)
    {
        for(int y = 0; y < 50; y++)
        {
            for(int x = 0; x < blockSizeDisplay; x++)
            {
                if(world[chunkPosY + y][(chunkPosX + i * 20) + x] == 1)
                {
                    block.setTextureRect({112, 0, 16, 16});
                    block.setPosition((chunkPosX + i * 20) * blockSizeDisplay + x * blockSizeDisplay, (chunkPosY) * blockSizeDisplay + y * blockSizeDisplay);
                    window.draw(block);
                }
            }
        }
    }
}

int main()
{
    start();
    while(window.isOpen())
    {
        while(window.pollEvent(e))
        {
            if(e.type == Event::Closed)
                window.close();
        }
        mousepos = window.mapPixelToCoords(Mouse::getPosition(window));
        window.clear(Color::Black);

        camera.move((p.getPosition().x - camera.getCenter().x) / 10,
                    (p.getPosition().y - camera.getCenter().y) / 10 );
        //camera.setCenter(p.getPosition());
        window.setView(camera);
        displayWorld();
        p.display(window, mousepos, spriteSheet);
        window.display();
    }

    return 0;
}
