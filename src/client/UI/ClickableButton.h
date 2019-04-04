#ifndef PROJECT_CLICKABLEBUTTON_H
#define PROJECT_CLICKABLEBUTTON_H

#include "client/UI/ScaledSprite.h"
#include <Engine/Renderer.h>

class ClickableButton
{
 public:
  explicit ClickableButton(const std::string& sprite_path);
  ~ClickableButton() = default;

  void setActive(bool _active) { is_active = _active; };
  bool isActive() { return is_active; };

  void setPos(Vector2 _pos);
  Vector2 getPos();

  bool clicked();

  void render();

 private:
  ASGE::Renderer* renderer = nullptr;
  ScaledSprite* sprite = nullptr;
  bool is_active = false;
};

#endif // PROJECT_CLICKABLEBUTTON_H
