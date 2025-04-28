#include "__sfPolygon.h"

sfPolygon::sfPolygon() = default;

void sfPolygon::setFillColor(const sf::Color& color){

    m_polygon.setFillColor(color);
}

void sfPolygon::setPointCount(const int& pointCount){
    
    m_polygon.setPointCount(pointCount);
}

void sfPolygon::setPoint(const int& index, const sf::Vector2f& point){

    m_polygon.setPoint(index, point);
}

void sfPolygon::setPoints(const std::vector<sf::Vector2f>& points){

    ASSERT(points.size() == m_polygon.getPointCount(), "Ungültige Anzahl an Punkten an Polygon übergeben");

    for(size_t index = 0; index < points.size(); index++){
        m_polygon.setPoint(index, points[index]);
    }

}

void sfPolygon::draw(sf::RenderWindow& win){

    win.draw(m_polygon);
}
    