#include "ClickableButton.h"
#include "client/Core/ServiceLocator.h"
#include "client/UI/Cursor.h"

/* Create button */
ClickableButton::ClickableButton(const std::string& sprite_path)
{
  renderer = Locator::getRenderer();
  sprite = new ScaledSprite(sprite_path);
}

/* Clicked inside the button */
bool ClickableButton::update()
{
  sprite->getSprite()->opacity(0.5f);
  if (is_active)
  {
    Vector2 _pos = Vector2(Locator::getCursor()->getPosition().x,
                           Locator::getCursor()->getPosition().y);
    if (sprite->getBoundingBox().isInside(_pos))
    {
      Locator::getCursor()->setCursorActive(true);
      sprite->getSprite()->opacity(1.0f);
      return true;
    }
    return false;
  }
  else
  {
    return false;
  }
}

/* Set position of button */
void ClickableButton::setPos(Vector2 _pos)
{
  return sprite->setPos(_pos);
}

/* Get position of button */
Vector2 ClickableButton::getPos()
{
  return sprite->getPos();
}

/* Render button */
void ClickableButton::render()
{
  renderer->renderSprite(*sprite->getSprite());
}

/*
if (current_state == game_state::PLAYING)
{
    game_sprites.roll_dice->getSprite()->colour(ASGE::COLOURS::GREY);
}
else if (current_state == game_state::IS_ROLLING_DICE)
{
    game_sprites.roll_dice->getSprite()->colour();
}
 */