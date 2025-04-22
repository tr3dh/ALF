#pragma once

#include <SFML/Graphics.hpp>
#include "__Asserts.h"

class sfPolygon {

public:

    sfPolygon() = default;

    void setFillColor(const sf::Color& color){

        m_polygon.setFillColor(color);
    }

    void setPointCount(const int& pointCount){
        
        m_polygon.setPointCount(pointCount);
    }

    void setPoint(const int& index, const sf::Vector2f& point){

        m_polygon.setPoint(index, point);
    }

    void setPoints(const std::vector<sf::Vector2f>& points){

        ASSERT(points.size() == m_polygon.getPointCount(), "Ungültige Anzahl an Punkten an Polygon übergeben");

        for(int index = 0; index < points.size(); index++){
            m_polygon.setPoint(index, points[index]);
        }

    }

    void draw(sf::RenderWindow& win){
        win.draw(m_polygon);
    }

private:

    // create an empty shape
    sf::ConvexShape m_polygon;
};