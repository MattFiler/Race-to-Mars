#ifndef PROJECT_MENU_H
#define PROJECT_MENU_H

#include "client/KeyHandler/KeyHandler.h"
#include "client/Locator/ServiceLocator.h"
#include "client/UI/ScaledSprite.h"
#include "gamelib/Debug/DebugText.h"
#include "gamelib/FileHandler/FileHandler.h"
#include "gamelib/Localisation/Localisation.h"
#include <Engine/Renderer.h>
#include <soloud.h>
#include <soloud_wav.h>

/*
 *
 * This class forms a dynamic menu system with the a range of simple
 * customisation options available.
 *
 */

class Menu
{
 public:
  Menu();
  ~Menu() = default;

  Menu(const Menu&) = delete;
  Menu& operator=(const Menu&) = delete;

  void addMenuItem(const std::string& item_text);
  void addMenuSprite(const std::string& filepath);

  void setMenuTextSpacing(float offset);
  void setLeftMargin(float offset);

  void setActiveTextColour(ASGE::Colour colour);
  void setInactiveTextColour(ASGE::Colour colour);

  bool selectedItemWas(const std::string& item_text);

  bool itemWasSelected(KeyHandler& user_input);
  void render();

  int getMiddleOfWindow() { return static_cast<int>(middle_of_window); };

 private:
  void realignMenuTextItems();

  /* Dynamic Content */
  std::vector<std::string> menu_text_items;
  std::vector<float> menu_text_offsets;
  std::vector<ScaledSprite> menu_sprites;

  /* Engine Functions */
  FileHandler file_handler;
  ASGE::Renderer* renderer = nullptr;
  DebugText debug_text;
  Localisation localiser;

  /* Index */
  int current_menu_index = 0;

  /* Offsets */
  float middle_of_window = 0;
  float x_offset = 100;
  float y_offset = 50;

  /* Sounds */
  SoLoud::Wav item_select;
  SoLoud::Wav item_activate;

  /* Colours */
  ASGE::Colour active_text_colour = ASGE::COLOURS::WHITE;
  ASGE::Colour inactive_text_colour = ASGE::COLOURS::GREY;
};

#endif // PROJECT_MENU_H