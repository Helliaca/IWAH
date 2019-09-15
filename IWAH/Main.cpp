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

void time(std::string context) {
	print(context + ": " + std::to_string(glfwGetTime() - stime) + "s");
	stime = glfwGetTime();
}

void parse() {
	std::ifstream infile("sample_60k.cnf");
	std::string line;
	maxlit = 0;
	while (std::getline(infile, line))
	{
		std::istringstream is(line);
		std::string in;
		unsigned int rule = 0, flip = 0;
		while (is >> in) {
			if (in=="c" || in=="%" || in=="0") break;
			else {
				int n = std::atoi(in.c_str());
				if (n == 0) break;
				if (abs(n) > maxlit) maxlit = abs(n);
				rule += pow(2, abs(n) - 1);
				if (n < 0) flip += pow(2, abs(n) - 1);
			}
		}
		if (rule != 0) {
			rules.push_back(rule);
			flips.push_back(flip);
		}
	}
}

int main() {
	stime = glfwGetTime();

	//Parse file
	parse();
	time("Parsing time");

	//Initialize glfw
	glfwInit();
	
	//This is the OpenGL window we render to. IF the number of literals is increased beyonbd 21, the resolution will have to be appropriately adjusted.
	GLFWwindow* window = glfwCreateWindow(1600, 1600, "NotSoSmart", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	Shader solve_shader("core/solve.vs", "core/solve.fs");

	//This 1x1 texture will serve as a return value for the solve shader.
	const std::vector<GLubyte> texBuffer(4, 0); //4 because RGBA
	unsigned int ret_tex;
	glGenTextures(1, &ret_tex);
	glBindTexture(GL_TEXTURE_2D, ret_tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1, 1);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &texBuffer[0]);
	GLubyte* result = (GLubyte*)malloc(texBuffer.size() * sizeof(GLubyte)); //Used below at result retrieval

	//Buffer object for our ruleset
	GLuint rules_buffer;
	glGenBuffers(1, &rules_buffer);
	glBindBuffer(GL_TEXTURE_BUFFER, rules_buffer);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(GLint)*rules.size(), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_TEXTURE_BUFFER, 0, sizeof(GLint)*rules.size(), &rules[0]);
	//Texture wrapper for the object above
	GLuint rules_tex;
	glGenTextures(1, &rules_tex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, rules_tex);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32UI, rules_buffer);

	//Buffer object for our flips
	GLuint flips_buffer;
	glGenBuffers(1, &flips_buffer);
	glBindBuffer(GL_TEXTURE_BUFFER, flips_buffer);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(GLint)*flips.size(), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_TEXTURE_BUFFER, 0, sizeof(GLint)*flips.size(), &flips[0]);
	//Texture wrapper for the object above
	GLuint flips_tex;
	glGenTextures(1, &flips_tex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_BUFFER, flips_tex);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32UI, flips_buffer);

	//VBO and VAO of the two triangles we render.
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Set viewport and other rendering options
	unsigned int max_val = pow(2, maxlit);
	unsigned int dim_x = ceil(sqrt(max_val));
	unsigned int dim_y = dim_x;
	glViewport(0, 0, dim_x, dim_y);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	//Activating shader and setting uniforms
	solve_shader.use();
	glBindImageTexture(0, ret_tex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
	glUniform1i(glGetUniformLocation(solve_shader.ID, "ret"), 0);
	glUniform1i(glGetUniformLocation(solve_shader.ID, "rules"), 1);
	glUniform1i(glGetUniformLocation(solve_shader.ID, "flips"), 2);
	solve_shader.setInt("rules_size", rules.size());
	solve_shader.setInt("dim_x", dim_x);

	time("Setup time");

	//Render
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//Retrieve result
	glBindTexture(GL_TEXTURE_2D, ret_tex);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); //Explicit Synchronization, as ImageStore is not always memory coherent
	glFinish(); //Block main thread until rendering is complete to get accurate time measurements
	time("Render time");
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, result); //Retrieve image data from GPU
	time("Result retrieval");

	//Misc. Output
	print("Total literals: " + std::to_string(maxlit));
	print("Total rules: " + std::to_string(rules.size()));
	print("Total cells: " + std::to_string(max_val));
	if (result[0] != 0) print("Result: SATISFIABLE");
	else print("Result: UNSATISFIABLE");

	//Destroy redundat objects and exit
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	free(result);

	checkErrors("end");
	glfwTerminate();
	while (1) {}
	return 0;
}