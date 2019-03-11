#include "Player.h"

Player::Player()
{
  renderer = Locator::getRenderer();
  config = file_handler.openAsJSON("CONFIGS/players.json");
}

void Player::render()
{
  counter.render();
}