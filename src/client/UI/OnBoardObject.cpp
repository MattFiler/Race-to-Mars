#include "OnBoardObject.h"
#include "gamelib/Constants.h"

#include "gamelib/Debug/DebugText.h"

OnBoardObject::OnBoardObject()
{
  renderer = Locator::getRenderer();
}

/* Set sprite texture for counter */
void OnBoardObject::setSprite(const std::string& sprite_path)
{
  sprite = new ScaledSprite(sprite_path);
  setDimensions(Vector2(sprite->width(), sprite->height()), true);
  setPosition(Vector2(0, 0), true);
}

/* Set sprite texture for the shadow (optional) */
void OnBoardObject::setShadowSprite(const std::string& sprite_path)
{
  sprite_shadow = new ScaledSprite(sprite_path);
  has_shadow = true;
}

/* Set position of the counter */
void OnBoardObject::setPosition(Vector2 _position, bool force)
{
  if (position == _position && !force)
  {
    return;
  }

  position.update(_position.x, _position.y);
  sprite->xPos(position.x);
  sprite->yPos(position.y);
  if (has_shadow)
  {
    // potentially expand shadow support for other offsets & dimensions?
    sprite_shadow->xPos(position.x -
                        (card_offsets.shadow_offset.x * shadow_scalar));
    sprite_shadow->yPos(position.y -
                        (card_offsets.shadow_offset.y * shadow_scalar));
  }
}

/* Set dimensions of the counter */
void OnBoardObject::setDimensions(Vector2 _dims, bool force)
{
  if (dimensions == _dims && !force)
  {
    return;
  }

  shadow_scalar = _dims.x / dimensions.x;
  DebugText text;
  text.print(std::to_string(shadow_scalar));
  text.print(std::to_string(_dims.x));
  text.print(std::to_string(_dims.y));
  dimensions.update(_dims.x, _dims.y);
  sprite->width(_dims.x);
  sprite->height(_dims.y);
  if (has_shadow)
  {
    sprite_shadow->width(_dims.x +
                         (card_offsets.shadow_offset.x * shadow_scalar * 2));
    sprite_shadow->height(_dims.y +
                          (card_offsets.shadow_offset.y * shadow_scalar * 2));
  }
}

/* Is the position within our bounding box? */
bool OnBoardObject::isInBoundingBox(Vector2 _pos)
{
  return sprite->getBoundingBox().isInside(_pos);
}

/* Render the counter's sprite */
void OnBoardObject::render(render_order _order)
{
  renderer->renderSprite(*sprite->getSprite(), _order);
  if (has_shadow)
  {
    // shadow render order should always be one below current
    renderer->renderSprite(*sprite_shadow->getSprite(),
                           static_cast<render_order>(_order - 1));
  }
}