#include "Game.h"



Game::Game()
{
}


Game::~Game()
{
	delete window;
}

void Game::Run()
{
	Initilialize();
	GamepLoop();
}

void Game::Initilialize()
{
	//network
	NetworkStuff();

	//Window Settings
	window = new sf::RenderWindow(sf::VideoMode(720, 720), "NetWorking");
	window->setFramerateLimit(15u);

	sf::Vector2f shapeSize(TILE_Size / 4, TILE_Size / 4);
	player1 = Player(shapeSize , sf::Color::Red);
	player2 = Player(shapeSize , sf::Color::Blue);
}

void Game::NetworkStuff()
{
	std::cout << "Ingresa \"host\" para ser host o ingresa la ip del host" << std::endl;
	std::string consoleInput;
	std::cin >> consoleInput;
	if (consoleInput == "host")
	{
		isHost = true;
		sf::TcpListener listener;
		listener.listen(54000);
		std::cout << "Waiting for connecion" << std::endl;
		listener.accept(socket);
	}
	else
	{
		isHost = false;
		std::cout << "Trying to connect..." << std::endl;
		socket.connect(consoleInput, 54000, sf::seconds(10));
	}

	socket.setBlocking(false);
}

void Game::GamepLoop()
{
	while (true)
	{
		//resetear variables
		NewGame();

		//loop hasta que todos esten listos
		ReadyLoop();

		//Game
		while (window->isOpen())
		{
			player1.PlayerPath.push_back(player1.shape);
			
			//input local
			Input();

			//Check for new packets
			CheckPackets();

			//Move local
			player1.shape.move(moveDir.x * TILE_Size, moveDir.y * TILE_Size);

			//checar colisiones
			CheckCollision();

			//enviar paquetes
			SendPackets();

			if (!DisplayAll())
			{
				break;
			}
		}
	}
}

void Game::NewGame()
{
	//clear paths
	player1.PlayerPath.clear();
	player2.PlayerPath.clear();

	//Texto
	font.loadFromFile("TronBoldInline.ttf");
	text.setFont(font);
	text.setString("    Ready?\n[press space]");
	text.setCharacterSize(80);
	text.setFillColor(sf::Color::Cyan);
	text.setPosition(200, 200);

	readyLocal = false;
	readyRemote = false;
	gameOverLocal = false;
	gameOverRemote = false;

	right = false;
	left = false;
	up = false;
	down = false;

	if (!isHost)
	{
		up = true;
		player1.shape.setPosition(350, 710);
		player2.shape.setPosition(350, 0);
		moveDir = sf::Vector2f(0, -0.25f);
	}
	else
	{
		down = true;
		player1.shape.setPosition(350, 0);
		player2.shape.setPosition(350, 710);
		moveDir = sf::Vector2f(0, 0.25f);
	}
}

void Game::ReadyLoop()
{
	while ((!readyLocal || !readyRemote) && window->isOpen())
	{
		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
			{
				readyLocal = true;

				sf::Packet packet;
				packet << "ready";
				socket.send(packet);
			}
			if (event.type == sf::Event::Closed)
			{
				window->close();
			}
		}

		sf::Packet packet;
		if (socket.receive(packet) == sf::Socket::Done)
		{
			std::string sReady;
			packet >> sReady;
			if (sReady == "ready")
			{
				readyRemote = true;
			}
		}

		window->clear();
		window->draw(text);
		window->display();
	}
}

void Game::Input()
{
	sf::Event event;

	while (window->pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			if (event.key.code == sf::Keyboard::W && !down)
			{
				up = true;
				down = false;
				left = false;
				right = false;

				moveDir = sf::Vector2f(0, -0.25f);
			}
			else if (event.key.code == sf::Keyboard::A && !right)
			{
				left = true;
				right = false;
				down = false;
				up = false;

				moveDir = sf::Vector2f(-0.25f, 0);
			}
			else if (event.key.code == sf::Keyboard::S && !up)
			{
				down = true;
				up = false;
				left = false;
				right = false;

				moveDir = sf::Vector2f(0, 0.25f);
			}
			else if (event.key.code == sf::Keyboard::D && !left)
			{
				right = true;
				left = false;
				down = false;
				up = false;

				moveDir = sf::Vector2f(0.25f, 0);
			}
			break;
		case sf::Event::Closed:
			window->close();
		default:
			break;
		}
	}
}

void Game::CheckPackets()
{
	sf::Packet packet;
	if (socket.receive(packet) == sf::Socket::Done)
	{
		sf::Vector2f pos;
		packet >> pos.x >> pos.y;
		player2.shape.setPosition(pos);
		player2.PlayerPath.push_back(player2.shape);
	}

	// Over Packet
	sf::Packet packetOver;
	if (socket.receive(packetOver) == sf::Socket::Done)
	{
		std::string sOver;
		packetOver >> sOver;
		if (sOver == "over")
		{
			gameOverLocal = true;
		}
	}
}

void Game::SendPackets()
{
	sf::Packet packet;
	packet << player1.shape.getPosition().x << player1.shape.getPosition().y;
	socket.send(packet);

	//avisar que ya acabo el juego
	if (gameOverLocal)
	{
		sf::Packet packetOverSend;
		packetOverSend << "over";
		socket.send(packetOverSend);
	}
}

void Game::CheckCollision()
{
	if (player1.shape.getPosition().x < 0 || player1.shape.getPosition().x > 720 || player1.shape.getPosition().y > 720 || player1.shape.getPosition().y < 0)
	{
		std::cout << "Colision" << std::endl;
		gameOverLocal = true;
	}

	for (int i = 0; i < player2.PlayerPath.size(); i++)
	{
		if (i != player2.PlayerPath.size() - 1)
		{
			if (player1.shape.getPosition().x == player2.PlayerPath[i].getPosition().x && player1.shape.getPosition().y == player2.PlayerPath[i].getPosition().y)
			{
				std::cout << "Colision" << std::endl;
				gameOverLocal = true;
			}
		}
	}
	for (int i = 0; i < player1.PlayerPath.size(); i++)
	{
		if (i != player1.PlayerPath.size() - 1)
		{
			if (player1.shape.getPosition().x == player1.PlayerPath[i].getPosition().x && player1.shape.getPosition().y == player1.PlayerPath[i].getPosition().y)
			{
				std::cout << "Colision" << std::endl;
				gameOverLocal = true;
			}
		}
	}
}

bool Game::DisplayAll()
{
	window->clear();
	window->draw(player1.shape);
	window->draw(player2.shape);
	for (int i = 0; i < player1.PlayerPath.size(); i++)
	{
		window->draw(player1.PlayerPath[i]);
	}
	for (int i = 0; i < player2.PlayerPath.size(); i++)
	{
		window->draw(player2.PlayerPath[i]);
	}
	if (gameOverLocal)
	{
		text.setString("Game Over");
		window->draw(text);
		window->display();
		Sleep(3000);
		return false;
	}
	window->display();
	return true;
}
