#define _USE_MATH_DEFINES
#include <stdio.h>
#include "physics.h"
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


void setShotSpeed::setSpeed()    //setting the speed of the balls
{
	if (ballID >= 0)   
	{
		speed[0] = follow_mouse_y - starting_mouse_y;
		speed[1] = 0.0;
		speed[2] = -(follow_mouse_x - starting_mouse_x);
		speedAngle = glm::cross(glm::vec3(0.0, 1.0, 0.0), speed);
	}
	else  
	{
		speed = glm::vec3(0.0f);
		speedAngle = glm::vec3(0.0f);
	}
}

///Starting positions for the spheres.
void spherePhysics::startingSpherePosition()
{
	float d = 2.0;
	spherePosition[0] = glm::vec3(0.0, 0.0, 16.0);     //set to starting position
	translation[0] = glm::translate(glm::mat4(1.0f), spherePosition[0]);  //set to 0 translation
	rotation[0] = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0, 0.0, 0.0)); // set to 0 rotation

	int k = 1;
	for (int i = 0; i<5; i++)
	{
		for (int j = 0; j<5 - i; j++)  //coordinate position of the spheres
		{
			spherePosition[k][0] = j * d*cos(M_PI / 3) - 2 * d + i * d; //x > vertical axis
			spherePosition[k][1] = 0.0;
			spherePosition[k][2] = j * d*sin(M_PI / 3) - 16.0; //z > horizontal axis
			translation[k] = glm::translate(glm::mat4(1.0f), spherePosition[k]);
			rotation[k] = glm::rotate(glm::mat4(1.0f), 1.6f, glm::vec3(1.0, 0.0, 0.0)); //starting side of the sphere that faces the camera
			k++;
		}
	}
}

void spherePhysics::startingSphereSpeed()  //setting Starting speed and starting saved angle = 0
{
	for (int i = 0; i<16; i++)
	{
		sphereSpeed[i] = glm::vec3(0.0f);
		sphereSpeedAngle[i] = glm::vec3(0.0f);
	}
}

///Function on what to do if a sphere collides withe a wall.
void spherePhysics::sphereHitsWall(int i)
{
	sphereSpeed[i] *= glm::vec3(wall, 0.0, -wall); //mirroring the speed on collision
	sphereSpeedAngle[i] = 2.5f*glm::cross(glm::vec3(0.0, 1.0, 0.0), sphereSpeed[i]); //mirroring the angle
}

///Function on what to do if a sphere collides with another sphere.
///In here we can add  variables in order to make our collisins more realistic
void spherePhysics::sphereHitsSphere(int i, int j)
{
	glm::vec3 n = glm::normalize(spherePosition[j] - spherePosition[i]);
	glm::vec3 niVel = glm::dot(sphereSpeed[i], n)*n;	//speed tranfered from sphere1 on sphere2 during impact
	glm::vec3 tiVel = sphereSpeed[i] - niVel;  //personal speed left on sphere1 from collision
	glm::vec3 njVel = glm::dot(sphereSpeed[j], -n)*(-n); //spped transfered from spere2 on sphere1 during impact
	glm::vec3 tjVel = sphereSpeed[j] - njVel; // personal speed left on sphere 2 after colision
	sphereSpeed[i] = njVel + tiVel; //new combined speed for sphere1
	sphereSpeed[j] = niVel + tjVel; //new combined speed for sphere2
	sphereSpeedAngle[i] = 2.5f*glm::cross(glm::vec3(0.0, 1.0, 0.0), sphereSpeed[i]); //setting the angle for mirroring the movement
}

void spherePhysics::createThePockets()  //setting up pocket pocket
{
	float x = 24;
	float z = 24;
	int k = 0;   //pocket id
	for (int i = 0; i<2; i++)
	{
		for (int j = 0; j<3; j++)  // i-j are itterating through all the pocket
		{
			pocket[k][0] = i * z - z / 2;  //beggining from pocket 0 (k goes from pocket 0 to pocket 5) and giving  coordinates (xyz)  . depth coordinate is not needed, thus the 0.
			pocket[k][1] = 0.0;
			pocket[k][2] = j * x - x;
			k++;
		}
	}
}

void spherePhysics::activatePockets(int i)  //making the ball go in the pocket if its in a disntance close to the pockets.
{
	for (int j = 0; j<6; j++)  //checking all 6 pockets
	{
		float dist = glm::distance(spherePosition[i], pocket[j]);
		if (dist <= 1.9)
		{
			///Sending the Balls on or under the table based on their ID. 
			///Cue Ball comes on the top again since the ID/i == 0.
			spherePosition[i] = glm::vec3(0.0, -i * 5.0, 12.0);
		}
	}
}