#pragma once
#ifndef SHADER_H
#define SHADER_H

#include "../ew/external/glad.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

//Based on the LearnOpenGL tutorial at this linK: https://tinyurl.com/3kbvfjf7

class Shader
{
public:
	unsigned int ID;

	Shader(const char* vertexPath, const char* fragmentPath);

	void use();

	void setBool(const std::string& name, bool value) const;

	void setInt(const std::string& name, int value) const;

	void setFloat(const std::string& name, float value) const;

private:

	void checkCompileErrors(unsigned int shader, std::string type);
};
#endif