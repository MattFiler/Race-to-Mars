#include "Player.h"
#include "AllPlayers.h"
#include "gamelib/Constants.h"
#include <gamelib/Math/SimpleMath.h>

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
    case game_global_scenes ::MAIN_MENU:
    {
      break;
    }
    case game_global_scenes ::DEFAULT:
    {
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
  game_tab_sprite = new ScaledSprite(getGameTabSpritePath());
}

int Player::getDiceRoll()
{
  return math.generateRandInt(1, 6);
}
