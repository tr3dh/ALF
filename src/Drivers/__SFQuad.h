#pragma once

#include <SFML/Graphics.hpp>
#include <math.h>

class sfQuad : public sf::Drawable
{
public:

    sfQuad() = default;

    void positionVerticies(const std::vector<sf::Vector2f>& points){

        vertices[0].position = points[0];
        vertices[1].position = points[1];
        vertices[2].position = points[2];
        vertices[3].position = points[3];
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

private:
    sf::Vertex vertices[4];
    sf::Color color;
};