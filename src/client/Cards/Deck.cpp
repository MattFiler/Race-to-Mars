//
// Created by j42-king on 13/03/2019.
//

#include "Deck.h"
#include "IssueCard.h"
#include "ItemCard.h"
#include "ObjectiveCard.h"
#include <algorithm>
#include <random>

Deck::Deck()
{
  // Initialise all decks here.
}

Deck::~Deck() {}

ItemCard Deck::drawItmCard()
{
  ItemCard itm_card = itm_card_deck.back();
  itm_card_deck.pop_back();
  return itm_card;
}

IssueCard Deck::drawIssCard()
{
  IssueCard iss_card = iss_card_deck.back();
  iss_card_deck.pop_back();
  return iss_card;
}

ObjectiveCard Deck::drawObjCard()
{
  ObjectiveCard obj_card = obj_card_deck.back();
  obj_card_deck.pop_back();
  return obj_card;
}

void Deck::shuffleDecks()
{
  // shuffle all decks here.
  std::random_device rd;
  std::mt19937 gen(rd());
  std::shuffle(itm_card_deck.begin(), itm_card_deck.end(), gen);
}
