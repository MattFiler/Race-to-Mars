#include "Counter.h"

Counter::Counter()
{
  renderer = Locator::getRenderer();
  sprite = renderer->createRawSprite();
}

/* Set sprite texture for counter */
void Counter::setSprite(const std::string& sprite_path)
{
  sprite->loadTexture(sprite_path);
}

/* Set position of the counter */
void Counter::setPosition(Vector2 _position)
{
  position = _position;
  sprite->xPos(position.x);
  sprite->yPos(position.y);
}

/* Render the counter's sprite */
void Counter::render()
{
  renderer->renderSprite(*sprite);
}