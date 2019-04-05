#include "GameSceneUI.h"
#include "client/Locator/ServiceLocator.h"

/* Create */
SceneUI::SceneUI()
{
  renderer = Locator::getRenderer();
}

/* Destroy */
SceneUI::~SceneUI()
{
  sprites.clear();
  sprite_ids.clear();
}

/* Create a sprite on the UI with ID */
ScaledSprite* SceneUI::createSprite(int id, const std::string& path)
{
  ScaledSprite* new_sprite = new ScaledSprite(path);
  sprites.push_back(new_sprite);
  sprite_ids.push_back(id);
  return new_sprite;
}

/* Get a sprite on the UI by ID */
ScaledSprite* SceneUI::getSprite(int id)
{
  int index = 0;
  for (int identifier : sprite_ids)
  {
    if (identifier == id)
    {
      return sprites.at(index);
    }
    index++;
  }
  throw "Sprite ID did not exist.";
}

/* Create a button on the UI with ID */
ClickableButton* SceneUI::createButton(int id, const std::string& path)
{
  ClickableButton* new_button = new ClickableButton(path);
  buttons.push_back(new_button);
  button_ids.push_back(id);
  return new_button;
}

/* Get button from the UI by ID */
ClickableButton* SceneUI::getButton(int id)
{
  int index = 0;
  for (int identifier : button_ids)
  {
    if (identifier == id)
    {
      return buttons.at(index);
    }
    index++;
  }
  throw "Button ID did not exist.";
}

/* Create a menu */
Menu* SceneUI::createMenu()
{
  if (has_menu)
  {
    return menu;
  }
  menu = new Menu();
  has_menu = true;
  return menu;
}

/* Get the menu if it exists */
Menu* SceneUI::getMenu()
{
  if (has_menu)
  {
    return menu;
  }
  return nullptr;
}

/* Update UI */
void SceneUI::update(const ASGE::GameTime& game_time)
{
  for (ClickableButton* this_button : buttons)
  {
    this_button->update();
  }
  popups().update(game_time);
}

/* Render UI */
void SceneUI::render()
{
  for (ScaledSprite* this_sprite : sprites)
  {
    renderer->renderSprite(*this_sprite->getSprite());
  }
  for (ClickableButton* this_button : buttons)
  {
    this_button->render();
  }
  popups().render();
}