// C++ Stuff
#include <stdio.h>
#include <math.h>
#include <iostream>
// Winebrenner stuff
#include <ew/external/glad.h>
#include <ew/external/stb_image.h>
#include <ew/ewMath/ewMath.h>
// GL stuff
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// Ben stuff
#include "../core/bl_library/shader.h"
#include "../core/bl_library/texture.h"
// IMGUI stuff
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// Screen settings
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Camera settings
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraSide = glm::vec3(1.0f, 0.0f, 0.0f);

bool isPerspectiveOn = true;

bool firstMouse = true;
float yaw = -90.0f;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 60.0f;

// Timing
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f;

// Lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// Number of cubes
int numberOfCubes = 20;

int main() {
	// GLFW: Initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// GLFW window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Assignment_4", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	// AFTER gladLoadGL
	// Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Tell GLFW to grab da mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set up global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	Shader lightingShader("assets/shaders/basicLighting.vert", "assets/shaders/basicLighting.frag");
	Shader lightCubeShader("assets/shaders/lightCube.vert", "assets/shaders/lightCube.frag");
	Shader cubeShader("assets/shaders/characterShader.vert", "assets/shaders/characterShader.frag");

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // nnn 00 = 0
		-0.5f, -0.5f, 0.5f,  0.0f, 0.0f,  // nnp 00 = 1
		-0.5f, 0.5f, 0.5f,  0.0f, 1.0f,  // npp 01 = 2
		0.5f, 0.5f, 0.5f,  1.0f, 1.0f,  // ppp 11 = 3
		0.5f, 0.5f, -0.5f,  1.0f, 1.0f,  // ppn 11 = 4
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // pnn 10 = 5
		-0.5f, 0.5f, -0.5f,  0.0f, 1.0f,  // npn 01 = 6
		0.5f, -0.5f, 0.5f,  1.0f, 0.0f,  // pnp 10 = 7
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // nnn 01 = 8
		-0.5f, 0.5f, 0.5f,  1.0f, 0.0f,  // npp 10 = 9
		0.5f, 0.5f, 0.5f,  1.0f, 0.0f,  // ppp 10 = 10
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  // pnn 11 = 11
		-0.5f, 0.5f, -0.5f,  1.0f, 1.0f,  // npn 11 = 12
		0.5f, -0.5f, 0.5f,  0.0f, 0.0f,  // pnp 00 = 13
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  // pnn 01 = 14
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f // npp 00 = 15
	};

	unsigned int indices[] = {
		0, 5, 4,
		4, 6, 0,

		1, 7, 3,
		3, 2, 1,

		9, 12, 8,
		8, 1, 9,

		10, 4, 14,
		14, 13, 10,

		8, 11, 7,
		7, 1, 8,

		6, 4, 10,
		10, 15, 6
	};

	// World space positions of our cubes
	glm::vec3* cubePositions = new glm::vec3[numberOfCubes];
	for (int i = 0; i < numberOfCubes; i++) {
		cubePositions[i] = glm::vec3((ew::RandomRange(-7.0, 7.0)), (ew::RandomRange(-7.0, 7.0)), (ew::RandomRange(-7.0, 7.0)));
	}

	// Time to put it all together for the non-lighting cubes
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Texture coord attributes
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Now time to (try) making a lighting cube
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);

	// Time to make some textures 
	// -------------------------

	Texture texture1("assets/textures/portal_wall_texture.png", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);
	Texture texture2("assets/textures/portal_cube.png", GL_NEAREST, GL_REPEAT);

	// Tell OpenGL for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	cubeShader.use();
	cubeShader.setInt("texture1", 0);
	cubeShader.setInt("texture2", 1);

	//Render loop
	while (!glfwWindowShouldClose(window)) {

		// Per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Input processing
		// -----
		processInput(window);

		// Rendering
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Bind textures on corresponding texture units
		texture1.Bind(GL_TEXTURE0);
		texture2.Bind(GL_TEXTURE1);

		// Use da shader(s)
		cubeShader.use();

		lightingShader.use();
		lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("lightPos", lightPos);

		// Passing ze projection matrix to ze shader
		glm::mat4 projection;

		// Switch between perspectives at the fickle whims of the user
		if (isPerspectiveOn) {
			projection = glm::perspective(glm::radians(fov), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);
		}
		else {
			projection = glm::ortho(-(float)SCREEN_WIDTH / 50, (float)SCREEN_WIDTH / 50, -(float)SCREEN_HEIGHT / 50, (float)SCREEN_HEIGHT / 50, 0.1f, 1000.0f);
		}

		cubeShader.setMat4("projection", projection);

		// Camera/view transformation
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		cubeShader.setMat4("view", view);

		// Render C U B E S
		glBindVertexArray(VAO);
		for (unsigned int i = 0; i < numberOfCubes; i++)
		{
			// Calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 10.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			cubeShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // Second argument = number of elements in the indices array
		}

		// Time to make a light cube
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lightCubeShader.setMat4("model", model);

		glBindVertexArray(lightCubeVAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		// Start drawing ImGUI
		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		// Create a window called Settings
		ImGui::Begin("Settings");
		ImGui::Text("Add Controls Here!");
		ImGui::End();

		// Actually render IMGUI elements using OpenGL
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// GLFW: Swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// De-allocate all resources once they've outlived their purpose: NO MEMORY LEAKS!
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightCubeVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// GLFW: Terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // Close the program
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = static_cast<float>(2.5 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // Moar s p e e d
		cameraSpeed = cameraSpeed * 2;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // Move forward
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // Move backward
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // Move left
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // Move right
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) // Move up
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraSide)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) // Move down
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraSide)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) // Flip projection mode
		isPerspectiveOn = !isPerspectiveOn;
}

// GLFW: Whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// Make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// GLFW: Whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // Change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
	cameraSide = glm::normalize(glm::cross(cameraFront, cameraUp));
}

// GLFW: Whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 120.0f)
		fov = 120.0f;
}