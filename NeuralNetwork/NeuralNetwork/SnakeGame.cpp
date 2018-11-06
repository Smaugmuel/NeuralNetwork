#include "SnakeGame.hpp"
#include "Intersection.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

SnakeGame::SnakeGame()
{
}
SnakeGame::~SnakeGame()
{
	if (m_sensorDirections)
	{
		delete[] m_sensorDirections;
		m_sensorDirections = nullptr;
	}
	if (m_sensorDistances)
	{
		delete[] m_sensorDistances;
		m_sensorDistances = nullptr;
	}
	if (m_sensorMarkers)
	{
		delete[] m_sensorMarkers;
		m_sensorMarkers = nullptr;
	}
}

void SnakeGame::Initialize(int nSensors, sf::Vector2f startPos, sf::Vector2f startDir)
{
	m_startPos = startPos;
	m_startDir = startDir;

	// Initialize snake
	sf::CircleShape head(SNAKE_RADIUS);
	head.setOrigin(sf::Vector2f(SNAKE_RADIUS, SNAKE_RADIUS));
	head.setFillColor(sf::Color::Green);
	head.setOutlineColor(sf::Color::Black);
	head.setOutlineThickness(-1.0f);
	head.setPosition(startPos);
	m_bodyParts.push_back(head);

	m_isAlive = true;

	m_movement = startDir;
	m_speed = SNAKE_SPEED;

	// Initialize sensors
	m_nSensors = nSensors;
	m_sensorDirections = new sf::Vector2f[m_nSensors];
	m_sensorDistances = new float[m_nSensors];
	m_sensorMarkers = new sf::CircleShape[m_nSensors];

	float angleIncrement = (SENSOR_END_ANGLE - SENSOR_START_ANGLE) / (m_nSensors - 1);
	for (int i = 0; i < m_nSensors; i++)
	{
		float currentAngle = SENSOR_START_ANGLE + angleIncrement * i;
		float sinv = std::sinf(currentAngle);
		float cosv = std::cosf(currentAngle);

		sf::Vector2f direction(
			cosv * m_movement.x - sinv * m_movement.y,
			sinv * m_movement.x + cosv * m_movement.y
		);

		m_sensorDirections[i] = direction;

		float sensorRadius = 5.0f;
		m_sensorMarkers[i].setRadius(sensorRadius);
		m_sensorMarkers[i].setFillColor(sf::Color::Yellow);
		m_sensorMarkers[i].setOrigin(sensorRadius, sensorRadius);
	}

	m_food.Initialize();
	m_food.Respawn();
}

void SnakeGame::ReInitialize()
{
	// Reset body
	m_bodyParts.erase(m_bodyParts.begin() + 1, m_bodyParts.end());
	m_bodyParts[0].setPosition(m_startPos);
	m_movement = m_startDir;
	m_isAlive = true;

	// Reset sensors
	float angleIncrement = (SENSOR_END_ANGLE - SENSOR_START_ANGLE) / (m_nSensors - 1);
	for (int i = 0; i < m_nSensors; i++)
	{
		float currentAngle = SENSOR_START_ANGLE + angleIncrement * i;
		float sinv = std::sinf(currentAngle);
		float cosv = std::cosf(currentAngle);

		sf::Vector2f direction(
			cosv * m_movement.x - sinv * m_movement.y,
			sinv * m_movement.x + cosv * m_movement.y
		);

		m_sensorDirections[i] = direction;

		float sensorRadius = 5.0f;
		m_sensorMarkers[i].setRadius(sensorRadius);
		m_sensorMarkers[i].setFillColor(sf::Color::Yellow);
		m_sensorMarkers[i].setOrigin(sensorRadius, sensorRadius);
	}

	m_food.Initialize();
	m_food.Respawn();
}

