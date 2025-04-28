#pragma once

#include <SFML/Graphics.hpp>
#include "Drivers/ReccHandling/__Asserts.h"

class sfPolygon {

public:

    sfPolygon();

    void setFillColor(const sf::Color& color);

    void setPointCount(const int& pointCount);

    void setPoint(const int& index, const sf::Vector2f& point);

    void setPoints(const std::vector<sf::Vector2f>& points);

    void draw(sf::RenderWindow& win);

private:

    //
    sf::ConvexShape m_polygon;
};