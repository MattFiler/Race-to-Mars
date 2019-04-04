#ifndef PROJECT_POPUPWINDOW_H
#define PROJECT_POPUPWINDOW_H

#include "client/KeyHandler/KeyHandler.h"
#include "client/UI/ScaledSprite.h"
#include <Engine/GameTime.h>
#include <Engine/Renderer.h>
#include <vector>

class PopupWindow
{
 public:
  PopupWindow();
  ~PopupWindow();

  void keyHandler(KeyHandler keys);

  ScaledSprite& createSprite(const std::string& sprite_path);
  void clearAllInternalSprites() { popup_sprites.clear(); };

  ScaledSprite& referenceSprite(ScaledSprite& ref_sprite);
  void clearAllReferencedSprites() { popup_sprites_referenced.resize(0); };

  bool isVisible() { return is_active; };

  // Automatic show/hide over time
  void showForTime(float _timeout);

  // Manual show/hide
  void show()
  {
    is_active = true;
    timeout = -1;
  };
  void hide()
  {
    is_active = false;
    timeout = -1;
  };

  void update(const ASGE::GameTime& game_time);
  void render();

 private:
  /* Core */
  ASGE::Renderer* renderer = nullptr;
  ScaledSprite* sprite = nullptr;
  bool is_active = false;
  double timeout = 0;
  double timer = 0;

  /* Popup Content */
  std::vector<ScaledSprite*> popup_sprites;
  std::vector<ScaledSprite*> popup_sprites_referenced;
};

#endif // PROJECT_POPUPWINDOW_H
