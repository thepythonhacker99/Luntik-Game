#pragma once

#include "SFML/Graphics.hpp"

#include "../Utils/vec2.h"
#include "../settings.h"

namespace Luntik::Renderer {
class Camera : public sf::View {
public:
  Camera() { setCenter(0, 0); };

  ~Camera(){

  };

  float getAspectRatio() const { return getSize().x / getSize().y; }
  float getAspectRatio(float x, float y) const { return x / y; }

  Luntik::Utils::vec2 getTopLeft() {
    return Luntik::Utils::vec2(getCenter()) -
           (Luntik::Utils::vec2(getSize()) / 2);
  }

  void setWidthByAspectRatio(float aspect) {
    setSize(Luntik::Settings::HEIGHT * aspect, Luntik::Settings::HEIGHT);
  }

  void moveTowards(Luntik::Utils::vec2 position, float amount) {
    setCenter(getCenter().x + ((position.x - getCenter().x) * amount),
              getCenter().y + ((position.y - getCenter().y) * amount));
  }

private:
};
} // namespace Luntik::Renderer
