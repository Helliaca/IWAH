#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

void print(std::string s) {
	std::cout << s << std::endl;
}

void checkErrors(std::string context = "Unknwon Context") {
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		print("OpenGL Error:");
		print("\t CONTEXT: " + context);
		if (err == GL_INVALID_ENUM) print("\t TYPE: Invalid Enum");
		else if (err == GL_INVALID_VALUE) print("\t TYPE: Invalid Value");
		else if (err == GL_INVALID_OPERATION) print("\t TYPE: Invalid Operation");
		else if (err == GL_OUT_OF_MEMORY) print("\t TYPE: Out of Memory");
		else if (err == GL_INVALID_FRAMEBUFFER_OPERATION) print("\t TYPE: Invalid Framebuffer Operation");
		else if (err == GL_CONTEXT_LOST) print("\t TYPE: Context Lost");
		else print("\t TYPE: Undefined Error");
	}
}