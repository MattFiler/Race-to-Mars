#ifndef PROJECT_MENU_H
#define PROJECT_MENU_H

#include "../Core/ServiceLocator.h"
#include "gamelib/Debug/DebugText.h"
#include "../FileHandler/FileHandler.h"
#include "../KeyHandler/KeyHandler.h"
#include "../Localisation/Localisation.h"
#include <Engine/Renderer.h>
#include <Engine/Sprite.h>

class Menu
{
 public:
  Menu();
  ~Menu() = default;

  Menu(const Menu&) = delete;
  Menu& operator=(const Menu&) = delete;

  void addMenuItem(const std::string& item_text);
  ASGE::Sprite* addMenuSprite(const std::string& filepath);

  void setMenuTextSpacing(float offset);
  void setLeftMargin(float offset);

  void setActiveTextColour(ASGE::Colour colour);
  void setInactiveTextColour(ASGE::Colour colour);

  bool selectedItemWas(const std::string& item_text);

  bool itemWasSelected(KeyHandler& user_input);
  void render(double delta_time);

  int getMiddleOfWindow() { return static_cast<int>(middle_of_window); };

 private:
  void realignMenuTextItems();

  std::vector<std::string> menu_text_items;
  std::vector<float> menu_text_offsets;
  std::vector<ASGE::Sprite*> menu_sprites;

  FileHandler file_handler;
  ASGE::Renderer* renderer = nullptr;
  DebugText debug_text;
  Localisation localiser;

  int current_menu_index = 0;

  float middle_of_window = 0;
  float x_offset = 100;
  float y_offset = 50;

  SoLoud::Soloud* sound_player;
  SoLoud::Wav item_select;
  SoLoud::Wav item_index_change;

  ASGE::Colour active_text_colour = ASGE::COLOURS::WHITE;
  ASGE::Colour inactive_text_colour = ASGE::COLOURS::GREY;
};

#endif // PROJECT_MENU_H