#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Window/Event.hpp>
#include "NeuralNetwork.hpp"

int main()
{
	NeuralNetwork nn;

	int nNeuronsPerLayer[] = { 2, 2, 2 };
	int nLayers = sizeof(nNeuronsPerLayer) / sizeof(int);
	nn.CreateNetwork(nLayers, nNeuronsPerLayer);
	//nn.UpdateValues();









	sf::RenderWindow window(sf::VideoMode(200, 200), "Neural Network");
	sf::CircleShape circle(100.0f);

	circle.setFillColor(sf::Color::Red);

	while (window.isOpen())
	{
		sf::Event e;
		while (window.pollEvent(e))
		{
			if (e.type == sf::Event::EventType::Closed)
			{
				window.close();
			}
		}

		window.clear();
		window.draw(circle);
		window.display();
	}

	return 0;
}