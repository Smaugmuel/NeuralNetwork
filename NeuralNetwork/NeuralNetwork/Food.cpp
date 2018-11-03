#include "Food.hpp"
#include "Settings.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

Food::Food()
{
}

Food::~Food()
{
}

void Food::Initialize()
{
	m_circle.setRadius(SNAKE_RADIUS);
	m_circle.setFillColor(sf::Color::Red);
	m_circle.setOrigin(FOOD_RADIUS, FOOD_RADIUS);
	m_circle.setPosition(sf::Vector2f(400.0f, 400.0f));
}

void Food::Respawn()
{
	sf::Vector2f newPos;
	newPos.x = (static_cast<float>(rand()) / RAND_MAX) * (WNDW - FOOD_RADIUS * 2) + FOOD_RADIUS;
	newPos.y = (static_cast<float>(rand()) / RAND_MAX) * (WNDH - FOOD_RADIUS * 2) + FOOD_RADIUS;
	m_circle.setPosition(newPos);
}

const sf::CircleShape & Food::Get() const
{
	return m_circle;
}

void Food::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	target.draw(m_circle, states);
}
