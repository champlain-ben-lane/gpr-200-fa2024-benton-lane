#include <stdio.h>
#include <math.h>
#include <iostream>
#include <ew/external/glad.h>
#include <ew/external/stb_image.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "../core/bl_library/shader.h"
#include "../core/bl_library/texture.h"
using namespace std;

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	Shader backgroundShader("assets/shaders/backgroundShader.vert", "assets/shaders/backgroundShader.frag");
	Shader characterShader("assets/shaders/characterShader.vert", "assets/shaders/characterShader.frag");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

	//Info for background vertices and indices
	float verticesBG[] = {
		// positions          // colors           // texture coords
		 1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   10.0f, 10.0f, // top right
		 1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   10.0f, 0.0f, // bottom right
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 10.0f  // top left 
	};
	unsigned int indicesBG[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	//Info for foreground vertices and indices
	float verticesFG[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indicesFG[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	// Time to put it all together
	unsigned int VAO, VBO_BG, EBO_BG, VBO_FG, EBO_FG;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_BG);
	glGenBuffers(1, &EBO_BG);
	glGenBuffers(1, &VBO_FG);
	glGenBuffers(1, &EBO_FG);

	// load and create a texture 
	// -------------------------

	Texture texture1("assets/textures/portal_wall_texture.png", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);
	Texture texture2("assets/textures/portal_turret_bg.png", GL_LINEAR, GL_REPEAT);
	Texture texture3("assets/textures/portal_cube.png", GL_NEAREST, GL_REPEAT);


	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	backgroundShader.use(); // don't forget to activate/use the shader before setting uniforms!
	// either set it manually like so:
	//glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
	// or set it via the texture class
	backgroundShader.setInt("texture1", 0);
	backgroundShader.setInt("texture2", 1);

	characterShader.use();
	characterShader.setInt("texture3", 2);

	//Render loop
	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();

		float time = (float)glfwGetTime();

		//Clear framebuffer
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f); // This is the background color assignment, lovely shade of blue at the moment
		glClear(GL_COLOR_BUFFER_BIT);

		// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_BG);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBG), verticesBG, GL_STATIC_DRAW);
		// Could also use glNamedBufferData(VBO, sizeof(vertices), vertices, GL_STATIC_DRAW) instead of the previous two lines

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_BG);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesBG), indicesBG, GL_STATIC_DRAW);

		// Position info
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Color info 
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// Texture coordinate info
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		//Drawing happens here!

		// bind textures on corresponding texture units
		texture1.Bind(GL_TEXTURE0);
		texture2.Bind(GL_TEXTURE1);
		texture3.Bind(GL_TEXTURE2);

		//Render the container
		backgroundShader.use();
		glBindVertexArray(VAO);

		//Draw call
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_FG);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesFG), verticesFG, GL_STATIC_DRAW);
		// Could also use glNamedBufferData(VBO, sizeof(vertices), vertices, GL_STATIC_DRAW) instead of the previous two lines

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_FG);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesFG), indicesFG, GL_STATIC_DRAW);

		// Position info
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Color info 
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// Texture coordinate info
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		//Render the container
		characterShader.use();
		int timeLoc = glGetUniformLocation(characterShader.ID, "uTime");
		glUniform1f(timeLoc, time);

		//Draw call
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
	}

	glfwTerminate();

	printf("Shutting down...");

}
