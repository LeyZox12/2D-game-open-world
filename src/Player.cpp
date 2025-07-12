#include "Player.h"

Player::Player()
{

}
void Player::init()
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
void Player::setWorldPos(vec2 pos)
{
    this -> pos = pos * (float)BLOCK_SIZE;
}
void Player::setPosition(vec2 pos)
{
    this -> pos = pos;
}
vec2 Player::getPosition()
{
    return pos;
}
void Player::move(vec2 offset)
{
    this -> pos += offset;
}
void Player::setVel(vec2 vel)
{
    this -> vel = vel;
}
void Player::addVel(vec2 vel)
{
    this -> vel += vel;
}
vec2 Player::getVel()
{
    return vel;
}
RectangleShape Player::getHead()
{
    return head;
}

RectangleShape Player::getFace()
{
    return face;
}

RectangleShape Player::getBody()
{
    return body;
}

vec2 Player::getWorldPos()
{
    return vec2(floor(pos.x / (float)BLOCK_SIZE), floor(pos.y / (float)BLOCK_SIZE));
}
void Player::update(float dt, vec2 mousepos)
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
void Player::display(RenderWindow& window)
{
    window.draw(body);
    window.draw(head);
    window.draw(face);
}

