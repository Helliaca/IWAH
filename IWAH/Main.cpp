//#include <glad/glad.h>
//#include <GLFW/glfw3.h>

#include "core/Shader.h"
#include "core/debug.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include<cmath>
#include <vector>

std::vector<int> rules;
std::vector<int> flips;
int maxlit;

float vertices[] = {-1.0f, -1.0f, 0.0f,	  1.0f, -1.0f, 0.0f,   1.0f,  1.0f, 0.0f,   -1.0f, -1.0f, 0.0f,   -1.0f, 1.0f, 0.0f,   1.0f,  1.0f, 0.0f,};

double stime = 0;

void parse() {
	std::ifstream infile("thefile.txt");
	std::string line;
	maxlit = 0;
	while (std::getline(infile, line))
	{
		std::istringstream is(line);
		int n;
		unsigned int rule = 0, flip = 0;
		while (is >> n) {
			if (n == 0) break;
			if (abs(n) > maxlit) maxlit = abs(n);
			rule += pow(2, abs(n) - 1);
			if (n < 0) flip += pow(2, abs(n) - 1);
		}
		if (rule != 0) {
			rules.push_back(rule);
			flips.push_back(flip);
		}
	}
}

int main() {
	parse();

	//Initialize glfw
	glfwInit();
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	//OpenGL requires a rendering window as context, so we create a 10x10 window for the sake of it.
	GLFWwindow* window = glfwCreateWindow(1600, 1600, "NotSoSmart", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	Shader solve_shader("core/solve.vs", "core/solve.fs");

	//Here we create a 1x1 black texture which we use to obtain the solve shaders results.
	const std::vector<GLubyte> texBuffer(4 * 1, 0); //4 because RGBA
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1, 1);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &texBuffer[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//VBO and VAO of the two triangles we render.
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	unsigned int max_val = pow(2, maxlit);
	unsigned int dim_x = ceil(sqrt(max_val));
	unsigned int dim_y = ceil(sqrt(max_val));
	std::cout << dim_y << std::endl;

	//GLint vp;
	//glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &vp);
	//std::cout << vp << std::endl;
	//glViewport(0, 0, dim_x, dim_y);
	glViewport(0, 0, dim_x, dim_y);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	// render the triangle
	//set uniforms yo
	solve_shader.use();
	checkErrors("asd");

	glBindImageTexture(0, textureID, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
	GLuint location = glGetUniformLocation(solve_shader.ID, "tex2D");
	glUniform1i(location, 0);

	solve_shader.setRuleSet(&rules[0], &flips[0], rules.size());
	checkErrors("3a");
	solve_shader.setInt("rules_size", rules.size());
	solve_shader.setInt("dim_x", dim_x);
	checkErrors("3");

	stime = glfwGetTime();

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	//glfwSwapBuffers(window);
	//glfwPollEvents();

	//retrieve data

	GLubyte* putdatahere;

	glBindTexture(GL_TEXTURE_2D, textureID);

	GLint byte_amount = 1 * 4;

	size_t mem_size = byte_amount * sizeof(GLubyte);
	putdatahere = (GLubyte*)malloc(mem_size); //Allocate image data into memory
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); //Explicit Synchronization, as ImageStore is not always memory coherent
	
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, putdatahere); //Retrieve image data from GPU

	std::cout << glfwGetTime() - stime << std::endl;

	std::cout << std::to_string(putdatahere[0]) << std::endl;


	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	checkErrors("end");
	glfwTerminate();
	while (1) {}
	return 0;
}