
#include <SFML\Window.hpp>
#include <SFML\System.hpp>
#include <SFML\System\Clock.hpp>
#include <GLEW\GL\glew.h>
#include <GLM\gtc\type_ptr.hpp>
#include <GLM\gtc\matrix_transform.hpp>
#include <GLM\gtc\quaternion.hpp>
#include <GLM\gtc\random.hpp>
#include <iostream>
#include <ctime>

#include "RenderShape.h"
#include "Init_Shader.h"
#include "RenderManager.h"
#include "InputManager.h"
#include "KDTreeManager.h"

sf::Window window;
sf::Clock timer = sf::Clock();

GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;

GLuint vbo;
GLuint vao0;
GLuint vao1;
GLuint ebo0;
GLuint ebo1;
GLint posAttrib;
GLint uTransform;
GLint uColor;

GLfloat vertices[] = {
	-1.0f, +1.0f,
	+1.0f, +1.0f,
	-1.0f, -1.0f,
	+1.0f, -1.0f
};

GLint elements[] = {
	0, 1, 2,
	1, 3, 2
};

GLint outlineElements[] = {
	1, 2 
};

void initShaders()
{
	char* shaders[] = { "fshader.glsl", "vshader.glsl" };
	GLenum types[] = { GL_FRAGMENT_SHADER, GL_VERTEX_SHADER };
	int numShaders = 2;
	
	shaderProgram = initShaders(shaders, types, numShaders);
	
	// Bind buffer data to shader values
	posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

	uTransform = glGetUniformLocation(shaderProgram, "transform");
	uColor = glGetUniformLocation(shaderProgram, "color");
}

void init()
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 2;

	window.create(sf::VideoMode(800, 600), "K-D_Tree", sf::Style::Close, settings);

	glewExperimental = true;
	glewInit();

	// Store the data for the triangles in a buffer that the gpu can use to draw
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao0);
	glBindVertexArray(vao0);

	glGenBuffers(1, &ebo0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo0);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	// Compile shaders
	initShaders();

	glGenVertexArrays(1, &vao1);
	glBindVertexArray(vao1);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(outlineElements), outlineElements, GL_STATIC_DRAW);

	initShaders();

	timer.restart();

	Shader shader;
	shader.shaderPointer = shaderProgram;
	shader.uTransform = uTransform;
	shader.uColor = uColor;

	time_t timer;
	time(&timer);
	srand((unsigned int)timer);

	RenderManager::GenerateShapes(shader, vao0, 500, GL_TRIANGLES, 6);

	InputManager::Init(&window);

	KDTreeManager::InitKDTree(5, RenderShape(vao1, 2, GL_LINE_STRIP, shader, glm::vec4(0.0f, 1.0f, 0.3f, 1.0f)));
	
	unsigned int shapesSize = RenderManager::interactiveShapes().size();
	for (unsigned int i = 0; i < shapesSize; ++i)
	{
		KDTreeManager::AddShape(RenderManager::interactiveShapes()[i]);
	}
	KDTreeManager::UpdateKDtree();
}

void step()
{
	// Clear to black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	InputManager::Update();

	// Get delta time since the last frame
	float dt = timer.getElapsedTime().asSeconds();
	timer.restart();

	// Change the max depth of the tree if the up or down arrow keys are pressed
	int dDepth = 0;
	dDepth += (InputManager::upKey(true) && !InputManager::upKey());
	dDepth -= (InputManager::downKey(true) && !InputManager::downKey());
	KDTreeManager::SetMaxDepth(KDTreeManager::maxDepth() + dDepth);

	RenderManager::Update(dt);
	if (RenderManager::shapeMoved())
		KDTreeManager::UpdateKDtree();

	RenderManager::Draw();

	// Swap buffers
	window.display();
}

void pollEvents()
{
	sf::Event windowEvent;
	while (window.pollEvent(windowEvent))
	{
		switch (windowEvent.type)
		{
		case sf::Event::Closed:
			window.close();
			break;
		}
	}
}

void cleanUp()
{
	glDeleteProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo0);
	glDeleteBuffers(1, &ebo1);
	glDeleteBuffers(1, &vao0);
	glDeleteBuffers(1, &vao1);

	RenderManager::DumpData();

	KDTreeManager::DumpData();
}

int main()
{
	init();

	while (window.isOpen())
	{
		step();
		pollEvents();
	}

	cleanUp();

	return 0;
}
