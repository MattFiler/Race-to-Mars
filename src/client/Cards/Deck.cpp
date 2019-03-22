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
  std::shuffle(iss_card_deck.begin(), iss_card_deck.end(), gen);
  std::shuffle(obj_card_deck.begin(), obj_card_deck.end(), gen);
}

// https://stackoverflow.com/questions/11197818/how-do-i-make-a-json-object-with-multiple-arrays
void Deck::initDecks()
{
  // Load in all decks here.
  //  card_config = file_handler.openAsJSON("CONFIGS/cards.json");
  //
  //  // load item cards
  //  retargetConfig("ITEMCARDS");
  //  for (const auto& category : *card_config["communications"].begin())
  //  {
  //    ItemCard temp_item;
  //    temp_item.setItemName(category["name"]);
  //    temp_item.setActionPoints(category["action_points"]);
  //    temp_item.setSpritePath("sprite_path");
  //  }

  // Load issue Cards

  // Load Objective Cards
}

void Deck::retargetConfig(const std::string& name)
{
  card_config = file_handler.loadConfigFromExisting(card_config, name);
}
