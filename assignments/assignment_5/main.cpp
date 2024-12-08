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
#include "../core/bl_library/camera.h"
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
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCREEN_WIDTH / 2.0;
float lastY = (float)SCREEN_HEIGHT / 2.0;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f;
float timeElapsed = 0.0f;

// Lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);


// Fire Variables
glm::vec3 firePos(0.0f, 0.0f, 0.0f);
glm::vec3 fireColor(1.0f, 0.7f, 0.3f);

// IMGui Variables
float ambientStrength = 0.5;
float diffuseStrength = 0.5;
float specularStrength = 0.5;
int shininessStrength = 250;
float flickerStrength = 0.5;
float windSpeed = 1.0;
int grassCount = 1000000;

//Leaving this here for the time being for testing purposes: likely want to add to texture.h for clean up purposes
// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

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
	stbi_set_flip_vertically_on_load(false);

	// Set up global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	Shader testShader("assets/shaders/modelTest.vert", "assets/shaders/modelTest.frag");
	Shader skyboxTest("assets/shaders/skyboxTest.vert", "assets/shaders/skyboxTest.frag");
	Shader grassShader("assets/shaders/instancedGrass.vert", "assets/shaders/instancedGrass.frag");
	Shader fireShader("assets/shaders/fireShader.vert", "assets/shaders/fireShader.frag");
	//Shader treeShader("assets/shaders/treeShader.vert", "assets/shaders/treeShader.frag");
	Shader groundShader("assets/shaders/groundPlane.vert", "assets/shaders/groundPlane.frag");

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// load skybox textures
	vector<std::string> faces
	{
		("assets/textures/Skybox/night_right.jpg"),
		("assets/textures/Skybox/night_left.jpg"),
		("assets/textures/Skybox/night_top.jpg"),
		("assets/textures/Skybox/night_bottom.jpg"),
		("assets/textures/Skybox/night_front.jpg"),
		("assets/textures/Skybox/night_back.jpg")
	};

	unsigned int cubemapTexture = loadCubemap(faces);

	// Load in models
	// -----------------------------

	//Model testModel("assets/models/backpack/backpack.obj");
	Model testChair("assets/models/chair/chair.obj");
	Model testFirepit("assets/models/firepit/pit.obj");
	Model testLogs("assets/models/logs/logs.obj");
	Model testGrass("assets/models/grass2.fbx");

	//grass instancing work
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[grassCount];
	srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
	for (unsigned int i = 0; i < grassCount; i++)
	{
		int randy = rand();
		glm::mat4 model = glm::mat4(1.0f);
		glm::vec3 displacement = glm::vec3(randy % 5000 / 100.0f - 25.0f, 0, rand() % 5000 / 100.0f - 25.0f);
		model = glm::translate(model, displacement);

		float scale = static_cast<float>(randy % 18 + 8.0);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.5f, 1.25f) / scale);

		float rotAngle = static_cast<float>((randy % 360));
		model = glm::rotate(model, rotAngle, glm::vec3(0.0f, 0.0f, 1.0f));

		modelMatrices[i] = model;
	}

	// configure instanced array
	// -------------------------
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, grassCount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	// set transformation matrices as an instance vertex attribute (with divisor 1)
	// note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
	// normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
	// -----------------------------------------------------------------------------------------------------------------------------------
	for (unsigned int i = 0; i < testGrass.meshes.size(); i++)
	{
		unsigned int VAO = testGrass.meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}
	t::Texture fireNoise("assets/textures/fire_noise.png", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);
	t::Texture fireGradient("assets/textures/fire_gradient.png", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);
	//t::Texture treeTex("assets/textures/tree.png", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);

	// shader configuration
	skyboxTest.use();
	skyboxTest.setInt("skybox", 0);
	fireShader.use();
	fireShader.setInt("noiseTex", 2);
	fireShader.setInt("gradientTex", 3);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	// Time to set up a ground plane for...the ground - Ben
	//---------------------------

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float planeVertices[] = {
		// positions            // normals         // texcoords
		 25.0f, 0.0f,  25.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-25.0f, 0.0f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-25.0f, 0.0f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

		 25.0f, 0.0f,  25.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-25.0f, 0.0f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
		 25.0f, 0.0f, -25.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
	};
	// plane VAO
	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);

	t::Texture groundPlane("assets/textures/groundPlane.jpg", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);

	groundShader.use();
	groundShader.setInt("texture1", 0);

	// Fiyah stuff - Olivia
	// --------------------------

	// fire vertices
	float fireVertices[] = {
		// Positions          Texture Coords
		-0.425f,  0.425f, 0.0f, 0.0f, 0.0f,
		 0.425f,  0.425f, 0.0f, 1.0f, 0.0f,
		 0.425f, -0.425f, 0.0f, 1.0f, 1.0f,
		-0.425f, -0.425f, 0.0f, 0.0f, 1.0f,
	};

	//fire VAO and VBO
	unsigned int fireVAO, fireVBO;

	glGenVertexArrays(1, &fireVAO);
	glGenBuffers(1, &fireVBO);

	glBindBuffer(GL_ARRAY_BUFFER, fireVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fireVertices), fireVertices, GL_STATIC_DRAW);

	glBindVertexArray(fireVAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	// tree stuff - the Lorax
	// ----------------------

	// tree vertices
	float treeVertices[] = {
		// Positions          Texture Coords
		-1.0f, 0.0f, -10.0f, 0.0f, 0.0f,
		1.0f, 0.0f, -10.0f, 1.0f, 0.0f,
		1.0f, 5.0f, -10.0f, 1.0f, 1.0f,
		-1.0f, 5.0f, -10.0f, 0.0f, 1.0f,
	};

	//Tree VAO and VBO
	unsigned int treeVAO, treeVBO;

	glGenVertexArrays(1, &treeVAO);
	glGenBuffers(1, &treeVBO);

	glBindBuffer(GL_ARRAY_BUFFER, treeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(treeVertices), treeVertices, GL_STATIC_DRAW);

	glBindVertexArray(treeVAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);


	//Render loop
	while (!glfwWindowShouldClose(window)) {

		// Per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		timeElapsed += deltaTime;

		// Input processing
		// -----
		processInput(window);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Rendering
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Passing ze projection matrix to ze shader
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

		// Camera/view transformation
		glm::mat4 view = camera.GetViewMatrix();

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);

		// Start of test code for model loading
		testShader.use();
		testShader.setVec3("viewPos", camera.Position);
		testShader.setMat4("projection", projection);
		testShader.setMat4("view", view);
		testShader.setVec3("lightColor", lightColor);
		testShader.setVec3("lightPos", lightPos);
		// view-projection and rotation matrix
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 fireModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f));
		glm::mat4 VP = projection * view * fireModel;

		// render the trees
		//treeShader.use();
		//treeShader.setMat4("VP", VP);
		//treeShader.setVec3("BillboardPos", treePos);
		//treeShader.setVec2("BillboardSize", glm::vec2(5.0f, 2.0f));
		//treeShader.setVec3("CameraRight_worldspace", cameraFront);
		//treeShader.setVec3("CameraUp_worldspace", cameraUp);
		//treeShader.setFloat("timeElapsed", timeElapsed);

		glBindVertexArray(treeVAO);
		//treeTex.Bind(GL_TEXTURE4);
		//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		testShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 2.0f)); // translate it down so it's at the center of the scene
		//model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		testShader.setMat4("model", model);
		testChair.Draw(testShader);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		//model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		testShader.setMat4("model", model);
		testFirepit.Draw(testShader);
		testLogs.Draw(testShader);

		//grass rendering
		grassShader.use();
		grassShader.setMat4("projection", projection);
		grassShader.setMat4("view", view);
		grassShader.setFloat("time", glfwGetTime());
		for (unsigned int i = 0; i < testGrass.meshes.size(); i++)
		{
			glBindVertexArray(testGrass.meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(testGrass.meshes[i].indices.size()), GL_UNSIGNED_INT, 0, grassCount);
			glBindVertexArray(0);
		}
		// End of test code for model loading

		// render ground plane - Ben
		groundShader.use();
		groundShader.setMat4("projection", projection);
		groundShader.setMat4("view", view);
		groundShader.setVec3("viewPos", camera.Position);
		groundShader.setVec3("lightPos", lightPos);
		glBindVertexArray(planeVAO);
		glActiveTexture(GL_TEXTURE0);
		groundPlane.Bind(GL_TEXTURE0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Draw skybox as last
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content. Otherwise C U B E bug
		skyboxTest.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // Remove translation from the view matrix - it's the sky dummy, it doesn't move when you move
		skyboxTest.setMat4("view", view);
		skyboxTest.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // Set depth function back to default just in case you need again for laterd

		// render the fire quad - Olivia
		fireShader.use();
		fireShader.setMat4("VP", VP);
		fireShader.setMat4("projection", projection);
		fireShader.setVec3("BillboardPos", firePos);
		fireShader.setVec2("BillboardSize", glm::vec2(0.85f, 0.85f));
		fireShader.setVec3("CameraRight_worldspace", camera.Right);
		fireShader.setVec3("CameraUp_worldspace", camera.Up);
		fireShader.setVec3("CameraFront_worldspace", camera.Front);
		fireShader.setFloat("timeElapsed", timeElapsed);
		fireShader.setFloat("flickerStrength", flickerStrength);
		fireShader.setFloat("windSpeed", windSpeed);

		glBindVertexArray(fireVAO);
		fireNoise.Bind(GL_TEXTURE2);
		fireGradient.Bind(GL_TEXTURE3);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		
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
		ImGui::SliderFloat("Flicker Strength", &flickerStrength, 0.0f, 1.0f);
		ImGui::SliderFloat("Wind Speed", &windSpeed, 0.0f, 10.0f);
		ImGui::SliderInt("GRASS", &grassCount, 100000, 1000000);
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
		camera.Position += cameraSpeed * camera.Front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // Move backward
		camera.Position -= cameraSpeed * camera.Front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // Move left
		camera.Position -= glm::normalize(glm::cross(camera.Front, camera.Up)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // Move right
		camera.Position += glm::normalize(glm::cross(camera.Front, camera.Up)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) // Move up
		camera.Position -= glm::normalize(glm::cross(camera.Front, camera.Right)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) // Move down
		camera.Position += glm::normalize(glm::cross(camera.Front, camera.Right)) * cameraSpeed;
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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// GLFW: Whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

