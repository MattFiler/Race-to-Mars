#include "PopupWindow.h"
#include "client/Core/ServiceLocator.h"
#include "gamelib/Constants.h"

/* Create the popup */
PopupWindow::PopupWindow()
{
  renderer = Locator::getRenderer();
  sprite = new ScaledSprite("data/UI/INGAME_UI/popup_blank.png");
}

/* Destroy the popup and all contents */
PopupWindow::~PopupWindow()
{
  delete sprite;
  sprite = nullptr;

  popup_sprites.clear();
}

/* Handle key inputs */
void PopupWindow::keyHandler(KeyHandler keys)
{
  if (keys.keyReleased("Back") && timeout == -1 && is_active)
  {
    hide();
  }
}

/* Handle mouse inputs */
void PopupWindow::clickHandler(Vector2 mouse_pos)
{
  // CLICK BUTTONS HERE?!
}

/* Create a sprite for the popup */
ScaledSprite& PopupWindow::createSprite(const std::string& sprite_path)
{
  ScaledSprite* new_sprite = new ScaledSprite(sprite_path);
  popup_sprites.push_back(new_sprite);
  return *new_sprite;
}

/* Add a reference of an existing sprite for the popup */
ScaledSprite& PopupWindow::referenceSprite(ScaledSprite& ref_sprite)
{
  popup_sprites_referenced.push_back(&ref_sprite);
  return ref_sprite;
}

/* Show for a period of time */
void PopupWindow::showForTime(float _timeout)
{
  timeout = static_cast<double>(_timeout);
  is_active = true;
}

/* Update the popup */
void PopupWindow::update(const ASGE::GameTime& game_time)
{
  if (!is_active || timeout == -1)
  {
    return;
  }

  timer += game_time.delta.count() / 1000;
  if (timer >= timeout)
  {
    is_active = false;
    timer = 0;
  }
}

/* Render the popup and all contents */
void PopupWindow::render()
{
  if (!is_active)
  {
    return;
  }

  renderer->renderSprite(*sprite->getSprite(), render_order::PRIORITY_UI_3);

  for (ScaledSprite* content : popup_sprites)
  {
    renderer->renderSprite(*content->getSprite(), render_order::PRIORITY_UI_4);
  }
  for (ScaledSprite* content : popup_sprites_referenced)
  {
    renderer->renderSprite(*content->getSprite(), render_order::PRIORITY_UI_4);
  }
}