#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

// Für Vector2...
template<typename T>
std::ostream& operator<<(std::ostream& os, const sf::Vector2<T>& v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}