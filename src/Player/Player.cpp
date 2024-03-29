#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include "Input/Input.hpp"
#include "Player/Player.hpp"
#include "Entity/EntityManager.hpp"

#define CANON_LENGTH 24.f
#define CANON_WIDTH 14.f
#define ROTATION_SPEED 5.f

tt::Player::Player(std::string id, sf::Vector2f position, tt::Player::Controls controls) :
    body(new sf::RectangleShape()),
    canon(new sf::RectangleShape({ CANON_LENGTH, CANON_WIDTH })),
    velocity({ 0, 0 }),
    direction(sf::Vector2f({ 1.f, 0.f })),
    controls(controls),
    Entity(id) {

    this->classList.add("player");

    this->body->setFillColor(sf::Color::White);
    this->body->setPosition(position);
    this->body->setSize({ this->size, this->size });

    this->canon->setFillColor(this->canonColor);
    this->canon->setOrigin(-2.f, CANON_WIDTH/2);
}

tt::Player::~Player() {
    delete this->body;
    delete this->canon;
}

void tt::Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(*this->body);
    target.draw(*this->canon);
}

void tt::Player::handleInput(float dt) {
    this->setVelocity({ 0, 0 });
    if (Input::isKeyPressed(this->controls.forward))     this->addVelocity(dt, 1);
    if (Input::isKeyPressed(this->controls.rotateLeft))  this->rotate(dt, -1);
    if (Input::isKeyPressed(this->controls.backward))    this->addVelocity(dt, -1);
    if (Input::isKeyPressed(this->controls.rotateRight)) this->rotate(dt, 1);
    if (Input::isKeyPressed(this->controls.shoot))       this->shoot();
}
void tt::Player::update(float dt) {
    this->body->move(this->velocity);
    this->canon->setPosition(this->getCenterPosition());
    this->canShoot = shootClock.getElapsedTime().asSeconds() >= SHOOT_DELAY;
}
bool tt::Player::shoot() {
    if (!this->canShoot) return false;

    sf::Vector2f center = this->getCenterPosition() - sf::Vector2f(BULLET_RADIUS, BULLET_RADIUS);
    sf::Vector2f muzzle = center + this->direction * CANON_LENGTH/3.f;
    tt::EntityManager::instantiate(new tt::Bullet(muzzle, this->direction, this->id));
    this->shootClock.restart();
    return true;
}
void tt::Player::addVelocity(float dt, int dir) {
    float offsetx = this->direction.x * PLAYER_VELOCITY * dt;
    float offsety = this->direction.y * PLAYER_VELOCITY * dt;
    this->velocity = { offsetx*dir, offsety*dir };
}
void tt::Player::rotate(float dt, int dir) {
    float rad_angle = this->getAngle() + (dir*dt*ROTATION_SPEED);
    float deg_angle = rad_angle * (180/M_PI);
    this->canon->setRotation(deg_angle);
    // Save the direction of the canon
    this->direction.x = std::cos(rad_angle);
    this->direction.y = std::sin(rad_angle);
}

// Setters 🐢
void tt::Player::setColor(sf::Color color) { this->body->setFillColor(color); }
void tt::Player::resetColor() { this->setColor(this->bodyColor); }
void tt::Player::setVelocity(sf::Vector2f vel) {
    this->velocity.x = vel.x;
    this->velocity.y = vel.y;
}
// Getters 🐢
float        tt::Player::getAngle() {return this->canon->getRotation() * (M_PI/180); }
sf::Vector2f tt::Player::getSize() { return this->body->getSize(); }
sf::Vector2f tt::Player::getVelocity() { return this->velocity; }
sf::Vector2f tt::Player::getPosition() { return this->body->getPosition(); }
sf::Vector2f tt::Player::getCenterPosition() {
    return this->body->getPosition() + this->getSize()/2.f;
}
