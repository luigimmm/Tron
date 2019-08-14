#pragma once
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <windows.h>
#include "Player.h"

#define TILE_Size 40.f

class Game
{
private:
	sf::RenderWindow* window;
	sf::TcpSocket socket;
	sf::Vector2f moveDir;
	Player player1;
	Player player2;
	sf::Font font;
	sf::Text text;

	bool isHost;
	bool readyLocal;
	bool readyRemote;
	bool gameOverLocal;
	bool gameOverRemote;
	bool right;
	bool left;
	bool up;
	bool down;

public:
	Game();
	~Game();

	void Run();
	void Initilialize();
	void NetworkStuff();
	void GamepLoop();
	void NewGame();
	void ReadyLoop();
	void Input();
	void CheckPackets();
	void SendPackets();
	void CheckCollision();
	bool DisplayAll();
};

