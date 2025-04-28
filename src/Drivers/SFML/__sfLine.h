#pragma once

#include <SFML/Graphics.hpp>
#include <math.h>

class sfLine : public sf::Drawable
{
public:

    sfLine();

    sfLine(const sf::Vector2f& point1, const sf::Vector2f& point2, const sf::Color& colorIn, const float& thicknesIn);

    void positionVerticies(const sf::Vector2f& point1, const sf::Vector2f& point2);

    void draw(sf::RenderTarget &target, sf::RenderStates states = sf::RenderStates()) const;

    void colorVerticies(const sf::Color& colorIn);

    void setThickness(const float& thicknessIn);

private:

    sf::Vertex vertices[4];
    float thickness;
    sf::Color color;
};