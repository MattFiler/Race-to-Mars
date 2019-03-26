#ifndef PROJECT_GAMESCENE_H
#define PROJECT_GAMESCENE_H

#include "Scene.h"
#include "client/Board/GameBoard.h"
#include "client/UI/Cursor.h"
#include <client/Cards/IssueCard.h>

// All sprites for the game ui
struct GameSprites
{
  ScaledSprite* background = nullptr;
  ScaledSprite* inactive_player_marker = nullptr;
  ScaledSprite* active_player_marker = nullptr;
  ScaledSprite* progress_meter = nullptr;
  ScaledSprite* progress_marker = nullptr;
  ScaledSprite* sync_overlay = nullptr;
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

  GameBoard m_board;
  Menu pause_menu;
  // Deck m_deck;

  std::vector<IssueCard> active_issues;

  GameSprites game_sprites;
  LobbyPlayer* players[4] = { nullptr, nullptr, nullptr, nullptr };
  int my_player_index = -1;

  int max_issue_cards = 5;
  int max_progress_index = 19; // win condition

  game_state current_state = game_state::PLAYING;
  int current_progress_index = 0;
  bool current_scene_lock_active = false; // optional "scene lock" to freeze
                                          // client interaction - useful for the
                                          // end of a turn?

  // BELOW IS ALL TEMP SHIT READY FOR JACK TO ADD HIS CARD IMPLEMENTATIONS
  int active_issue_cards[5] = { -1, -1, -1, -1, -1 };
  int active_client_objective_card = -1; // hmm, maybe keep this data for every
                                         // client?
};

#endif // PROJECT_GAMESCENE_H
