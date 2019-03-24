//
// Created by j42-king on 24/03/2019.
//

#ifndef PROJECT_ITEMDECK_H
#define PROJECT_ITEMDECK_H

#include <Engine/Renderer.h>
#include <gamelib/FileHandler/FileHandler.h>
#include <gamelib/Localisation/Localisation.h>
#include <gamelib/Math/BoundingBox.h>
#include <gamelib/Math/Vector2.h>
#include <json.hpp>

#include "client/UI/OnBoardObject.h"

using json = nlohmann::json;

class ItemDeck
{
 public:
  ItemDeck();
  ~ItemDeck() = default;

  Vector2 getCentre();
  bool isInBoundingBox(Vector2 _pos);
  void render();

 private:
  ASGE::Renderer* renderer;
  OnBoardObject sprite;
  FileHandler file_handler{};
  BoundingBox bounding_box;
  Localisation localiser{};
  Vector2 centre_point{};
  json config_deck;
};

#endif // PROJECT_ITEMDECK_H
