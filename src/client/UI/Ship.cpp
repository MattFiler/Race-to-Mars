#include "Ship.h"
#include "client/Core/ServiceLocator.h"

Ship::Ship()
{
  renderer = Locator::getRenderer();
  board_sprite.setSprite("data/UI/BOARD/ship-test.png");
  board_sprite.setPosition(Vector2(100, 100));
  board_sprite.setDimensions(Vector2(300, 300));
}

void Ship::render()
{
  board_sprite.render();
}