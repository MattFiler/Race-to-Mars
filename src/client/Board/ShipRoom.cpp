#include "ShipRoom.h"

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
    static_cast<float>(room_config["centre_point"][0]) + board_offset.x,
    static_cast<float>(room_config["centre_point"][1]) + board_offset.x);
  friendly_name = localiser.getString(room_config["friendly_name"]);
}

/* Get the centre of the room */
Vector2 ShipRoom::getCentre()
{
  return centre_point;
}

/* Get the localised name of the room */
std::string ShipRoom::getName()
{
  return friendly_name;
}

/* Get the localised name of the room */
bool ShipRoom::isInBoundingBox(Vector2 _pos)
{
  return bounding_box.isInside(_pos);
}