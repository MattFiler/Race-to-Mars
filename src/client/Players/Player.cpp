#include "Player.h"

Player::Player()
{
  renderer = Locator::getRenderer();
  config = file_handler.openAsJSON("CONFIGS/players.json");
  counter.setDimensions(Vector2(50, 50));
}

void Player::render()
{
  counter.render();
}