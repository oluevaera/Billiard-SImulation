#ifndef variables_h
#define variables_h

#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "graphics.h"
#include "physics.h"
#include <string>

// Global variables
int screen_width = 1920, screen_height = 1080;
GLuint ball_vbo, table_vbo;
GLuint ball_vao, table_vao;
GLuint select_ball_vao;

GLuint program;
GLuint select_program;
GLint attribute_v_coord;
GLint attribute_v_normal;
GLint attribute_v_texcoord;
GLint uniform_mv, uniform_pro, uniform_m, uniform_v;
GLint uniform_lightPos, uniform_ambient, uniform_diffuse, uniform_specular, uniform_shine;
GLint uniform_ballID;
GLint uniform_select_pro, uniform_select_mv, uniform_select_m, uniform_select_v;
GLint attribute_v_select_coord;

GLuint mytexture_id[17];
GLuint uniform_mytexture;

#define ORTHO
glm::mat4 model = glm::mat4(1.0f);

#ifdef ORTHO
glm::mat4 view = glm::lookAt(glm::vec3(0.0, 3.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0));
glm::mat4 project = glm::ortho(30.0f, -30.0f, 15.0f, -15.0f, 0.1f, 6.0f);
#endif

glm::mat4 modelview = view * model;


struct filename
{
	const char* vshader_filename;
	const char* fshader_filename;
};
struct filename files = { "poolgame.v.glsl","poolgame.f.glsl" };
struct filename select_files = { "selection.v.glsl","selection.f.glsl" };

std::string texture_files[17];

ball myBall(30, 30);
table myTable(48.0, 24.0, 1.0);
spherePhysics mySpherePhysics;
glm::vec3 myInitBallPos[16];
glm::mat4 myInitTranslate[16];


#endif
