#ifndef SETTINGS_HPP
#define SETTINGS_HPP

/*
Window settings
*/
#define WNDW 1000
#define WNDH 800

/*
Gameplay settings - snake
*/
#define SNAKE_SPEED 200.0f
#define SNAKE_ROTATION_SPEED 3.0f
#define SNAKE_RADIUS 20.0f
#define SNAKE_RADIUS_SQUARED SNAKE_RADIUS * SNAKE_RADIUS

/*
Gameplay settings - food
*/
#define FOOD_RADIUS SNAKE_RADIUS

/*
Neural network input settings
*/
#define N_SENSORS 15
#define SENSOR_START_ANGLE -3.14159265f * 2 / 3		// -120 degrees
#define SENSOR_END_ANGLE 3.14159265f * 2 / 3		// 120 degrees

/*
Neural network training settings
*/
#define N_NETWORKS 500U

/*
Neural netwok mutate settings
*/
#define NETWORKS_TO_KEEP_PERCENT 0.25f
#define NETWORKS_TO_MUTATE_FROM_BEST_PERCENT 0.25f


#endif