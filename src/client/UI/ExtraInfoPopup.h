#ifndef PROJECT_EXTRAINFOPOPUP_H
#define PROJECT_EXTRAINFOPOPUP_H

#include "client/KeyHandler/KeyHandler.h"
#include "client/UI/ClickableButton.h"
#include "client/UI/ScaledSprite.h"
#include "gamelib/FileHandler/FileHandler.h"
#include <Engine/GameTime.h>
#include <Engine/Renderer.h>
#include <soloud.h>
#include <soloud_wav.h>
#include <vector>

class ExtraInfoPopup
{
 public:
  ExtraInfoPopup();
  ~ExtraInfoPopup();

  ExtraInfoPopup(const ExtraInfoPopup&) = delete;
  ExtraInfoPopup& operator=(const ExtraInfoPopup&) = delete;

  ScaledSprite& setSprite(const std::string& popup_name);

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

  /* Sound-related */
  FileHandler file_handler;
  SoLoud::Wav active_sfx;
  SoLoud::Wav inactive_sfx;
};

#endif // PROJECT_EXTRAINFOPOPUP_H
