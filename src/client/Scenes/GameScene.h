#ifndef PROJECT_GAMESCENE_H
#define PROJECT_GAMESCENE_H

#include "Scene.h"
#include "client/Board/GameBoard.h"
#include "client/Cards/Deck.h"
#include "client/UI/Cursor.h"

// All sprites for the game ui
struct GameSprites
{
  ScaledSprite* background = nullptr;
  ScaledSprite* inactive_player_marker = nullptr;
  ScaledSprite* active_player_marker = nullptr;
  ScaledSprite* progress_meter = nullptr;
  ScaledSprite* progress_marker = nullptr;
};

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
  enum game_state
  {
    PLAYING,               // The regular in-game state
    LOCAL_PAUSE,           // The quit menu
    LOOKING_AT_PLAYER_INFO // The player info popup
  };

  game_state current_state = game_state::PLAYING;

  GameBoard m_board;
  Menu pause_menu;
  Deck m_deck;
  GameSprites game_sprites;
  LobbyPlayer* players[4] = { nullptr, nullptr, nullptr, nullptr };
  int my_player_index = -1;
};

#endif // PROJECT_GAMESCENE_H