void SnakeGame::Update(float dt)
{
	if (m_isAlive)
	{
		UpdateMovement(dt);
		CheckWallAndBodyCollisions();
		CheckFoodCollision();
	}
}
void SnakeGame::UpdateMovement(float dt)
{
	int nParts = m_bodyParts.size();

	m_bodyParts[0].move(m_movement * m_speed * dt);

	for (int i = 1; i < nParts; i++)
	{
		// From this to part in front
		sf::Vector2f toBody = m_bodyParts[i - 1].getPosition() - m_bodyParts[i].getPosition();

		// Normalize direction
		float length = std::sqrtf(toBody.x * toBody.x + toBody.y * toBody.y);
		float inverseLength = 1.0f / length;
		toBody.x *= inverseLength;
		toBody.y *= inverseLength;

		if (length > SNAKE_RADIUS * 2.0f)
		{
			m_bodyParts[i].move(toBody * m_speed * dt);
		}
	}
}
void SnakeGame::CheckWallAndBodyCollisions()
{
	int nParts = m_bodyParts.size();
	sf::Vector2f headPosition = m_bodyParts[0].getPosition();

	// Check body collision
	for (int i = 2; i < nParts; i++)
	{
		sf::Vector2f toHead = headPosition - m_bodyParts[i].getPosition();
		float squaredDistanceToHead = toHead.x * toHead.x + toHead.y * toHead.y;
		if (squaredDistanceToHead < (SNAKE_RADIUS * 2.0f) * (SNAKE_RADIUS * 2.0f))
		{
			m_isAlive = false;
			return;
		}
	}

	// Check wall collision
	if (headPosition.x < SNAKE_RADIUS || headPosition.y < SNAKE_RADIUS || headPosition.x > WNDW - SNAKE_RADIUS || headPosition.y > WNDH - SNAKE_RADIUS)
	{
		m_isAlive = false;
	}
}
void SnakeGame::CheckFoodCollision()
{
	sf::Vector2f toFood = m_food.Get().getPosition() - m_bodyParts[0].getPosition();
	float squaredDistanceToFood = toFood.x * toFood.x + toFood.y * toFood.y;
	if (squaredDistanceToFood < (SNAKE_RADIUS * 2.0f) * (SNAKE_RADIUS * 2.0f))
	{
		m_food.Respawn();
		AddPart();
	}
}

void SnakeGame::SteerLeft(float dt)
{
	RotateHeadAndSensors(-SNAKE_ROTATION_SPEED * dt);
}
void SnakeGame::SteerRight(float dt)
{
	RotateHeadAndSensors(SNAKE_ROTATION_SPEED * dt);
}
void SnakeGame::RotateHeadAndSensors(float v)
{
	float sinv = std::sinf(v);
	float cosv = std::cosf(v);

	sf::Vector2f newVector(
		cosv * m_movement.x - sinv * m_movement.y,
		sinv * m_movement.x + cosv * m_movement.y
	);
	m_movement = newVector;

	for (int i = 0; i < m_nSensors; i++)
	{
		m_sensorDirections[i] = sf::Vector2f(
			cosv * m_sensorDirections[i].x - sinv * m_sensorDirections[i].y,
			sinv * m_sensorDirections[i].x + cosv * m_sensorDirections[i].y
		);
	}
}

void SnakeGame::AddPart()
{
	sf::CircleShape body(SNAKE_RADIUS);

	body.setOrigin(sf::Vector2f(SNAKE_RADIUS, SNAKE_RADIUS));
	body.setFillColor(sf::Color::Green);
	body.setOutlineColor(sf::Color::Black);
	body.setOutlineThickness(-1.0f);
	body.setPosition(m_bodyParts.back().getPosition());

	m_bodyParts.push_back(body);
}

