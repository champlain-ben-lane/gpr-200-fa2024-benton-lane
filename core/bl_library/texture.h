#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdio.h>
#include <math.h>
#include <iostream>
#include "../ew/external/glad.h"
#include "../ew/external/stb_image.h"
#include "../ew/ewMath/ewMath.h"
#include <GLFW/glfw3.h>
#include "../glm/glm.hpp"
namespace t {
	class Texture
	{
	public:

		Texture(const char* filePath, int filterMode, int wrapMode);

		~Texture() {}

		void Bind(unsigned int slot); //Bind to a specific texture unit

	private:

		unsigned int m_id; //GL texture handle

		int m_width, m_height;
	};
}
#endif