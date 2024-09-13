#include <stdio.h>
#include <math.h>
#include <iostream>
#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
using namespace std;

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

//Vertex shader source
const char* vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"layout (location = 1) in vec4 aColor;\n"
	"out vec4 Color;\n"
	"uniform float uTime;\n"
	"void main()\n"
	"{\n"
	"   Color = aColor;\n"
	"   vec3 pos = aPos;\n"
	"   pos.y += sin(uTime * 5.0 + pos.x) / 4.0;\n" // This is the line that makes the triangle bounce up and down
	"   gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);\n"
	"}\0";

//Fragment shader source
const char* fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"in vec4 Color;\n"
	"uniform float uTime = 1.0f;\n"
	"uniform vec4 uColor = vec4(1.0);\n"
	"void main()\n"
	"{\n"
	"   FragColor = Color * (sin(uTime) * 5.0 * 0.5 + 0.5);\n" // This is the line that makes the colors go weeeee
	"}\n\0";

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
	//Initialization goes here...? Yeah, here is good.

	//Vertex shader creation
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//Vertex shader error check
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	}

	//Fragment shader creation
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//Fragment shader error check
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
	}

	//Time to make a shader program (yo dawg, we heard you like programs, so we made a program inside your program)
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	//Shader program error check
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	//Delete the shaders, don't need 'em anymore
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//Triangle(s)
	float vertices[] = {
	//    X      Y     Z     R     G     B     A
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,	0.0f, 1.0f, 0.0f, 1.0f,
		 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
	};

	// Time to put it all together
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Could also use glNamedBufferData(VBO, sizeof(vertices), vertices, GL_STATIC_DRAW) instead of the previous two lines

	//Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Color (RGBA)
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	
	//Render loop
	while (!glfwWindowShouldClose(window)) {
		
		glfwPollEvents();

		float time = (float)glfwGetTime();
		
		//Clear framebuffer
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f); // This is the background color assignment, lovely shade of blue at the moment
		glClear(GL_COLOR_BUFFER_BIT);
		
		//Drawing happens here!
		glUseProgram(shaderProgram);
		int timeLoc = glGetUniformLocation(shaderProgram, "uTime");
		glUniform1f(timeLoc, time);
		glBindVertexArray(VAO);
		
		//Draw call
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
	}
	
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
	
	glfwTerminate();

	printf("Shutting down...");

}
