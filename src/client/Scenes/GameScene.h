#ifndef PROJECT_GAMESCENE_H
#define PROJECT_GAMESCENE_H

#include "Scene.h"
#include "client/Board/GameBoard.h"
#include "client/Cards/Deck.h"
#include "client/UI/Cursor.h"

class GameScene : public Scene
{
 public:
  GameScene() { debug_text.print("entered game scene."); };
  ~GameScene() = default;

  void init() override;

  void networkConnected() override;
  void networkDisconnected() override;
  void networkDataReceived(const enet_uint8* data, size_t data_size) override;

  void keyHandler(const ASGE::SharedEventData data) override;
  void clickHandler(const ASGE::SharedEventData data) override;
  game_global_scenes update(const ASGE::GameTime& game_time) override;
  void render() override;

  int test_int2 = 1;
  bool test_val = false;

 private:
  GameBoard m_board;
  Menu m_board_menu;
  Deck m_deck;
};

#endif // PROJECT_GAMESCENE_H
