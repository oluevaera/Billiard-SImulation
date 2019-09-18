#define _USE_MATH_DEFINES
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <math.h>
#include "graphics.h"
#include "physics.h"
#include <iostream>

/// Creating the ball class in order to create the spheres and their IDs
ball::ball(int slc, int stk) 
{
	slices = slc;
	stacks = stk;
}

///Here we will give shape to our spheres
void ball::ballMesh() 
{
	vertexNum = 2 * slices * stacks;  //many vertices == smoother model
	vertex = (glm::vec3*)malloc(sizeof(glm::vec3)*vertexNum);
	normal = (glm::vec3*)malloc(sizeof(glm::vec3)*vertexNum);
	texcoord = (glm::vec2*)malloc(sizeof(glm::vec2)*vertexNum);
	double stepsk = M_PI / stacks;
	double stepsl = 2 * M_PI / slices;
	double alpha;  //angle alpha
	double beta;   //angle beta
	 
	for (int i = 0; i < stacks; i++)
	{
		alpha = stepsk * i;
		for (int j = 0; j < slices; j++) 
		{
			beta = stepsl * j;
			glm::vec3 v1 = glm::vec3(sin(alpha) * sin(beta), cos(alpha), sin(alpha) * cos(beta));
			glm::vec3 v2 = glm::vec3(sin(alpha + stepsk) * sin(beta), cos(alpha + stepsk), sin(alpha + stepsk) * cos(beta));
			*(vertex++) = v1;
			*(vertex++) = v2;
			*(normal++) = v1;
			*(normal++) = v2;
			*(texcoord++) = glm::vec2(-(atan2(v1[0], v1[1]) / M_PI + 1.0) / 2, -acos(v1[2]) / M_PI);
			*(texcoord++) = glm::vec2(-(atan2(v2[0], v2[1]) / M_PI + 1.0) / 2, -acos(v2[2]) / M_PI);
		}
	}
	vertex = vertex - vertexNum;
	normal = normal - vertexNum;
	texcoord = texcoord - vertexNum;
}

/// Class for the table
table::table(float l, float w, float h)
{
	length = l;
	width = w;
	height = h;
}
/// Giving the table its vertices
void table::quad(int a, int b, int c, int d) 
{
	glm::vec3 u = points[b] - points[a];
	glm::vec3 v = points[c] - points[b];
	glm::vec3 n = glm::normalize(glm::cross(u, v));

	*(normal + id) = n; *(vertex + id) = points[a]; *(texcoord + id) = tex[0]; id++;
	*(normal + id) = n; *(vertex + id) = points[b]; *(texcoord + id) = tex[1]; id++;
	*(normal + id) = n; *(vertex + id) = points[c]; *(texcoord + id) = tex[2]; id++;
	*(normal + id) = n; *(vertex + id) = points[a]; *(texcoord + id) = tex[0]; id++;
	*(normal + id) = n; *(vertex + id) = points[c]; *(texcoord + id) = tex[2]; id++;
	*(normal + id) = n; *(vertex + id) = points[d]; *(texcoord + id) = tex[3]; id++;
}
///Creating the table model
void table::tableMesh() 
{
	vertexNum = 6 * 5;
	id = 0;
	vertex = (glm::vec3*)malloc(sizeof(glm::vec3)*vertexNum);
	normal = (glm::vec3*)malloc(sizeof(glm::vec3)*vertexNum);
	texcoord = (glm::vec2*)malloc(sizeof(glm::vec2)*vertexNum);

	points = (glm::vec3*)malloc(sizeof(glm::vec3) * 8);
	*(points++) = glm::vec3(-1.0, -1.0, 1.0);
	*(points++) = glm::vec3(1.0, -1.0, 1.0);
	*(points++) = glm::vec3(1.0, 1.0, 1.0);
	*(points++) = glm::vec3(-1.0, 1.0, 1.0);
	*(points++) = glm::vec3(1.0, -1.0, -1.0);
	*(points++) = glm::vec3(-1.0, -1.0, -1.0);
	*(points++) = glm::vec3(-1.0, 1.0, -1.0);
	*(points++) = glm::vec3(1.0, 1.0, -1.0);
	points = points - 8;

	std::cout << "hi, i am ok" << std::endl;
	tex[0] = glm::vec2(0.0, 0.0);
	tex[1] = glm::vec2(1.0, 0.0);
	tex[2] = glm::vec2(1.0, 1.0);
	tex[3] = glm::vec2(0.0, 1.0);
	std::cout << "hi, i am ok" << std::endl;

	quad(1, 0, 3, 2);
	quad(0, 5, 6, 3);
	quad(5, 4, 7, 6);
	quad(4, 1, 2, 7);
	quad(0, 1, 4, 5);
}
/// This part is taken from moodle 212CR week7 commented code.
GLuint setVBO(glm::vec3 *points, glm::vec3 *normals, glm::vec2 *texcoord, int vertexNum) {
	GLuint vbo;
	int sizePoints = sizeof(glm::vec3)*vertexNum;
	int sizeTexcoord = sizeof(glm::vec2)*vertexNum;
	glGenBuffers(1, &vbo);   //Generating the buffer ID
	glBindBuffer(GL_ARRAY_BUFFER, vbo); //biinding the vertex buffer
	glBufferData(GL_ARRAY_BUFFER, sizePoints * 2 + sizeTexcoord, NULL, GL_STATIC_DRAW); //reserving the space 
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizePoints, points);  //Split the buffer in 3, the 1/3 is for the vertices
	glBufferSubData(GL_ARRAY_BUFFER, sizePoints, sizePoints, normals);  //the 2/3 is for the normals
	glBufferSubData(GL_ARRAY_BUFFER, 2 * sizePoints, sizeTexcoord, texcoord);  // and the last part is for texcoord
	return vbo;
}

