#pragma once
#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <windows.h>
#include "Player.h"

class Player
{
public:
	Player();
	Player(sf::Vector2f shapeSize, sf::Color color);
	~Player();

	sf::RectangleShape shape;
	std::vector<sf::RectangleShape> PlayerPath;
};

