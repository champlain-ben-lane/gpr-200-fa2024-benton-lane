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
float timeElapsed = 0.0f;

// Lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);


// Fire Variables
glm::vec3 firePos(0.0f, 0.0f, 0.0f);
glm::vec3 fireColor(1.0f, 0.7f, 0.3f);
glm::vec3 fireJitter = firePos;
float fireSin1;
float fireSin2;

// IMGui Variables
float ambientStrength = 0.5;
float diffuseStrength = 0.5;
float specularStrength = 0.5;
int shininessStrength = 250;
float flickerStrength = 0.5;
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

	// load textures
	vector<std::string> faces
	{
		("assets/textures/Skybox/right.jpg"),
		("assets/textures/Skybox/left.jpg"),
		("assets/textures/Skybox/top.jpg"),
		("assets/textures/Skybox/bottom.jpg"),
		("assets/textures/Skybox/front.jpg"),
		("assets/textures/Skybox/back.jpg")
	};

	unsigned int cubemapTexture = loadCubemap(faces);

	// Load in models
	// -----------------------------

	//Model testModel("assets/models/backpack/backpack.obj");
	Model testChair("assets/models/chair/chair.fbx");
    Model testFirepit("assets/models/firepit/firepit.fbx");
	Model testLogs("assets/models/logs.fbx");
	Model testGrass("assets/models/grass2.fbx");

	//grass instancing work
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[grassCount];
	srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
	for (unsigned int i = 0; i < grassCount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		glm::vec3 displacement = glm::vec3(rand() % 5000 / 100.0f - 25.0f, 0, rand() % 5000 / 100.0f - 25.0f);
		model = glm::translate(model, displacement);

		float scale = static_cast<float>(rand() % 18 + 8.0);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.5f, 2.0f) / scale);

		float rotAngle = static_cast<float>((rand() % 360));
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


	// Fiyah stuff - Olivia
	// --------------------------

	// fire vertices
	float fireVertices[] = {
		// Positions          Texture Coords
		-0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 1.5f, 0.0f, 1.0f, 1.0f,
		-0.5f, 1.5f, 0.0f, 0.0f, 1.0f,
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

	//fire VAO and VBO
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
		glm::mat4 projection;

		// Switch between perspectives at the fickle whims of the user
		if (isPerspectiveOn) {
			projection = glm::perspective(glm::radians(fov), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);
		}
		else {
			projection = glm::ortho(-(float)SCREEN_WIDTH / 50, (float)SCREEN_WIDTH / 50, -(float)SCREEN_HEIGHT / 50, (float)SCREEN_HEIGHT / 50, 0.1f, 1000.0f);
		}

		// Camera/view transformation
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);

		// Start of test code for model loading
		testShader.use();
		testShader.setVec3("viewPos", cameraPos);
		
		testShader.setMat4("projection", projection);
		testShader.setMat4("view", view);
		testShader.setVec3("lightColor", lightColor);
		testShader.setVec3("lightPos", lightPos);
		// view-projection and rotation matrix
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 VP = projection * view * rotation;

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxTest.use();
		//view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxTest.setMat4("view", view);
		skyboxTest.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to defaultd

		

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
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		testShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));	// it's a bit too big for our scene, so scale it down
		testShader.setMat4("model", model);
		testChair.Draw(testShader);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));	// it's a bit too big for our scene, so scale it down
		testShader.setMat4("model", model);
		testFirepit.Draw(testShader);

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

		// render the fire quad - Olivia
		fireShader.use();
		fireShader.setMat4("VP", VP);
		fireShader.setVec3("BillboardPos", firePos);
		fireShader.setVec2("BillboardSize", glm::vec2(1.0f, 1.0f));
		fireShader.setVec3("CameraRight_worldspace", cameraFront);
		fireShader.setVec3("CameraUp_worldspace", cameraUp);
		fireShader.setFloat("timeElapsed", timeElapsed);


		glBindVertexArray(fireVAO);
		fireNoise.Bind(GL_TEXTURE2);
		fireGradient.Bind(GL_TEXTURE3);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		// add fire jitter (offset to firePos so light flickers)
		fireSin1 = sin(timeElapsed);
		fireSin2 = sin(timeElapsed * 1.8f);
		fireJitter.x += (fireSin1 + fireSin2) * 0.7f * (flickerStrength);
		fireJitter.y += (fireSin1 + fireSin2) * 0.5f * (flickerStrength);
		fireJitter.z += (fireSin1 + fireSin2) * 0.9f * (flickerStrength);

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
		ImGui::SliderInt("GRASS", &grassCount, 500000, 20000000);
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

