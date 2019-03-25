//
// Created by j42-king on 13/03/2019.
//

#ifndef PROJECT_DECK_H
#define PROJECT_DECK_H

#include "gamelib/FileHandler/FileHandler.h"
#include "gamelib/NetworkedData/Cards.h"
#include <client/UI/ScaledSprite.h>
#include <json.hpp>
#include <vector>

using json = nlohmann::json;

class ItemCard;
class IssueCard;
class ObjectiveCard;
class Card;

class Deck
{
 public:
  Deck();
  ~Deck();

  void setup();

  std::string getObjectiveBackSpritePath()
  {
    return card_config["CARDBACK"]["objective_card"];
  }
  std::string getItemBackSpritePath()
  {
    return card_config["CARDBACK"]["item_card"];
  }
  std::string getIssueBackSpritePath()
  {
    return card_config["CARDBACK"]["issue_card"];
  }

  void initDecks();
  void retargetConfig(const std::string& name);

  ItemCard drawItmCard();
  IssueCard drawIssCard();
  ObjectiveCard drawObjCard();
  void shuffleDecks();

 private:
  ASGE::Renderer* renderer = nullptr;

  std::vector<ItemCard> itm_card_deck;
  std::vector<IssueCard> iss_card_deck;
  std::vector<ObjectiveCard> obj_card_deck;

  FileHandler file_handler;
  json card_config;

  ScaledSprite* m_itmdeck_card_sprite = nullptr;
  // ScaledSprite* m_issdeck_back_sprite = nullptr;
  // ScaledSprite* m_objdeck_card_sprite = nullptr;
};

#endif // PROJECT_DECK_H
