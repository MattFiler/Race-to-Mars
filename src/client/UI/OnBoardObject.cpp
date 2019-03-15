#include "OnBoardObject.h"
#include "gamelib/Constants.h"

OnBoardObject::OnBoardObject()
{
  renderer = Locator::getRenderer();
}

/* Set sprite texture for counter */
void OnBoardObject::setSprite(const std::string& sprite_path)
{
  sprite = new ScaledSprite(sprite_path);
  setDimensions(Vector2(sprite->width(), sprite->height()));
}

/* Set position of the counter */
void OnBoardObject::setPosition(Vector2 _position)
{
  position.update(_position.x, _position.y);
  sprite->xPos(position.x);
  sprite->yPos(position.y);
}

/* Set dimensions of the counter */
void OnBoardObject::setDimensions(Vector2 _dims)
{
  dimensions.update(_dims.x, _dims.y);
  sprite->width(_dims.x);
  sprite->height(_dims.y);
}

/* Render the counter's sprite */
void OnBoardObject::render()
{
  renderer->renderSprite(*sprite->getSprite());
}