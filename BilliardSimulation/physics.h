#ifndef phYsics_h
#define phYsics_h

#include <GL/glew.h>
#include <glm/glm.hpp>

class setShotSpeed
{
public:
	int starting_mouse_x;  
	int starting_mouse_y;
	int follow_mouse_x;
	int follow_mouse_y;
	int tracking;
	int ballID;
	glm::vec3 speed;
	glm::vec3 speedAngle;

	void setSpeed();
};

class spherePhysics
{
public:
	glm::vec3 spherePosition[16];
	glm::mat4 translation[16];
	glm::mat4 rotation[16];
	glm::vec3 sphereSpeed[16];
	glm::vec3 sphereSpeedAngle[16];
	glm::vec3 pocket[6];


	float deltatime = 0.001f; // seconds
	float acceleration = 9.8f; 
	float friction = 0.4f;
	float speedVar = -friction * acceleration*deltatime;
	float angleVarOnSpeed = 2.5f*speedVar;
	float wall;

	void startingSpherePosition();
	void startingSphereSpeed();
	void sphereHitsWall(int i);
	void sphereHitsSphere(int i, int j);
	void createThePockets();
	void activatePockets(int i);

};
#endif