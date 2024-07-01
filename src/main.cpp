/**
This application displays a mesh in wireframe using "Modern" OpenGL 3.0+.
The Mesh3D class now initializes a "vertex array" on the GPU to store the vertices
	and faces of the mesh. To render, the Mesh3D object simply triggers the GPU to draw
	the stored mesh data.
We now transform local space vertices to clip space using uniform matrices in the vertex shader.
	See "simple_perspective.vert" for a vertex shader that uses uniform model, view, and projection
		matrices to transform to clip space.
	See "uniform_color.frag" for a fragment shader that sets a pixel to a uniform parameter.
*/

#include <glad/glad.h>
#include <iostream>
#include <memory>
#include <filesystem>

#include "AssimpImport.h"
#include "Mesh3D.h"
#include "Object3D.h"
#include "ShaderProgram.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>

// You will need to add your own AssimpImport.cpp from HW 4 if you want to load
// other meshes.

struct Scene {
	std::vector<Object3D> objects;
	ShaderProgram program;
};

ShaderProgram textureShader() {
	ShaderProgram shader;
	try {
		shader.load("shaders/texture_perspective.vert", "shaders/texture_inverted.frag");
	}
	catch (std::runtime_error& e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		exit(1);
	}
	return shader;
}

/*
* YOU CAN USE THIS SCENE ONLY AFTER YOU HAVE FINISHED assimpLoad()
*/
Scene bunnyTextured() {
	auto bunny = assimpLoad("models/bunny_textured.obj", true); // will automatically load the associated texture image.
	bunny.move(glm::vec3(0.2, -1, -5));
	bunny.grow(glm::vec3(9, 9, 9));

	return Scene{
		{ bunny },
		textureShader()
	};
}


// A scene of a textured triangle.
Scene triangle() {
	StbImage wall;
	wall.loadFromFile("models/paint.jpg");

	auto triangle = Object3D(std::make_shared<Mesh3D>(Mesh3D::triangle(wall)));
	triangle.move(glm::vec3(0, 0, -2));

	return Scene{
		{triangle},
		textureShader()
	};
}

int main() {
	std::cout << "Current executable path: " << std::endl;
	std::cout << std::filesystem::current_path() << std::endl;
	std::cout << "ALL MODEL AND SHADER PATHS MUST BE RELATIVE TO THIS LOCATION" << std::endl;
	
	// Initialize the window and OpenGL.
	sf::ContextSettings settings;
	settings.depthBits = 24; // Request a 24 bits depth buffer
	settings.stencilBits = 8;  // Request a 8 bits stencil buffer
	settings.antialiasingLevel = 2;  // Request 2 levels of antialiasing
	settings.majorVersion = 3;
	settings.minorVersion = 3;
	sf::Window window(sf::VideoMode{ 1000, 1000 }, "Modern OpenGL", sf::Style::Resize | sf::Style::Close, settings);

	gladLoadGL();
	// Draw in wireframe mode for now.
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	// Inintialize scene objects.
	auto myScene = triangle();
	auto& obj = myScene.objects[0];

	// Activate the shader program.
	myScene.program.activate();
	myScene.program.setUniform("color", glm::vec3(1, 0.2, 0.9));

	// Set up the view and projection matrices.
	glm::mat4 camera = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	glm::mat4 perspective = glm::perspective(glm::radians(45.0), static_cast<double>(window.getSize().x) / window.getSize().y, 0.1, 100.0);
	myScene.program.setUniform("view", camera);
	myScene.program.setUniform("projection", perspective);

	// Ready, set, go!
	bool running = true;
	sf::Clock c;
	auto last = c.getElapsedTime();
	while (running) {
		
		sf::Event ev;
		while (window.pollEvent(ev)) {
			if (ev.type == sf::Event::Closed) {
				running = false;
			}
		}
		auto now = c.getElapsedTime();
		auto diff = now - last;
		//std::cout << 1 / diff.asSeconds() << " FPS " << std::endl;
		last = now;

		// Update the scene.
		// obj.rotate(glm::vec3(0, 0.0002, 0));

		// Clear the OpenGL "context".
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (auto& o : myScene.objects) {
			o.render(myScene.program);
		}
		window.display();


		
	}

	return 0;
}


