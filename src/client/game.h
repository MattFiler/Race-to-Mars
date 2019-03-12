#pragma once
#include "Debug/DebugText.h"
#include "FileHandler/FileHandler.h"
#include "KeyHandler/KeyHandler.h"
#include "Localisation/Localisation.h"
#include "Scenes/SceneManager.h"
#include <Engine/OGLGame.h>
#include <atomic>
#include <enetpp/client.h>
#include <json.hpp>
#include <queue>
#include <soloud.h>
#include <string>
using json = nlohmann::json;

/**
 *  An OpenGL Game based on ASGE.
 */
class RaceToSpace : public ASGE::OGLGame
{
 public:
  RaceToSpace();
  ~RaceToSpace();
  virtual bool init() override;

  void networkLoop();
  void networkMessageDebug();

  void connection();
  void disconnection();
  void data(const enet_uint8* data, size_t data_size);

 private:
  void keyHandler(const ASGE::SharedEventData data);
  void clickHandler(const ASGE::SharedEventData data);
  void setupResolution();

  virtual void update(const ASGE::GameTime&) override;
  virtual void render(const ASGE::GameTime&) override;

  int key_callback_id = -1;   /**< Key Input Callback ID. */
  int mouse_callback_id = -1; /**< Mouse Input Callback ID. */

 private:
  enetpp::client client;
  std::atomic<bool> exiting = false;
  std::queue<std::string> msg_queue;
  std::mutex msg_queue_mtx;

  bool has_connected_to_server = false;

  json game_config;
  FileHandler file_handler;
  DebugText debug_text;
  KeyHandler key_handler;
  SoLoud::Soloud audio;
  SceneManager scene_manager;
  Localisation localiser;
};