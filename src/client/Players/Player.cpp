#include "Player.h"
#include "gamelib/Constants.h"

Player::Player()
{
  renderer = Locator::getRenderer();
  config = file_handler.openAsJSON("CONFIGS/players.json");
}

void Player::render(game_global_scenes game_scene)
{
  switch (game_scene)
  {
    case game_global_scenes::IN_GAME:
    {
      counter.render();
      break;
    }
    case game_global_scenes::LOBBY:
    {
      renderer->renderSprite(*lobby_sprite->getSprite());
      break;
    }
    default:
    {
      break;
    }
  }
}

void Player::setup()
{
  counter.setSprite(getCounterSpritePath());
  counter.setDimensions(
    Vector2(config["counter_width"], config["counter_height"]));
  lobby_sprite = new ScaledSprite(getLobbySpritePath());
}