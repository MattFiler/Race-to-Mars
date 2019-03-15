#include "ShipRoom.h"

ShipRoom::ShipRoom(const std::string& room_name,
                   Vector2 board_offset,
                   Vector2 ship_base_size)
{
  json room_config = file_handler.loadConfig("ship_rooms.json", room_name);

  float bb_x = static_cast<float>(room_config["bounding_box_pos"][0]);
  float bb_y = static_cast<float>(room_config["bounding_box_pos"][1]);

  bounding_box.setup(
    Vector2(
      (bb_x * ship_base_size.x) + (board_offset.x / GameResolution::scale),
      (bb_y * ship_base_size.y) + (board_offset.y / GameResolution::scale)),
    static_cast<float>(room_config["bounding_box_size"][0]) * ship_base_size.x,
    static_cast<float>(room_config["bounding_box_size"][1]) * ship_base_size.y);

  centre_point.update(static_cast<float>(room_config["centre_point"][0]) +
                        (board_offset.x / GameResolution::scale),
                      static_cast<float>(room_config["centre_point"][1]) +
                        (board_offset.x / GameResolution::scale));
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