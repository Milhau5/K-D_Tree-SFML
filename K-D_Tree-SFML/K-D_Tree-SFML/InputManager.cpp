#include "InputManager.h"
#include <SFML\Window.hpp>

sf::Vector2i InputManager::_mousePos = sf::Vector2i();
bool InputManager::_leftMouseButton = false;
bool InputManager::_prevLeftMouseButton = false;
bool InputManager::_upKey = false;
bool InputManager::_prevUpKey = false;
bool InputManager::_downKey = false;
bool InputManager::_prevDownKey = false;
sf::Window* InputManager::_window;
float InputManager::_aspectRatio = 0.0f;
sf::Vector2u InputManager::_windowSize = sf::Vector2u();

void InputManager::Init(sf::Window* window)
{
	_window = window;
	_windowSize = _window->getSize();
	_aspectRatio = (float)_windowSize.x / (float)_windowSize.y;
}

void InputManager::Update()
{
	_prevLeftMouseButton = _leftMouseButton;
	_leftMouseButton = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
	_mousePos = sf::Mouse::getPosition(*_window);
	_prevDownKey = _downKey;
	_downKey = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
	_prevUpKey = _upKey;
	_upKey = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
}

glm::vec2 InputManager::GetMouseCoords()
{
	glm::vec2 ret = glm::vec2();
	ret.x = (((float)_mousePos.x / (float)_windowSize.x) * 2.0f - 1.0f) * _aspectRatio;
	ret.y = -(((float)_mousePos.y / (float)_windowSize.y) * 2.0f - 1.0f);
	return ret;
}
bool InputManager::leftMouseButton(bool prev) { if (prev) return _prevLeftMouseButton; else return _leftMouseButton; }
bool InputManager::upKey(bool prev) { if (prev) return _prevUpKey; else return _upKey; }
bool InputManager::downKey(bool prev) { if (prev) return _prevDownKey; else return _downKey; }
