#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL2/SOIL2.h>
/* Use shader_utils.h for create and link shaders */
#include "shader_utils.h"
#include "graphics.h"
#include "physics.h"
#include "Variables.h"
#include <iostream>
#include <string>





/// Starting the program. and sending our verticees to the GPU to prccess
int init_resources()
{
	myBall.ballMesh(); 
	ball_vbo = setVBO(myBall.vertex, myBall.normal, myBall.texcoord, myBall.vertexNum); //send the sphere vertices to the GPU

	myTable.tableMesh();
	table_vbo = setVBO(myTable.vertex, myTable.normal, myTable.texcoord, myTable.vertexNum); //send the table vertices to the GPU

	mySpherePhysics.startingSpherePosition();  //starting sphere position
	mySpherePhysics.startingSphereSpeed(); //starting sphere speed
	mySpherePhysics.createThePockets(); //creating the pockets
	

	///Using the SOIL2 library in order to load up the images I use for shaders.
	///This decision came after struggling with manualy adding the textures to opengl and after researching how to load up
	///images and what other opengl developers prefere.
    ///
	///SOIL2 is easy to use,lighweight, and can use standard .jpg images which I mostly work with.
	///All the help that I needed in order to compile it,link it and use were found here >> https://bitbucket.org/SpartanJ/soil2 
	///Other sources are shown in my video report.
	
	
	texture_files[0] = "PlayerBall.jpg";
	for (int i = 1; i<16; i++)
	{
		texture_files[i] = "Ball" + std::to_string(i) + ".jpg";
	}
	texture_files[16] = "Billiard.jpg";


	for (int i = 0; i<17; i++)
	{
		mytexture_id[i] = SOIL_load_OGL_texture
		(
			texture_files[i].c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS
		);
	}

	// Create and link shader//
	GLint link_ok = GL_FALSE;
	GLuint vs, fs;
	if ((vs = create_shader(files.vshader_filename, GL_VERTEX_SHADER)) == 0) return 0;
	if ((fs = create_shader(files.fshader_filename, GL_FRAGMENT_SHADER)) == 0) return 0;

	//creating our program and linking its vertex and fragment shader
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);


	if ((vs = create_shader(select_files.vshader_filename, GL_VERTEX_SHADER)) == 0) return 0;
	if ((fs = create_shader(select_files.fshader_filename, GL_FRAGMENT_SHADER)) == 0) return 0;

	select_program = glCreateProgram();
	glAttachShader(select_program, vs);
	glAttachShader(select_program, fs);
	glLinkProgram(select_program);
	glGetProgramiv(select_program, GL_LINK_STATUS, &link_ok);


	const char* attribute_name;
	attribute_name = "v_coord";
	attribute_v_coord = glGetAttribLocation(program, attribute_name);

	attribute_name = "v_normal";
	attribute_v_normal = glGetAttribLocation(program, attribute_name);

	attribute_name = "v_texcoord";
	attribute_v_texcoord = glGetAttribLocation(program, attribute_name);

	const char* uniform_name;
	uniform_name = "mv";
	uniform_mv = glGetUniformLocation(program, uniform_name);

	uniform_name = "m";
	uniform_m = glGetUniformLocation(program, uniform_name);
	uniform_name = "v";
	uniform_v = glGetUniformLocation(program, uniform_name);

	uniform_name = "mytexture";
	uniform_mytexture = glGetUniformLocation(program, uniform_name);

	uniform_name = "pro";
	uniform_pro = glGetUniformLocation(program, uniform_name);

	uniform_name = "lightPos";
	uniform_lightPos = glGetUniformLocation(program, uniform_name);

	uniform_name = "ambient";
	uniform_ambient = glGetUniformLocation(program, uniform_name);

	uniform_name = "diffuse";
	uniform_diffuse = glGetUniformLocation(program, uniform_name);

	uniform_name = "specular";
	uniform_specular = glGetUniformLocation(program, uniform_name);

	uniform_name = "shine";
	uniform_shine = glGetUniformLocation(program, uniform_name);


	uniform_select_m = glGetUniformLocation(select_program, "select_m");
	uniform_select_v = glGetUniformLocation(select_program, "select_v");

	uniform_select_pro = glGetUniformLocation(select_program, "select_pro");
	uniform_ballID = glGetUniformLocation(select_program, "ballID");
	attribute_v_select_coord = glGetAttribLocation(select_program, "select_v_coord");

	return 1;
}

