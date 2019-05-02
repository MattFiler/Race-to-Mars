#ifndef PROJECT_POPUPWINDOW_H
#define PROJECT_POPUPWINDOW_H

#include "client/KeyHandler/KeyHandler.h"
#include "client/UI/ClickableButton.h"
#include "client/UI/ScaledSprite.h"
#include "gamelib/FileHandler/FileHandler.h"
#include <Engine/GameTime.h>
#include <Engine/Renderer.h>
#include <soloud.h>
#include <soloud_wav.h>
#include <vector>

class PopupWindow
{
 public:
  PopupWindow();
  ~PopupWindow();

  PopupWindow(const PopupWindow&) = delete;
  PopupWindow& operator=(const PopupWindow&) = delete;

  void keyHandler(KeyHandler& keys);
  void clickHandler();

  ScaledSprite& createSprite(const std::string& sprite_path);
  void clearAllInternalSprites() { popup_sprites.clear(); };

  ScaledSprite& referenceSprite(ScaledSprite& ref_sprite);
  void clearAllReferencedSprites() { popup_sprites_referenced.resize(0); };

  ClickableButton& createButton(const std::string& sprite_path);
  void clearAllInternalButtons() { popup_buttons.clear(); };

  ClickableButton& referenceButton(ClickableButton& ref_button);
  void clearAllReferencedButtons() { popup_buttons_referenced.resize(0); };

  void positionCloseButton(Vector2 _pos);
  Vector2 getCloseButtonPos();

  /* EXTERNALLY USING BUTTONS IS DANGEROUS AS WE MANAGE THEM INTERNALLY! */
  std::vector<ClickableButton*> getInternalButtons() { return popup_buttons; };
  std::vector<ClickableButton*> getReferencedButtons()
  {
    return popup_buttons_referenced;
  };

  /* EXTERNALLY USING SPRITES IS DANGEROUS AS WE MANAGE THEM INTERNALLY! */
  std::vector<ScaledSprite*> getInternalSprites() { return popup_sprites; };
  std::vector<ScaledSprite*> getReferencedSprites()
  {
    return popup_sprites_referenced;
  };

  void renderTextAtPosition(const std::string& text, Vector2 position);
  void clearAllRenderText();

  bool isVisible() { return is_active; };

  // Automatic show/hide over time
  void showForTime(float _timeout);

  // Manual show/hide
  void show();
  void hide();

  void update(const ASGE::GameTime& game_time);
  void render();

 private:
  /* Locator */
  ASGE::Renderer* renderer = nullptr;
  ScaledSprite* sprite = nullptr;
  bool is_active = false;
  double timeout = 0;
  double timer = 0;

  /* Popup Content */
  std::vector<ScaledSprite*> popup_sprites;
  std::vector<ClickableButton*> popup_buttons;
  std::vector<ScaledSprite*> popup_sprites_referenced;
  std::vector<ClickableButton*> popup_buttons_referenced;
  ClickableButton* close_button;
  std::vector<std::string> popup_text;
  std::vector<Vector2> popup_text_pos;

  /* Sound-related */
  FileHandler file_handler;
  SoLoud::Wav active_sfx;
  SoLoud::Wav inactive_sfx;
};

#endif // PROJECT_POPUPWINDOW_H
