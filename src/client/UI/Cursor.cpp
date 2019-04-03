#include "Cursor.h"
#include "client/Core/ServiceLocator.h"

Cursor::~Cursor()
{
  delete cursor_sprite_active;
  cursor_sprite_active = nullptr;

  delete cursor_sprite_inactive;
  cursor_sprite_inactive = nullptr;
}

/* Create and set all cursor sprites */
void Cursor::configure()
{
  renderer = Locator::getRenderer();

  cursor_sprite_active = new ScaledSprite("data/UI/CURSOR_POINTER.png");
  cursor_sprite_inactive = new ScaledSprite("data/UI/CURSOR_DEFAULT.png");
}

/* set cursor type (hover/not) */
void Cursor::setCursorActive(bool _active)
{
  is_active = _active;
}

/* Update cursor postion */
void Cursor::updatePosition(double x_pos, double y_pos)
{
  cursor_pos.update(static_cast<float>(x_pos) / GameResolution::scale,
                    static_cast<float>(y_pos) / GameResolution::scale);
  if (is_active)
  {
    cursor_sprite_active->xPos(static_cast<float>(x_pos - 7) /
                               GameResolution::scale);
    cursor_sprite_active->yPos(static_cast<float>(y_pos) /
                               GameResolution::scale);
    return;
  }
  cursor_sprite_inactive->xPos(static_cast<float>(x_pos) /
                               GameResolution::scale);
  cursor_sprite_inactive->yPos(static_cast<float>(y_pos) /
                               GameResolution::scale);
}

/* Get cursor postion */
Vector2 Cursor::getPosition()
{
  return cursor_pos;
}

/* render cursor */
void Cursor::render()
{
  if (is_active)
  {
    renderer->renderSprite(*cursor_sprite_active->getSprite(),
                           render_order::PRIORITY_CURSOR);
    return;
  }
  renderer->renderSprite(*cursor_sprite_inactive->getSprite(),
                         render_order::PRIORITY_CURSOR);
}