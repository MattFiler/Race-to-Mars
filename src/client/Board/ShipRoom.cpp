#include "ShipRoom.h"

ShipRoom::ShipRoom(const std::string& room_name, Vector2 board_offset)
{
  room_config = file_handler.loadConfig("ship_rooms.json", room_name);

  bounding_box.setup(
    Vector2(
      static_cast<float>(room_config["bounding_box_pos"][0]) + board_offset.x,
      static_cast<float>(room_config["bounding_box_pos"][1]) + board_offset.y),
    room_config["bounding_box_size"][0],
    room_config["bounding_box_size"][1]);
}

/* Get the centre of the room */
Vector2 ShipRoom::getCentre()
{
  return Vector2(room_config["centre_point"][0],
                 room_config["centre_point"][1]);
}

/* Get the localised name of the room */
std::string ShipRoom::getName()
{
  return localiser.getString(room_config["friendly_name"]);
}

/* Get the localised name of the room */
bool ShipRoom::isInBoundingBox(Vector2 _pos)
{
  return bounding_box.isInside(_pos);
}