#ifndef FOOD_HPP
#define FOOD_HPP
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/CircleShape.hpp>

class Food : public sf::Drawable
{
public:
	Food();
	~Food();

	void Initialize();

	void Respawn();
	const sf::CircleShape& Get() const;

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	sf::CircleShape m_circle;
};

#endif