void initVAO()
{
	glGenVertexArrays(1, &ball_vao);
	glBindVertexArray(ball_vao);
	glBindBuffer(GL_ARRAY_BUFFER, ball_vbo);
	glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(attribute_v_normal);
	glVertexAttribPointer(attribute_v_normal, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(glm::vec3)*myBall.vertexNum));
	glEnableVertexAttribArray(attribute_v_texcoord);
	glVertexAttribPointer(attribute_v_texcoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(2 * sizeof(glm::vec3)*myBall.vertexNum));
	glBindVertexArray(0);

	glGenVertexArrays(1, &table_vao);
	glBindVertexArray(table_vao);
	glBindBuffer(GL_ARRAY_BUFFER, table_vbo);
	glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(attribute_v_normal);
	glVertexAttribPointer(attribute_v_normal, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(glm::vec3)*myTable.vertexNum));
	glEnableVertexAttribArray(attribute_v_texcoord);
	glVertexAttribPointer(attribute_v_texcoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(glm::vec3)*myTable.vertexNum * 2));
	glBindVertexArray(0);

	glGenVertexArrays(1, &select_ball_vao);
	glBindVertexArray(select_ball_vao);
	glBindBuffer(GL_ARRAY_BUFFER, ball_vbo);
	glEnableVertexAttribArray(attribute_v_select_coord);
	glVertexAttribPointer(attribute_v_select_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void initLight()
{
	glm::vec4 lightPos = glm::vec4(0.0, 10.0, 0.0, 1.0);
	glm::vec4 lightAmbient = glm::vec4(0.5, 0.5, 0.5, 1.0);
	glm::vec4 lightDiffuse = glm::vec4(1.0, 1.0, 1.0, 1.0);
	glm::vec4 lightSpecular = glm::vec4(1.0, 1.0, 1.0, 1.0);

	glm::vec4 materialAmbient = glm::vec4(1.0, 1.0, 1.0, 1.0);
	glm::vec4 materialDiffuse = glm::vec4(1.0, 1.0, 1.0, 1.0);
	glm::vec4 materialSpecular = glm::vec4(1.0, 1.0, 1.0, 1.0);
	float materialShine = 100;

	glm::vec4 ambient = lightAmbient * materialAmbient;
	glm::vec4 diffuse = lightDiffuse * materialDiffuse;
	glm::vec4 specular = lightSpecular * materialSpecular;

	glUniform4fv(uniform_ambient, 1, glm::value_ptr(ambient));
	glUniform4fv(uniform_diffuse, 1, glm::value_ptr(diffuse));
	glUniform4fv(uniform_specular, 1, glm::value_ptr(specular));
	glUniform4fv(uniform_lightPos, 1, glm::value_ptr(lightPos));
	glUniform1f(uniform_shine, materialShine);
}

///The use of the shaders and the rotations made with quaternion mathematics have been added to the game from 
///tutorials and through adjusted implementations from other developers. All the credits and my sources can be found publicaly 
///on the video reprot of this Modules Assignement.

quaternion myQuater(screen_width, screen_height);
void quaRotate()
{
	glm::mat4 rot = myQuater.quaRot();
	model = rot * model;
}

void ballSlide() //Main collision function. all collision are called form here          Physics mastercode
{
	for (int i = 0; i<16; i++)  //callision me toixo
	{
		glm::vec3 p = mySpherePhysics.spherePosition[i];

		if (abs(p[0]) >= myTable.width / 2 - 1) // p0  einai h diastash x
		{
			mySpherePhysics.wall = -1.0;
			mySpherePhysics.sphereHitsWall(i);  // an nai tote trexei to collision me ton toixo
		}
		else if (abs(p[2]) >= myTable.length / 2 - 1) //p2 eiani h diastash y (x(0) , y(1), z(2))
		{
			mySpherePhysics.wall = 1.0;
			mySpherePhysics.sphereHitsWall(i); // pali trexei collision
		}

		for (int j = 0; j<16; j++)  //collision me mpala
		{
			float dist = glm::distance(mySpherePhysics.spherePosition[j], p);
			if ((j != i) && (dist <= 1.83))  //otan prokeitai gia diaforetikes mpales kai to dist einai mikrotero apo aktina 
			{
				mySpherePhysics.sphereHitsSphere(i, j); //pai3e collision me mpala i kai mpala j
			}
		}

		mySpherePhysics.activatePockets(i);  //energopoiei tis trypes gia tis mpales

		glm::vec3 v = mySpherePhysics.sphereSpeed[i];  //krata thn taxythta gia thn kathe mpala, einai mesta sto forloop
		glm::vec3 ds = v * mySpherePhysics.deltatime;  // ti kanei to dt, pollaplasiazei thn taxythta me to .dt p den kserw ti einai
		glm::vec3 nextVel = v * (1 + mySpherePhysics.speedVar);  // ypologizei thn taxythta meta thn kroysh  ti einai t dv. logika einai h epivradynsh

		glm::vec3 w = mySpherePhysics.sphereSpeedAngle[i];  //vector gia th taxythta metakroyshs
		glm::vec3 ddw = w * mySpherePhysics.deltatime;  // vektor p pollaplasiazei th taxythta me thn epivradynsh .dt p eidame apo panw.
		float ang = glm::length(ddw); 
		glm::vec3 nw = glm::normalize(ddw);  //normalizing > scaling the above vector elements so it returns a vector length of 1 and eb able to save the angle asn a float.
		glm::vec3 nextAngVel = w * (1 + mySpherePhysics.angleVarOnSpeed);  //angle after the collisiion

		if (glm::length(v) && ang)  
		{
			mySpherePhysics.spherePosition[i] += ds;
			mySpherePhysics.translation[i] = glm::translate(glm::mat4(1.0f), mySpherePhysics.spherePosition[i]); //translation of the ball
			mySpherePhysics.rotation[i] *= glm::rotate(glm::mat4(1.0f), ang, nw);  //rotation of the ball
			if (glm::dot(nextVel, v) > 0.0)  //dot product of the  two vectors into float.checking if ball is still moving
			{
				mySpherePhysics.sphereSpeed[i] = nextVel;  //if it still moves, then put new speed
				mySpherePhysics.sphereSpeedAngle[i] = nextAngVel;  //and put the new angle
			}
			else  
			{		
				mySpherePhysics.sphereSpeed[i] = glm::vec3(0.0f);  //setting ball speed to 0
				mySpherePhysics.sphereSpeedAngle[i] = glm::vec3(0.0f);  //resseting angle in order to have a new collision with resetted angle
			}
		}
	}
	glutPostRedisplay();   //refreshing my window in order to get movement
}


void onDisplay() //https://www.khronos.org/opengl/wiki/Uniform_Buffer_Object
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);
	glUniformMatrix4fv(uniform_pro, 1, GL_FALSE, glm::value_ptr(project));
	glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(view));

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(uniform_mytexture, GL_TEXTURE0);

	initLight();
	ballSlide();

	glBindVertexArray(ball_vao);
	for (int i = 0; i<16; i++)
	{
		glBindTexture(GL_TEXTURE_2D, mytexture_id[i]);
		model = mySpherePhysics.translation[i] * mySpherePhysics.rotation[i];
		glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(model));
		modelview = view * model*mySpherePhysics.translation[i] * mySpherePhysics.rotation[i];
		glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(modelview));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, myBall.vertexNum);
	}

	glBindVertexArray(table_vao);
	glBindTexture(GL_TEXTURE_2D, mytexture_id[16]);
	model = glm::scale(glm::mat4(1.0f), glm::vec3(myTable.width / 2, myTable.height, myTable.length / 2));
	glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(model));

	modelview = view * model*glm::scale(glm::mat4(1.0f), glm::vec3(myTable.width / 2, myTable.height, myTable.length / 2));
	glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(modelview));
	glDrawArrays(GL_TRIANGLES, 0, myTable.vertexNum);

	glutSwapBuffers();
}

