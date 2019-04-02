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
  return sprite->getBoundingBox().isInside(_pos);
}

ItemDeck::ItemDeck()
{
  setSprite("data/UI/BOARD/item_card_back.png");
}
