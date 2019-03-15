#include "Menu.h"
#include "gamelib/Constants.h"

/* Grab window scale */
Menu::Menu()
{
  // middle_of_window = (SCREEN_HEIGHT * ScaledSpriteArray::width_scale / 2) +
  // (15 * ScaledSpriteArray::width_scale);
  middle_of_window = static_cast<float>((GameResolution::height / 2) + 15);
  // x_offset *= ScaledSpriteArray::width_scale;
  // y_offset *= ScaledSpriteArray::width_scale;

  sound_player = Locator::getAudio();
  renderer = Locator::getRenderer();

  file_handler.loadSound(item_select, "CLICK_03", 0.7f);
  file_handler.loadSound(item_index_change, "CLICK_04", 0.5f);
}

/* Add menu text item */
void Menu::addMenuItem(const std::string& item_text)
{
  menu_text_items.push_back(localiser.getString(item_text));
  menu_text_offsets.push_back(0);

  realignMenuTextItems();
}

/* Add menu sprite item */
ASGE::Sprite* Menu::addMenuSprite(const std::string& filepath)
{
  // Create sprite
  ASGE::Sprite* new_sprite = renderer->createRawSprite();
  new_sprite->loadTexture("data/UI/" + filepath);

  return new_sprite;
}

/* Set vertical spacing between menu text items */
void Menu::setMenuTextSpacing(float offset)
{
  // y_offset = offset * ScaledSpriteArray::width_scale;
  y_offset = offset;
  realignMenuTextItems();
}

/* Set the offset to the left of the screen */
void Menu::setLeftMargin(float offset)
{
  // x_offset = offset * ScaledSpriteArray::width_scale;
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
    sound_player->play(item_select);
    return true;
  }
  else if (user_input.keyReleased("Menu Up") && (current_menu_index > 0))
  {
    sound_player->play(item_index_change);
    current_menu_index--;
  }
  else if (user_input.keyReleased("Menu Down") &&
           (current_menu_index < static_cast<int>(menu_text_items.size() - 1)))
  {
    sound_player->play(item_index_change);
    current_menu_index++;
  }
  return false;
}

/* Render */
void Menu::render(double delta_time)
{
  // Render sprites
  for (ASGE::Sprite* item_sprite : menu_sprites)
  {
    renderer->renderSprite(*item_sprite);
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