void SnakeGame::RetrieveSensorValues(float* sensorDistances, float* sensorTypes) const
{
	int nParts = m_bodyParts.size();
	sf::Vector2f headPosition = m_bodyParts[0].getPosition();
	
	// Screen diagonal
	float squaredFarthestDistance = static_cast<float>(WNDW * WNDW + WNDH * WNDH);
	float farthestDistance = std::sqrtf(squaredFarthestDistance);

	IntersectionData intersection;

	for (int i_sensor = 0; i_sensor < m_nSensors; i_sensor++)
	{
		sf::Vector2f sensorDirection = m_sensorDirections[i_sensor];
		
		// Set default distance
		sensorDistances[i_sensor] = farthestDistance;

		// Check sensors against every body part apart from the head and "neck"
		for (int i_body = 2; i_body < nParts; i_body++)
		{
			intersection = RayVsCircle(headPosition, sensorDirection, m_bodyParts[i_body].getPosition(), SNAKE_RADIUS);

			if (intersection.isIntersecting)
			{
				// Store the closest distance
				sensorDistances[i_sensor] = std::min(sensorDistances[i_sensor], intersection.distance);
				sensorTypes[i_sensor] = 1.0f;
			}
		}

		// Check sensor against food
		intersection = RayVsCircle(headPosition, sensorDirection, m_food.Get().getPosition(), FOOD_RADIUS);
		if (intersection.isIntersecting)
		{
			// Store the closest distance
			sensorDistances[i_sensor] = std::min(sensorDistances[i_sensor], intersection.distance);
			sensorTypes[i_sensor] = 0.0f;
		}


		if (sensorDistances[i_sensor] >= farthestDistance)
		{
			// Nothing was seen by this sensor, check walls
			
			float k = sensorDirection.y / sensorDirection.x;
			float k_inv = 1.0f / k;

			// Set 0.5 as wall identifier
			sensorTypes[i_sensor] = 0.5f;

			sf::Vector2f toWall;

			if (sensorDirection.x < 0)
			{
				if (sensorDirection.y < 0)
				{
					float xSide = headPosition.x + (0 - headPosition.y) * k_inv;
					float ySide = headPosition.y + (0 - headPosition.x) * k;

					if (xSide > 0)
					{
						// Intersection at the bottom wall (top if (0,0) is top left)
						toWall = sf::Vector2f(xSide, 0) - headPosition;
					}
					else
					{
						// Intersection at the left wall
						toWall = sf::Vector2f(0, ySide) - headPosition;
					}
				}
				else
				{
					float xSide = headPosition.x + (WNDH - headPosition.y) * k_inv;
					float ySide = headPosition.y + (0 - headPosition.x) * k;

					if (xSide > 0)
					{
						// Intersection at the top wall (bottom if (0,0) is top left)
						toWall = sf::Vector2f(xSide, WNDH) - headPosition;
					}
					else
					{
						// Intersection at the left wall
						toWall = sf::Vector2f(0, ySide) - headPosition;
					}
				}
			}
			else
			{
				if (sensorDirection.y < 0)
				{
					float xSide = headPosition.x + (0 - headPosition.y) * k_inv;
					float ySide = headPosition.y + (WNDW - headPosition.x) * k;

					if (xSide < WNDW)
					{
						// Intersection at the bottom wall (top if (0,0) is top left)
						toWall = sf::Vector2f(xSide, 0) - headPosition;
					}
					else
					{
						// Intersection at the right wall
						toWall = sf::Vector2f(WNDW, ySide) - headPosition;
					}
				}
				else
				{
					float xSide = headPosition.x + (WNDH - headPosition.y) * k_inv;
					float ySide = headPosition.y + (WNDW - headPosition.x) * k;

					if (xSide < WNDW)
					{
						// Intersection at the top wall (bottom if (0,0) is top left)
						toWall = sf::Vector2f(xSide, WNDH) - headPosition;
					}
					else
					{
						// Intersection at the right wall
						toWall = sf::Vector2f(WNDW, ySide) - headPosition;
					}
				}
			}

			sensorDistances[i_sensor] = toWall.x * toWall.x + toWall.y * toWall.y;
			sensorDistances[i_sensor] = std::sqrtf(sensorDistances[i_sensor]);
		}

		// Set positions of where the intersection markers are
		m_sensorMarkers[i_sensor].setPosition(
			m_bodyParts[0].getPosition() +
			m_sensorDirections[i_sensor] * sensorDistances[i_sensor]
		);

		// Convert to [0, 1] range
		sensorDistances[i_sensor] /= farthestDistance;
	}
}

int SnakeGame::GetSize() const
{
	return m_bodyParts.size();
}

bool SnakeGame::IsAlive() const
{
	return m_isAlive;
}

void SnakeGame::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	if (!m_isAlive)
	{
		return;
	}

	int nParts = m_bodyParts.size();
	for (int i = 0; i < nParts; i++)
	{
		target.draw(m_bodyParts[i], states);
	}

	/*for (int i = 0; i < m_nSensors; i++)
	{
		target.draw(m_sensorMarkers[i], states);
	}*/

	target.draw(m_food, states);
}