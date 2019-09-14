#include "Engine.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include<cmath>

Engine::Engine() {}

Engine::~Engine() {}

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


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

void Engine::parse() {
	std::ifstream infile("thefile.txt");
	std::string line;
	maxlit = 0;
	while (std::getline(infile, line))
	{
		std::istringstream is(line);
		int n;
		unsigned int rule=0, flip=0;
		while (is >> n) {
			if (n == 0) break;
			if (abs(n) > maxlit) maxlit = abs(n);
			rule += pow(2, abs(n)-1);
			if (n < 0) flip += pow(2, abs(n)-1);
		}
		rules.push_back(rule);
		flips.push_back(flip);
	}
}

void Engine::run() {
	parse();
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
														 // glfw window creation
														 // --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "NotSoSmart", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}
	
	// build and compile our shader program
	// ------------------------------------
	Shader ourShader("core/solve.vs", "core/solve.fs"); // you can name your shader files however you like

														// set up vertex data (and buffer(s)) and configure vertex attributes
														// ------------------------------------------------------------------
	float vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};

	const std::vector<GLubyte> texBuffer(4 * 1 , 0); //4 because RGBA
	const std::string nm = "tex2D";

	/*
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1, 1); //Specify storage

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &texBuffer[0]); //Set colors for lowest mipmap level (all black)
	checkErrors("start");
	//activate
	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_3D, textureID);

	glUniform1i(glGetUniformLocation(ourShader.ID, nm.c_str()), 0);
	//glBindImageTexture(0, textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA);
	checkErrors("start2");*/
	/*
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texBuffer[0]);
	glUniform1i(glGetUniformLocation(ourShader.ID, nm.c_str()), 0);
	
	*/

	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1, 1);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &texBuffer[0]);
	checkErrors("1");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	checkErrors("2");


	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	// glBindVertexArray(0);


	// render loop
	// -----------
	unsigned int poss = pow(2, maxlit);
	unsigned int dim_x = ceil(sqrt(poss));
	unsigned int dim_y = ceil(sqrt(poss));
	std::cout << dim_x << std::endl;

	//GLint vp;
	//glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &vp);
	//std::cout << vp << std::endl;
	//glViewport(0, 0, dim_x, dim_y);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	// render
	// ------
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// render the triangle
	//set uniforms yo
	ourShader.use();
	checkErrors("asd");

	glBindImageTexture(0, textureID, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
	GLuint location = glGetUniformLocation(ourShader.ID, nm.c_str());
	glUniform1i(location, 0);
	
	ourShader.setRuleSet(&rules[0], &flips[0], rules.size());
	checkErrors("3a");
	ourShader.setInt("rules_size", rules.size());
	ourShader.setInt("dim_x", dim_x);
	checkErrors("3");

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------

	//retrieve data
	
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	GLubyte* putdatahere;

	glBindTexture(GL_TEXTURE_2D, textureID);

	GLint byte_amount = 1 * 4;

	size_t mem_size = byte_amount * sizeof(GLubyte);
	putdatahere = (GLubyte*)malloc(mem_size); //Allocate image data into memory
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); //Explicit Synchronization, as ImageStore is not always memory coherent
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, putdatahere); //Retrieve image data from GPU

	std::cout << std::to_string(putdatahere[0]) << std::endl;

	checkErrors("end");



	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	while (1) {}
	return;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}