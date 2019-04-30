#include "Menu.h"
#include "gamelib/Constants.h"

/* Grab window scale */
Menu::Menu()
{
  middle_of_window = (static_cast<float>(GameResolution::height) / 2) + 15;

  renderer = Locator::getRenderer();

  file_handler.loadSound(item_select, "menu_option_changed", 1.0f);
  file_handler.loadSound(item_activate, "menu_option_selected", 1.0f);

  y_offset *= GameResolution::scale;
  x_offset *= GameResolution::scale;
}

/* Add menu text item */
void Menu::addMenuItem(const std::string& item_text)
{
  menu_text_items.push_back(localiser.getString(item_text));
  menu_text_offsets.push_back(0);
  realignMenuTextItems();
}

/* Add menu sprite item */
void Menu::addMenuSprite(const std::string& filepath)
{
  // Create sprite
  menu_sprites.emplace_back("data/UI/" + filepath);
}

/* Set vertical spacing between menu text items */
void Menu::setMenuTextSpacing(float offset)
{
  y_offset = offset;
  realignMenuTextItems();
}

/* Set the offset to the left of the screen */
void Menu::setLeftMargin(float offset)
{
  x_offset = offset;
}

/* Set text active colour */
void Menu::setActiveTextColour(ASGE::Colour colour)
{
  active_text_colour = colour;
}

/* Set text inactive colour */
void Menu::setInactiveTextColour(ASGE::Colour colour)
{
  inactive_text_colour = colour;
}

/* Realign the menu text items */
void Menu::realignMenuTextItems()
{
  float vertical_correction =
    ((static_cast<float>(menu_text_items.size()) - 1) * y_offset) / 2;
  for (int i = 0; i < static_cast<int>(menu_text_items.size()); i++)
  {
    menu_text_offsets[i] = (static_cast<float>(i) * y_offset) -
                           vertical_correction + middle_of_window;
  }
}

/* Checks to see if the given menu item was selected (identified by its text) */
bool Menu::selectedItemWas(const std::string& item_text)
{
  if (menu_text_items[current_menu_index] == localiser.getString(item_text))
  {
    return true;
  }
  return false;
}

/* Key handler - returns true if the active option was selected */
bool Menu::itemWasSelected(KeyHandler& user_input)
{
  if (user_input.keyReleased("Activate"))
  {
    Locator::getAudio()->play(item_activate);
    return true;
  }
  else if (user_input.keyReleased("Menu Up") && (current_menu_index > 0))
  {
    Locator::getAudio()->play(item_select);
    current_menu_index--;
  }
  else if (user_input.keyReleased("Menu Down") &&
           (current_menu_index < static_cast<int>(menu_text_items.size() - 1)))
  {
    Locator::getAudio()->play(item_select);
    current_menu_index++;
  }
  return false;
}

/* Render */
void Menu::render()
{
  // Render sprites
  for (ScaledSprite& item_sprite : menu_sprites)
  {
    renderer->renderSprite(*item_sprite.getSprite());
  }

  // Render text
  int index = 0;
  for (std::string& item_text : menu_text_items)
  {
    renderer->renderText(item_text,
                         static_cast<int>(x_offset),
                         static_cast<int>(menu_text_offsets[index]),
                         1,
                         (current_menu_index == index) ? active_text_colour
                                                       : inactive_text_colour);
    index++;
  }
}