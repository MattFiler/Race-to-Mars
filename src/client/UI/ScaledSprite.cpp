#include "ScaledSprite.h"
#include "gamelib/Debug/DebugText.h"

/* Create */
ScaledSprite::ScaledSprite(const std::string& filename)
{
  renderer = Locator::getRenderer();

  sprite = renderer->createRawSprite();
  sprite->loadTexture(filename);

  sprite->scale(GameResolution::scale);

  bounding_box.setup(Vector2(0, 0), sprite->width(), sprite->height());
}

/* Destroy */
ScaledSprite::~ScaledSprite()
{
  delete sprite;
  sprite = nullptr;
}

/* Set width */
void ScaledSprite::width(float width)
{
  sprite->scale(1);
  sprite->width(width * GameResolution::scale);
  bounding_box.width = sprite->width();
}

/* Set height */
void ScaledSprite::height(float height)
{
  sprite->scale(1);
  sprite->height(height * GameResolution::scale);
  bounding_box.height = sprite->height();
}

/* Set X */
void ScaledSprite::xPos(float x_pos)
{
  sprite->xPos(x_pos * GameResolution::scale);
  bounding_box.pos = Vector2(sprite->xPos(), bounding_box.pos.y);
}

/* Set Y */
void ScaledSprite::yPos(float y_pos)
{
  sprite->yPos(y_pos * GameResolution::scale);
  bounding_box.pos = Vector2(bounding_box.pos.x, sprite->yPos());
}

/* Set X and Y */
void ScaledSprite::setPos(Vector2 new_pos)
{
  sprite->xPos(new_pos.x * GameResolution::scale);
  sprite->yPos(new_pos.y * GameResolution::scale);
  bounding_box.pos = Vector2(sprite->xPos(), sprite->yPos());
}