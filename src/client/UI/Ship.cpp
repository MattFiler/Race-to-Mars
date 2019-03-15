#include "Ship.h"
#include "client/Core/ServiceLocator.h"
#include <fstream>

Ship::Ship()
{
  //room_config = file_handler.openAsJSON("CONFIGS/players.json");


  renderer = Locator::getRenderer();
  board_sprite.setSprite("data/UI/BOARD/bounding box debug.png");
  board_sprite.setPosition(Vector2(100, 100));
  board_sprite.setDimensions(Vector2(300, 300));
}

Vector2 Ship::getPositionForRoom(ship_rooms _room)
{
  switch (_room)
  {
    case ship_rooms::COCKPIT:
    {
        return Vector2(0,0);
    }
    case ship_rooms::COMMUNICATIONS:
    {
        return Vector2(0,0);
    }
    case ship_rooms::ENGINE_ROOM:
    {
        return Vector2(0,0);
    }
    case ship_rooms::MEDIC_WING:
    {
        return Vector2(0,0);
    }
    case ship_rooms::SUPPLY_BAY:
    {
        return Vector2(0,0);
    }
  }
    return Vector2(0,0);
}

void Ship::render()
{
  board_sprite.render();
}