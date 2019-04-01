#include "ShipRoom.h"
#include "../Core/ServiceLocator.h"
#include "../Players/AllPlayers.h"

ShipRoom::ShipRoom(const std::string& room_name, Vector2 board_offset)
{
  json room_config = file_handler.loadConfig("ship_rooms.json", room_name);

  bounding_box.setup(
    Vector2((static_cast<float>(room_config["bounding_box_pos"][0])) +
              board_offset.x,
            (static_cast<float>(room_config["bounding_box_pos"][1])) +
              board_offset.y),
    static_cast<float>(room_config["bounding_box_size"][0]),
    static_cast<float>(room_config["bounding_box_size"][1]));

  centre_point.update(
    (static_cast<float>(room_config["bounding_box_pos"][0])) + board_offset.x +
      (static_cast<float>(room_config["bounding_box_size"][0]) / 2),
    (static_cast<float>(room_config["bounding_box_pos"][1])) + board_offset.y +
      (static_cast<float>(room_config["bounding_box_size"][1]) / 2));
  friendly_name = localiser.getString(room_config["friendly_name"]);
}

/* Get the centre of the room */
Vector2 ShipRoom::getPosForPlayer(player_classes _player)
{
  Vector2 player_offset = Vector2(0, 0);
  Vector2 player_size =
    Vector2(Locator::getPlayers()->getPlayer(_player)->getWidth(),
            Locator::getPlayers()->getPlayer(_player)->getHeight());
  switch (_player)
  {
    case player_classes::UNASSIGNED:
    {
      player_offset.update(-9999, -9999); // Don't think this should ever be
                                          // called. Disconnected players can't
                                          // move!
      break;
    }
    case player_classes::COMMUNICATIONS:
    {
      player_offset.update(-player_size.x, -player_size.y);
      break;
    }
    case player_classes::ENGINEER:
    {
      player_offset.update(0, -player_size.y);
      break;
    }
    case player_classes::PILOT:
    {
      player_offset.update(-player_size.x, 0);
      break;
    }
    case player_classes::MEDIC:
    {
      player_offset.update(0, 0);
      break;
    }
  }
  return centre_point + player_offset;
}

/* Get the localised name of the room */
std::string ShipRoom::getName()
{
  return friendly_name;
}

/* Is the position within our bounding box? */
bool ShipRoom::isInBoundingBox(Vector2 _pos)
{
  return bounding_box.isInside(_pos);
}