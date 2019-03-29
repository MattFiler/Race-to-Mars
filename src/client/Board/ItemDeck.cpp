//
// Created by j42-king on 24/03/2019.
//

#include "ItemDeck.h"

Vector2 ItemDeck::getCentre()
{
  return centre_point;
}

bool ItemDeck::isInBoundingBox(Vector2 _pos)
{
  return bounding_box.isInside(_pos);
}

void ItemDeck::render()
{
  sprite.render();
}

ItemDeck::ItemDeck()
{
  renderer = Locator::getRenderer();

  sprite.setSprite("data/UI/BOARD/item_card_back.png");
}
