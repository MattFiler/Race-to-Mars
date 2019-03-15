//
// Created by j42-king on 13/03/2019.
//

#ifndef PROJECT_DECK_H
#define PROJECT_DECK_H

#include <json.hpp>
#include <vector>
using json = nlohmann::json;

class ItemCard;
class IssueCard;
class ObjectiveCard;

class Deck
{
 public:
  Deck();
  ~Deck();

  ItemCard drawItmCard();
  IssueCard drawIssCard();
  ObjectiveCard drawObjCard();
  void shuffleDecks();

 private:
  std::vector<ItemCard> itm_card_deck;
  std::vector<IssueCard> iss_card_deck;
  std::vector<ObjectiveCard> obj_card_deck;
  json config;
};

#endif // PROJECT_DECK_H
