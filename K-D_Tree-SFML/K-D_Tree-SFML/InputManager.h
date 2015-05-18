#pragma once
#include <SFML\Window\Mouse.hpp>
#include <GLM\glm.hpp>

class InputManager
{
public:
	static void Init(sf::Window* window);

	static void Update();

	static glm::vec2 GetMouseCoords();
	static bool leftMouseButton(bool prev = false);
	static bool downKey(bool prev = false);
	static bool upKey(bool prev = false);

private:

	static sf::Vector2<int> _mousePos;
	static bool _leftMouseButton;
	static bool _prevLeftMouseButton;
	static bool _upKey;
	static bool _prevUpKey;
	static bool _downKey;
	static bool _prevDownKey;
	static sf::Window* _window;
	static float _aspectRatio;
	static sf::Vector2<unsigned int> _windowSize;
};
