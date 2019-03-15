#include "ScaledSprite.h"
#include "gamelib/Debug/DebugText.h"

/* Create */
ScaledSprite::ScaledSprite(const std::string& filename)
{
  renderer = Locator::getRenderer();

  sprite = renderer->createRawSprite();
  sprite->loadTexture(filename);
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
  sprite->width(width * GameResolution::scale);
}

/* Set height */
void ScaledSprite::height(float height)
{
  sprite->height(height * GameResolution::scale);
}

/* Set X */
void ScaledSprite::xPos(float x)
{
  sprite->xPos(x * GameResolution::scale);
}

/* Set Y */
void ScaledSprite::yPos(float y)
{
  sprite->yPos(y * GameResolution::scale);
}

/* Set X and Y */
void ScaledSprite::setPos(Vector2 new_pos)
{
  sprite->xPos(new_pos.x * GameResolution::scale);
  sprite->yPos(new_pos.y * GameResolution::scale);
}