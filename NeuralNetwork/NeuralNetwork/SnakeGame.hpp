#ifndef SNAKE_GAME_HPP
#define SNAKE_GAME_HPP
#include "Food.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include "Settings.hpp"
#include <vector>

class SnakeGame : public sf::Drawable
{
public:
	SnakeGame();
	~SnakeGame();

	void Initialize(int nSensors, sf::Vector2f startPos = sf::Vector2f(WNDW / 5, WNDH / 2), sf::Vector2f startDir = sf::Vector2f(1, 0));
	void ReInitialize();

	void Update(float dt);
	void UpdateMovement(float dt);
	void CheckWallAndBodyCollisions();
	void CheckFoodCollision();

	void SteerLeft(float dt);
	void SteerRight(float dt);

	void AddPart();

	void RetrieveSensorValues(float* sensorDistances, float* sensorTypes) const;

	int GetSize() const;
	bool IsAlive() const;

private:
	void RotateHeadAndSensors(float v);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	// Snake variables
	std::vector<sf::CircleShape> m_bodyParts;
	sf::Vector2f m_movement;
	float m_speed;
	bool m_isAlive;

	// Sensor variables
	sf::Vector2f* m_sensorDirections;
	float* m_sensorDistances;
	sf::CircleShape* m_sensorMarkers;
	int m_nSensors;

	Food m_food;
};

#endif