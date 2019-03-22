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
  card_config = file_handler.openAsJSON("CONFIGS/cards.json");

  // load item cards
  for (size_t i = 0; i < 10; ++i)
  {
    ItemCard temp_card;
    temp_card.setCardName(card_config["ITEMCARDS"]["communications"][i]["nam"
                                                                        "e"]);
    temp_card.setActionPoints(
      card_config["ITEMCARDS"]["communications"][i]["action_points"]);
    temp_card.setSpritePath(card_config["ITEMCARDS"]["communications"][i]["spri"
                                                                          "te_"
                                                                          "pat"
                                                                          "h"]);
    this->itm_card_deck.push_back(temp_card);
  }

  for (size_t i = 0; i < 10; ++i)
  {
    ItemCard temp_card;
    temp_card.setCardName(card_config["ITEMCARDS"]["engineer"][i]["name"]);
    temp_card.setActionPoints(card_config["ITEMCARDS"]["engineer"][i]["action_"
                                                                      "point"
                                                                      "s"]);
    temp_card.setSpritePath(card_config["ITEMCARDS"]["engineer"][i]["sprite_"
                                                                    "path"]);
    this->itm_card_deck.push_back(temp_card);
  }

  for (size_t i = 0; i < 10; ++i)
  {
    ItemCard temp_card;
    temp_card.setCardName(card_config["ITEMCARDS"]["medic"][i]["name"]);
    temp_card.setActionPoints(card_config["ITEMCARDS"]["medic"][i]["action_"
                                                                   "points"]);
    temp_card.setSpritePath(card_config["ITEMCARDS"]["medic"][i]["sprite_"
                                                                 "path"]);
    this->itm_card_deck.push_back(temp_card);
  }

  for (size_t i = 0; i < 10; ++i)
  {
    ItemCard temp_card;
    temp_card.setCardName(card_config["ITEMCARDS"]["pilot"][i]["name"]);
    temp_card.setActionPoints(card_config["ITEMCARDS"]["pilot"][i]["action_"
                                                                   "points"]);
    temp_card.setSpritePath(card_config["ITEMCARDS"]["pilot"][i]["sprite_"
                                                                 "path"]);
    this->itm_card_deck.push_back(temp_card);
  }

  // Load Issue Cards
  for (size_t i = 0; i < 6; ++i)
  {
    IssueCard temp_card;
    temp_card.setCardName(card_config["ISSUECARDS"]["communications"][i]["nam"
                                                                         "e"]);
    temp_card.setCardDescription(
      card_config["ISSUECARDS"]["communications"][i]["description"]);
    temp_card.setActionPoints(
      card_config["ISSUECARDS"]["communications"][i]["action_points"]);
    temp_card.setSpritePath(
      card_config["ISSUECARDS"]["communications"][i]["sprite_path"]);
    this->iss_card_deck.push_back(temp_card);
  }

  for (size_t i = 0; i < 6; ++i)
  {
    IssueCard temp_card;
    temp_card.setCardName(card_config["ITEMCARDS"]["engineer"][i]["name"]);
    temp_card.setCardDescription(card_config["ISSUECARDS"]["engineer"][i]["desc"
                                                                          "ript"
                                                                          "io"
                                                                          "n"]);
    temp_card.setActionPoints(card_config["ITEMCARDS"]["engineer"][i]["action_"
                                                                      "point"
                                                                      "s"]);
    temp_card.setSpritePath(card_config["ITEMCARDS"]["engineer"][i]["sprite_"
                                                                    "path"]);
    this->iss_card_deck.push_back(temp_card);
  }

  for (size_t i = 0; i < 6; ++i)
  {
    IssueCard temp_card;
    temp_card.setCardName(card_config["ISSUECARDS"]["global"][i]["name"]);
    temp_card.setCardDescription(card_config["ISSUECARDS"]["global"][i]["descri"
                                                                        "ptio"
                                                                        "n"]);
    temp_card.setActionPoints(card_config["ISSUECARDS"]["global"][i]["action_"
                                                                     "points"]);
    temp_card.setSpritePath(card_config["ISSUECARDS"]["global"][i]["sprite_"
                                                                   "path"]);
    this->iss_card_deck.push_back(temp_card);
  }

  for (size_t i = 0; i < 6; ++i)
  {
    IssueCard temp_card;
    temp_card.setCardName(card_config["ISSUECARDS"]["medic"][i]["name"]);
    temp_card.setCardDescription(card_config["ISSUECARDS"]["medic"][i]["descrip"
                                                                       "tion"]);
    temp_card.setActionPoints(card_config["ISSUECARDS"]["medic"][i]["action_"
                                                                    "points"]);
    temp_card.setSpritePath(card_config["ISSUECARDS"]["medic"][i]["sprite_"
                                                                  "path"]);
    this->iss_card_deck.push_back(temp_card);
  }

  for (size_t i = 0; i < 6; ++i)
  {
    IssueCard temp_card;
    temp_card.setCardName(card_config["ISSUECARDS"]["pilot"][i]["name"]);
    temp_card.setCardDescription(card_config["ISSUECARDS"]["pilot"][i]["descrip"
                                                                       "tion"]);
    temp_card.setActionPoints(card_config["ISSUECARDS"]["pilot"][i]["action_"
                                                                    "points"]);
    temp_card.setSpritePath(card_config["ISSUECARDS"]["pilot"][i]["sprite_"
                                                                  "path"]);
    this->iss_card_deck.push_back(temp_card);
  }

  // Load Objective Cards
  for (size_t i = 0; i < 26; ++i)
  {
    IssueCard temp_card;
    temp_card.setCardName(card_config["OBJECTIVECARDS"]["cards"][i]["name"]);
    temp_card.setCardDescription(
      card_config["OBJECTIVECARDS"]["cards"][i]["description"]);
    temp_card.setAbilityText(card_config["OBJECTIVECARDS"]["cards"][i]["abilit"
                                                                       "y"]);
    temp_card.setSpritePath(card_config["OBJECTIVECARDS"]["sprite_path"]);
    this->iss_card_deck.push_back(temp_card);
  }
}

void Deck::retargetConfig(const std::string& name)
{
  card_config = file_handler.loadConfigFromExisting(card_config, name);
}
