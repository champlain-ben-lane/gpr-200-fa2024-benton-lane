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
#include "../core/bl_library/mesh.h"
#include "../core/bl_library/model.h"
// IMGUI stuff
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
// Assimp stuff
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// Screen settings
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

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
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

// IMGui Variables
float ambientStrength = 0.5;
float diffuseStrength = 0.5;
float specularStrength = 0.5;
int shininessStrength = 250;

int main() {
	// GLFW: Initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// GLFW window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Assignment_5", NULL, NULL);
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

	// Tell GLFW to grab da mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// AFTER gladLoadGL
	// Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// Set up global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	/*Shader lightingShader("assets/shaders/basicLighting.vert", "assets/shaders/basicLighting.frag");
	Shader lightCubeShader("assets/shaders/lightCube.vert", "assets/shaders/lightCube.frag");*/

	Shader testShader("assets/shaders/modelTest.vert", "assets/shaders/modelTest.frag");

	// Load in models
	// -----------------------------

	Model testModel("assets/models/Survival_BackPack_2.fbx");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	//glm::vec3 tangents[] = {
	//	lightingShader.calculateNormalTangent(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f),glm::vec3(0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 0.0f),glm::vec2(1.0f, 0.0f),glm::vec2(1.0f, 1.0f)),
	//	lightingShader.calculateNormalTangent(glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(-0.5f,  0.5f, -0.5f),glm::vec3(-0.5f, -0.5f, -0.5), glm::vec2(1.0f, 1.0f),glm::vec2(0.0f, 1.0f),glm::vec2(0.0f, 0.0f)),

	//	lightingShader.calculateNormalTangent(glm::vec3(-0.5f, -0.5f,  0.5), glm::vec3(0.5f, -0.5f,  0.5f),glm::vec3(0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 0.0f),glm::vec2(1.0f, 0.0f),glm::vec2(1.0f, 1.0f)),
	//	lightingShader.calculateNormalTangent(glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(-0.5f,  0.5f,  0.5f),glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 1.0f),glm::vec2(0.0f, 1.0f),glm::vec2(0.0f, 0.0f)),

	//	lightingShader.calculateNormalTangent(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-0.5f,  0.5f, -0.5f),glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f),glm::vec2(1.0f, 1.0f),glm::vec2(0.0f, 1.0f)),
	//	lightingShader.calculateNormalTangent(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f,  0.5f),glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f),glm::vec2(0.0f, 0.0f),glm::vec2(1.0f, 0.0f)),

	//	lightingShader.calculateNormalTangent(glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.5f,  0.5f, -0.5f),glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f),glm::vec2(1.0f, 1.0f),glm::vec2(0.0f, 1.0f)),
	//	lightingShader.calculateNormalTangent(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f,  0.5f),glm::vec3(0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f),glm::vec2(0.0f, 0.0f),glm::vec2(1.0f, 0.0f)),

	//	lightingShader.calculateNormalTangent(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f),glm::vec3(0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 1.0f),glm::vec2(1.0f, 1.0f),glm::vec2(1.0f, 0.0f)),
	//	lightingShader.calculateNormalTangent(glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(-0.5f, -0.5f,  0.5f),glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f),glm::vec2(0.0f, 0.0f),glm::vec2(0.0f, 1.0f)),

	//	lightingShader.calculateNormalTangent(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.5f,  0.5f, -0.5f),glm::vec3(0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f),glm::vec2(1.0f, 1.0f),glm::vec2(1.0f, 0.0f)),
	//	lightingShader.calculateNormalTangent(glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(-0.5f,  0.5f,  0.5f),glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 0.0f),glm::vec2(0.0f, 0.0f),glm::vec2(0.0f, 1.0f))

	//};
	//float vertices[] = {
	//   //Positions            Normals              Texture     Normal Tangents
	//	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, tangents[0].x, tangents[0].y,tangents[0].z,
	//	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f, tangents[0].x, tangents[0].y,tangents[0].z,
	//	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, tangents[0].x, tangents[0].y,tangents[0].z,
	//	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, tangents[1].x, tangents[1].y,tangents[1].z,
	//	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f, tangents[1].x, tangents[1].y,tangents[1].z,
	//	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, tangents[1].x, tangents[1].y,tangents[1].z,

	//	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, tangents[2].x, tangents[2].y,tangents[2].z,
	//	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f, tangents[2].x, tangents[2].y,tangents[2].z,
	//	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, tangents[2].x, tangents[2].y,tangents[2].z,
	//	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, tangents[3].x, tangents[3].y,tangents[3].z,
	//	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, tangents[3].x, tangents[3].y,tangents[3].z,
	//	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, tangents[3].x, tangents[3].y,tangents[3].z,

	//	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f, tangents[4].x, tangents[4].y,tangents[4].z,
	//	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, tangents[4].x, tangents[4].y,tangents[4].z,
	//	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f, tangents[4].x, tangents[4].y,tangents[4].z,
	//	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f, tangents[5].x, tangents[5].y,tangents[5].z,
	//	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, tangents[5].x, tangents[5].y,tangents[5].z,
	//	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f, tangents[5].x, tangents[5].y,tangents[5].z,

	//	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, tangents[6].x, tangents[6].y,tangents[6].z,
	//	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, tangents[6].x, tangents[6].y,tangents[6].z,
	//	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f, tangents[6].x, tangents[6].y,tangents[6].z,
	//	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f, tangents[7].x, tangents[7].y,tangents[7].z,
	//	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, tangents[7].x, tangents[7].y,tangents[7].z,
	//	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, tangents[7].x, tangents[7].y,tangents[7].z,

	//	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f, tangents[8].x, tangents[8].y,tangents[8].z,
	//	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f, tangents[8].x, tangents[8].y,tangents[8].z,
	//	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f, tangents[8].x, tangents[8].y,tangents[8].z,
	//	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f, tangents[9].x, tangents[9].y,tangents[9].z,
	//	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, tangents[9].x, tangents[9].y,tangents[9].z,
	//	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f, tangents[9].x, tangents[9].y,tangents[9].z,

	//	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, tangents[10].x, tangents[10].y,tangents[10].z,
	//	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, tangents[10].x, tangents[10].y,tangents[10].z,
	//	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, tangents[10].x, tangents[10].y,tangents[10].z,
	//	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, tangents[11].x, tangents[11].y,tangents[11].z,
	//	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, tangents[11].x, tangents[11].y,tangents[11].z,
	//	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, tangents[11].x, tangents[11].y,tangents[11].z
	//};

	//// first, configure the cube's VAO (and VBO)
	//unsigned int VBO, cubeVAO;
	//glGenVertexArrays(1, &cubeVAO);
	//glGenBuffers(1, &VBO);

	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glBindVertexArray(cubeVAO);

	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	//glEnableVertexAttribArray(3);
	//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	//glBindVertexArray(0);

	//// Time to make some textures 
	//// -------------------------

	//t::Texture texture1("assets/textures/portal_wall_texture.png", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);
	//t::Texture normal1("assets/textures/alt_normal.jpg", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);

	//// Tell OpenGL for each sampler to which texture unit it belongs to (only has to be done once)
	//// -------------------------------------------------------------------------------------------
	///*lightingShader.use();
	//lightingShader.setInt("texture1", 0);
	//lightingShader.setInt("normal1", 1);*/

	//// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	//unsigned int lightCubeVAO;
	//glGenVertexArrays(1, &lightCubeVAO);
	//glBindVertexArray(lightCubeVAO);

	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);

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

		// be sure to activate shader when setting uniforms/drawing objects
		/*lightingShader.use();
		lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("lightColor", lightColor);
		lightingShader.setVec3("lightPos", lightPos);
		lightingShader.setVec3("viewPos", cameraPos);
		lightingShader.setFloat("ambientStrength", ambientStrength);
		lightingShader.setFloat("diffuseStrength", diffuseStrength);
		lightingShader.setFloat("specularStrength", specularStrength);
		lightingShader.setInt("shininessStrength", shininessStrength);*/

		// Passing ze projection matrix to ze shader
		glm::mat4 projection;

		// Switch between perspectives at the fickle whims of the user
		if (isPerspectiveOn) {
			projection = glm::perspective(glm::radians(fov), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);
		}
		else {
			projection = glm::ortho(-(float)SCREEN_WIDTH / 50, (float)SCREEN_WIDTH / 50, -(float)SCREEN_HEIGHT / 50, (float)SCREEN_HEIGHT / 50, 0.1f, 1000.0f);
		}

		/*lightingShader.setMat4("projection", projection);*/

		// Camera/view transformation
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		/*lightingShader.setMat4("view", view);*/

		// world transformation
		/*glm::mat4 model = glm::mat4(1.0f);*/
		//model = glm::rotate(model, (float)glm::radians(glfwGetTime() * 6), glm::vec3(1, 1, 0));
		/*lightingShader.setMat4("model", model);*/

		// render the cube
		/*glBindVertexArray(cubeVAO);*/

		// Bind textures on corresponding texture units
		/*texture1.Bind(GL_TEXTURE0);
		normal1.Bind(GL_TEXTURE1);
		glDrawArrays(GL_TRIANGLES, 0, 36);*/

		// also draw the lamp object
		//lightCubeShader.use();
		//lightCubeShader.setMat4("projection", projection);
		//lightCubeShader.setMat4("view", view);
		//lightCubeShader.setVec3("lightColor", lightColor);
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, lightPos);
		//model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		//lightCubeShader.setMat4("model", model);

		//glBindVertexArray(lightCubeVAO);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		// Start of test code for model loading
		testShader.use();

		
		testShader.setMat4("projection", projection);
		testShader.setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		testShader.setMat4("model", model);
		testModel.Draw(testShader);
		// End of test code for model loading

		// Start drawing ImGUI
		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		// Create a window called Settings
		ImGui::Begin("Settings");
		ImGui::DragFloat3("Light Position", &lightPos.x, 0.1f);
		ImGui::ColorEdit3("Light Color", &lightColor.r);
		ImGui::SliderFloat("Ambient Strength", &ambientStrength, 0.0f, 1.0f);
		ImGui::SliderFloat("Diffuse Strength", &diffuseStrength, 0.0f, 1.0f);
		ImGui::SliderFloat("Specular Strength", &specularStrength, 0.0f, 1.0f);
		ImGui::SliderInt("S H I N I N E S S", &shininessStrength, 2, 1024);
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
	/*glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightCubeVAO);
	glDeleteBuffers(1, &VBO);*/

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
	//if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) != GLFW_PRESS) // If the mouse isn't being pressed, don't do anything
	//	return;

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_RELEASE)
	{
		firstMouse = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		return;
	}
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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