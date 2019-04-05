#include "GameSceneUI.h"
#include "client/Core/ServiceLocator.h"

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
ScaledSprite*
SceneUI::createSprite(const std::string& id, const std::string& path)
{
  ScaledSprite* new_sprite = new ScaledSprite(path);
  sprites.push_back(new_sprite);
  sprite_ids.push_back(id);
  return new_sprite;
}

/* Get a sprite on the UI by ID */
ScaledSprite* SceneUI::getSprite(const std::string& id)
{
  int index = 0;
  for (std::string& identifier : sprite_ids)
  {
    if (identifier == id)
    {
      return sprites.at(index);
    }
    index++;
  }
  throw "Sprite ID did not exist.";
}

/* Update UI */
void SceneUI::update(const ASGE::GameTime& game_time)
{
  popups().update(game_time);
}

/* Render UI */
void SceneUI::render()
{
  for (ScaledSprite* this_sprite : sprites)
  {
    renderer->renderSprite(*this_sprite->getSprite());
  }
  popups().render();
}