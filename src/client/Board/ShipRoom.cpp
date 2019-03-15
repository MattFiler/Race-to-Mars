#include "ShipRoom.h"

ShipRoom::ShipRoom(const std::string& room_name, Vector2 board_offset)
{
  room_config = file_handler.loadConfig("UI/BOARD/ship_rooms.json", room_name);

  bounding_box->width = room_config["bounding_box_size"][0];
  bounding_box->height = room_config["bounding_box_size"][1];
  bounding_box->pos.update(
    static_cast<float>(room_config["bounding_box_pos"][0]) + board_offset.x,
    static_cast<float>(room_config["bounding_box_pos"][1]) + board_offset.y);
}

ShipRoom::~ShipRoom()
{
  delete bounding_box;
  bounding_box = nullptr;
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
  return bounding_box->isInside(_pos);
}