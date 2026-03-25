#include "Road.h"

Road::Road(sf::Texture& tex) : texture(tex), va(sf::PrimitiveType::Triangles) {
    texture.setRepeated(true);
}

void Road::update(float speed) {
    offset += speed * 0.7f;
    if (offset > 1.0f) offset -= 1.0f;
    va.clear();
    float startY = HORIZON_Y + 100.f;
    float h = WINDOW_HEIGHT - startY;

    for (int i = 0; i < 40; i++) {
        float p1 = (float)i / 40.f;
        float p2 = (float)(i + 1) / 40.f;
        float w1 = 0.15f + (p1 * 0.85f);
        float w2 = 0.15f + (p2 * 0.85f);
        float y1 = startY + p1 * h;
        float y2 = startY + p2 * h;
        float x1_l = 500.f - (w1 * 500.f);
        float x1_r = 500.f + (w1 * 500.f);
        float x2_l = 500.f - (w2 * 500.f);
        float x2_r = 500.f + (w2 * 500.f);
        float tx = (float)texture.getSize().x;
        float ty = (float)texture.getSize().y;
        float v1 = (p1 - offset) * ty * 2.0f;
        float v2 = (p2 - offset) * ty * 2.0f;

        va.append(sf::Vertex({x1_l, y1}, sf::Color::White, {0.f, v1}));
        va.append(sf::Vertex({x1_r, y1}, sf::Color::White, {tx, v1}));
        va.append(sf::Vertex({x2_l, y2}, sf::Color::White, {0.f, v2}));
        va.append(sf::Vertex({x1_r, y1}, sf::Color::White, {tx, v1}));
        va.append(sf::Vertex({x2_r, y2}, sf::Color::White, {tx, v2}));
        va.append(sf::Vertex({x2_l, y2}, sf::Color::White, {0.f, v2}));
    }
}

void Road::draw(sf::RenderWindow& window) {
    sf::RenderStates states;
    states.texture = &texture;
    window.draw(va, states);
}