setShotSpeed myShot;

void renderSelection()  //https://www.khronos.org/opengl/wiki/Uniform_Buffer_Object
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(select_program);
	glUniformMatrix4fv(uniform_select_pro, 1, GL_FALSE, glm::value_ptr(project));
	glUniformMatrix4fv(uniform_select_v, 1, GL_FALSE, glm::value_ptr(view));

	glBindVertexArray(select_ball_vao);
	for (int i = 0; i<16; i++)
	{
		model = mySpherePhysics.translation[i] * mySpherePhysics.rotation[i];
		glUniformMatrix4fv(uniform_select_m, 1, GL_FALSE, glm::value_ptr(model));
		modelview = view * model*mySpherePhysics.translation[i] * mySpherePhysics.rotation[i];
		glUniformMatrix4fv(uniform_select_mv, 1, GL_FALSE, glm::value_ptr(modelview));

		glUniform1i(uniform_ballID, i + 1);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, myBall.vertexNum);
	}

	glClearColor(0.0, 0.0, 0.0, 1.0);
}


void onReshape(int width, int height) // when adjusting the window size, refresh the screen width and height variables.
{
	screen_width = width;
	screen_height = height;
	glViewport(0, 0, screen_width, screen_height);
}


void keyfunc(unsigned char key, int x, int y)  //escape button to close the game window.
{
	switch (key) {
	case 27:
		exit(0);
		break;
	}
}