quaternion::quaternion(int w, int h)   //creating quaternion class
{
	screen_width = w;
	screen_height = h;
}

glm::vec3 quaternion::get_trackball_vec(int x, int y)  
{
	glm::vec3 pvec = glm::vec3(1.0*x / screen_width * 2 - 1.0, 1.0*y / screen_height * 2 - 1.0, 0);
	pvec.y = -pvec.y;
	float plength = pvec.x*pvec.x + pvec.y*pvec.y;
	if (plength <= 1.0)
	{
		pvec.z = sqrt(1 - plength);
	}
	else pvec = glm::normalize(pvec);
	return pvec;
}

glm::vec4 quaternion::quaMul(glm::vec4 a, glm::vec4 b)     //vector functionality for quaternion multiplication.
{
	float angA = a.x;
	float angB = b.x;
	glm::vec3 axisA = glm::vec3(a.y, a.z, a.w);
	glm::vec3 axisB = glm::vec3(b.y, b.z, b.w);
	return glm::vec4(angA*angB - glm::dot(axisA, axisB), glm::vec3(angA*axisB + angB * axisA + glm::cross(axisA, axisB)));
}

glm::mat4 quaternion::quaRot()     //setting up the rotation angle of the spheres.
{
	float rotAng = 0;
	glm::vec3 rotAxis = glm::vec3(1.0f);

	if (cur_mx != last_mx || last_my != cur_my)  //checking to see if there was any mouse movement.
	{
		glm::vec3 va = get_trackball_vec(last_mx, last_my);  //my origninal mouse position
		glm::vec3 vb = get_trackball_vec(cur_mx, cur_my);    //my final mouse position
		rotAng = acos(glm::min(1.0f, glm::dot(va, vb)));   //getting the dot product of my mouse movement
		rotAxis = glm::normalize(glm::cross(va, vb));  //normalizing the vectors to get our rezult between my mouse movement, using the vector created whiel I was pressing the left mouse button
		last_mx = cur_mx;    //resetting
		last_my = cur_my;    //resetting
	}
	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), rotAng, rotAxis);
	return rot;  //retuntring the rotation
}





