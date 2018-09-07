#include "cvm.h"
#include "chip8.h"

#include <cstdlib>
#include <string>
#include <SFML/Graphics.hpp>

using namespace std;

cvm::cvm()
{
}


cvm::~cvm()
{
}


int cvm::run()
{
	sf::RenderWindow window(sf::VideoMode(640, 320), "CHIP-8");
	sf::RectangleShape pixel[64 * 32];

	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 32; j++) {
			pixel[j * 64 + i].setSize(sf::Vector2f(10, 10));
			pixel[j * 64 + i].setPosition(sf::Vector2f(i * 10, j * 10));
		}
	}
	

	chip8 c;
	c.initial();
	
	c.loadRomFile(string("./PONG"));

	window.setFramerateLimit(60);
	while (window.isOpen()) {
		c.step();

		sf::Event event;
		if (window.pollEvent(event)){
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;

			case sf::Event::KeyPressed: {
				switch (event.key.code)
				{
				case sf::Keyboard::Num0:
					c.pressKeypad(0);
					break;
				case sf::Keyboard::Num1:
					c.pressKeypad(1);
					break;
				case sf::Keyboard::Num2:
					c.pressKeypad(2);
					break;
				case sf::Keyboard::Num3:
					c.pressKeypad(3);
					break;
				case sf::Keyboard::Num4:
					c.pressKeypad(4);
					break;
				case sf::Keyboard::Num5:
					c.pressKeypad(5);
					break;
				case sf::Keyboard::Num6:
					c.pressKeypad(6);
					break;
				case sf::Keyboard::Num7:
					c.pressKeypad(7);
					break;
				case sf::Keyboard::Num8:
					c.pressKeypad(8);
					break;
				case sf::Keyboard::Num9:
					c.pressKeypad(9);
					break;
				case sf::Keyboard::A:
					c.pressKeypad(0xA);
					break;
				case sf::Keyboard::B:
					c.pressKeypad(0xB);
					break;
				case sf::Keyboard::C:
					c.pressKeypad(0xC);
					break;
				case sf::Keyboard::D:
					c.pressKeypad(0xD);
					break;
				case sf::Keyboard::E:
					c.pressKeypad(0xE);
					break;
				case sf::Keyboard::F:
					c.pressKeypad(0xF);
					break;
				}
				break;
			}
				
			case sf::Event::KeyReleased: {
				switch (event.key.code)
				{
				case sf::Keyboard::Num0:
					c.releaseKeypad(0);
					break;
				case sf::Keyboard::Num1:
					c.releaseKeypad(1);
					break;
				case sf::Keyboard::Num2:
					c.releaseKeypad(2);
					break;
				case sf::Keyboard::Num3:
					c.releaseKeypad(3);
					break;
				case sf::Keyboard::Num4:
					c.releaseKeypad(4);
					break;
				case sf::Keyboard::Num5:
					c.releaseKeypad(5);
					break;
				case sf::Keyboard::Num6:
					c.releaseKeypad(6);
					break;
				case sf::Keyboard::Num7:
					c.releaseKeypad(7);
					break;
				case sf::Keyboard::Num8:
					c.releaseKeypad(8);
					break;
				case sf::Keyboard::Num9:
					c.releaseKeypad(9);
					break;
				case sf::Keyboard::A:
					c.releaseKeypad(0xA);
					break;
				case sf::Keyboard::B:
					c.releaseKeypad(0xB);
					break;
				case sf::Keyboard::C:
					c.releaseKeypad(0xC);
					break;
				case sf::Keyboard::D:
					c.releaseKeypad(0xD);
					break;
				case sf::Keyboard::E:
					c.releaseKeypad(0xE);
					break;
				case sf::Keyboard::F:
					c.releaseKeypad(0xF);
					break;
				}
				break;
			}

			}
		}
		if (c.isVideoUpdated()) {
			window.clear();
			for (int i = 0; i < 64; i++) {
				for (int j = 0; j < 32; j++) {
					if (c.videoBuff[j * 64 + i] == 1) {
						window.draw(pixel[j * 64 + i]);
					}
				}
			}
			window.display();
		}
		

		//system("pause");
	}

	return 0;
}