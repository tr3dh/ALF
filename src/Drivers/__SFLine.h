#include <SFML/Graphics.hpp>
#include <math.h>

class sfLine : public sf::Drawable
{
public:

    sfLine() = default;

    sfLine(const sf::Vector2f& point1, const sf::Vector2f& point2, const sf::Color& colorIn, const float& thicknesIn):
        color(colorIn), thickness(thicknesIn)
    {
        positionVerticies(point1, point2);
        colorVerticies(color);
    }

    void positionVerticies(const sf::Vector2f& point1, const sf::Vector2f& point2){

        sf::Vector2f direction = point2 - point1;
        sf::Vector2f unitDirection = direction/std::sqrt(direction.x*direction.x+direction.y*direction.y);
        sf::Vector2f unitPerpendicular(-unitDirection.y,unitDirection.x);

        sf::Vector2f offset = (thickness/2.f)*unitPerpendicular;

        vertices[0].position = point1 + offset;
        vertices[1].position = point2 + offset;
        vertices[2].position = point2 - offset;
        vertices[3].position = point1 - offset;
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states = sf::RenderStates()) const
    {
        target.draw(vertices,4,sf::Quads);
    }

    void colorVerticies(const sf::Color& colorIn){
        
        color = colorIn;

        for (int i=0; i<4; ++i)
            vertices[i].color = color;
    }

    void setThickness(const float& thicknessIn){
        thickness = thicknessIn;
    }


private:
    sf::Vertex vertices[4];
    float thickness;
    sf::Color color;
};