#ifndef graphiCs_h
#define graphiCs_h

#include <GL/glew.h>
#include <glm/glm.hpp>

class ball {
private:
	int slices;
	int stacks;
public:
	glm::vec3 *vertex;
	int vertexNum;
	glm::vec3 *normal;
	glm::vec2 *texcoord;

	ball(int slc, int stk);
	void ballMesh();
};

class table {
public:
	float length;
	float width;
	float height;
	int vertexNum;
	glm::vec3 *vertex;
	glm::vec3 *normal;
	glm::vec3 *points;
	int id;
	glm::vec2 *texcoord;
	glm::vec2 tex[4];
	table(float l, float w, float h);
	void quad(int a, int b, int c, int d);
	void tableMesh();

};

class light {
private:
	glm::vec3 position;
public:
	light(glm::vec3 pos);
};

class quaternion {
private:
	int screen_width;
	int screen_height;
public:
	int last_mx;
	int last_my;
	int cur_mx;
	int cur_my;
	int trackball_on;

	quaternion(int w, int h);
	glm::vec3 get_trackball_vec(int x, int y);
	glm::vec4 quaMul(glm::vec4 a, glm::vec4 b);
	glm::mat4 quaRot();
};

void initBallPos(glm::vec3 *pos, float diameter);

GLuint setVBO(glm::vec3 *points, glm::vec3 *normal, glm::vec2 *texcoord, int vertexNum);
GLuint setIBO(GLushort *element, int vertexNum);

#endif
