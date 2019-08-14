#include "Player.h"



Player::Player()
{
}

Player::Player(sf::Vector2f shapeSize, sf::Color color)
{
	shape.setSize(shapeSize);
	shape.setFillColor(color);
}


Player::~Player()
{
}
