#include "Ship.h"
#include "client/Core/ServiceLocator.h"
#include <fstream>

Ship::Ship()
{
  renderer = Locator::getRenderer();

  // Configure ship
  sprite.setSprite("data/UI/BOARD/bounding box debug.png");
  ship_config = file_handler.openAsJSON("CONFIGS/ship_config.json");
  sprite.setPosition(
    Vector2(ship_config["default_pos"][0], ship_config["default_pos"][1]));
  sprite.setDimensions(
    Vector2(ship_config["default_size"][0], ship_config["default_size"][1]));

  // Build up the rooms in the ship
  rooms.emplace_back("COCKPIT", sprite.getPosition());
  rooms.emplace_back("SUPPLY_BAY", sprite.getPosition());
  rooms.emplace_back("MEDBAY", sprite.getPosition());
  rooms.emplace_back("COMMUNICATIONS", sprite.getPosition());
  rooms.emplace_back("ENGINE", sprite.getPosition());
}

/* Render the ship */
void Ship::render()
{
  sprite.render();
}