void processSelection(int x, int y)    //running the render and selectign the moving balls in order to refresh the window to the new positions.
{
	unsigned char res[4];
	renderSelection();
	glReadPixels(x, screen_height - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &res);
	myShot.ballID = res[0] - 1;
	myShot.setSpeed();
	if (myShot.ballID != -1)
	{
		mySpherePhysics.sphereSpeed[myShot.ballID] = myShot.speed;
		mySpherePhysics.sphereSpeedAngle[myShot.ballID] = myShot.speedAngle;
	}
	std::cout << "myBallPhy.ballVel[myShot.ballID]: " << myShot.speed[0] << ", " << myShot.speed[1] << std::endl;
}


void onMouse(int button, int state, int x, int y)   // getting the coordinates of my mouse positions x and y and activating bool for keeping the mouse down for onMotion function 
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)   //saved last position (time of the click) for myshot function
	{
		myShot.starting_mouse_x = x;
		myShot.starting_mouse_y = y;
		myShot.tracking = true;
	}
	else myShot.tracking = false;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)   //saved last position (time of the click) for myQuaternions function
	{
		myQuater.last_mx = myQuater.cur_mx = x;
		myQuater.last_my = myQuater.cur_my = y;
		myQuater.trackball_on = true;
	}
	else myQuater.trackball_on = false;
}


void onMotion(int x, int y)  // updating function for my mouse coordinates while button is pressed just like above,but for current mouse position.
{
	if (myShot.tracking)
	{
		myShot.follow_mouse_x = x;
		myShot.follow_mouse_y = y;
		processSelection(x, y);
		printf("cur_mx: %d, cur_my: %d\n", myShot.follow_mouse_x, myShot.follow_mouse_y);
	}

	if (myQuater.trackball_on)
	{
		myQuater.cur_mx = x;
		myQuater.cur_my = y;
	}

}


void free_resources()       //deleting  saved data to make program faster.
{
	glDeleteProgram(program);
	glDeleteBuffers(1, &ball_vbo);
	glDeleteBuffers(1, &table_vbo);
}

int main(int argc, char* argv[])  //function caller
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(screen_width, screen_height);
	glutCreateWindow("Billiard Simulation");


	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return EXIT_FAILURE;
	}

	if (!GLEW_VERSION_2_0) {
		fprintf(stderr, "Error: your graphic card does not support OpenGL 2.0\n");
		return EXIT_FAILURE;
	}

	if (init_resources()) {
		initVAO();
		glutDisplayFunc(onDisplay);
		glutReshapeFunc(onReshape);
		glutMouseFunc(onMouse);
		glutMotionFunc(onMotion);
		glutKeyboardFunc(keyfunc);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glutMainLoop();
	}

	free_resources();
	return EXIT_SUCCESS;
}