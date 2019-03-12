#include <string>

#include "Core/ServiceLocator.h"
#include <Engine/DebugPrinter.h>
#include <Engine/Input.h>
#include <Engine/InputEvents.h>
#include <Engine/Keys.h>
#include <Engine/Sprite.h>

#include <iostream>

#include "game.h"
/**
 *   @brief   Default Constructor.
 *   @details Consider setting the game's width and height
 *            and even seeding the random number generator.
 */
RaceToSpace::RaceToSpace()
{
  game_name = "Race to Space";
}

/**
 *   @brief   Destructor.
 *   @details Remove any non-managed memory and callbacks.
 */
RaceToSpace::~RaceToSpace()
{
  enetpp::global_state::get().deinitialize();

  this->inputs->unregisterCallback(static_cast<unsigned int>(key_callback_id));

  this->inputs->unregisterCallback(
    static_cast<unsigned int>(mouse_callback_id));
}

/**
 *   @brief   Initialises the game.
 *   @details The game window is created and all assets required to
 *            run the game are loaded. The keyHandler and clickHandler
 *            callback should also be set in the initialise function.
 *   @return  True if the game initialised correctly.
 */
bool RaceToSpace::init()
{
  // Load core config
  game_config = file_handler.loadConfig("game_core.json");

  // Initialise networking and connect to server
  std::string server_ip(game_config["server_hostname"]);
  enetpp::global_state::get().initialize();
  client.connect(enetpp::client_connect_params()
                   .set_channel_count(1)
                   .set_server_host_name_and_port(server_ip.c_str(),
                                                  game_config["server_port"]));

  // Thread out our network loop so we can continue with the game loop
  std::thread th(&RaceToSpace::networkLoop, this);
  th.detach();

  // Enable debug input to test comms from client to server (& other clients)
  std::thread th2(&RaceToSpace::networkMessageDebug, this);
  th2.detach();

  // Configure game
  setupResolution();
  if (!initAPI())
  {
    return false;
  }

  // Enable debug outputs if requested
  if (game_config["enable_debug"])
  {
    debug_text.enabled = true;
    toggleFPS();
  }

  // Setup our locator
  Locator::setupRenderer(renderer.get());
  Locator::setupInput(inputs.get());
  Locator::setupAudio(&audio);

  // Setup keybinds
  key_handler.setup(game_config["keybinds"]);

  // Configure localisation
  localiser.configure(game_config["language"]);

  // Start out on the main menu
  scene_manager.setCurrentScene(game_global_scenes::MAIN_MENU);

  // input handling functions
  inputs->use_threads = false;

  key_callback_id =
    inputs->addCallbackFnc(ASGE::E_KEY, &RaceToSpace::keyHandler, this);

  mouse_callback_id = inputs->addCallbackFnc(
    ASGE::E_MOUSE_CLICK, &RaceToSpace::clickHandler, this);

  return true;
}

// Our network connection loop
void RaceToSpace::networkLoop()
{
  while (client.is_connecting_or_connected())
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    using namespace std::placeholders;
    client.consume_events(std::bind(&RaceToSpace::connection, this),
                          std::bind(&RaceToSpace::disconnection, this),
                          std::bind(&RaceToSpace::data, this, _1, _2));

    while (msg_queue.size())
    {
      std::lock_guard<std::mutex> lock(msg_queue_mtx);
      const auto& txt = msg_queue.front();
      assert(sizeof(char) == sizeof(enet_uint8));
      client.send_packet(0,
                         reinterpret_cast<const enet_uint8*>(txt.data()),
                         txt.length(),
                         ENET_PACKET_FLAG_RELIABLE);
      msg_queue.pop();
    }
  }

  exiting = true;
}

// Allow us to send a debug message over the network to our server and any other
// clients
void RaceToSpace::networkMessageDebug()
{
  while (!exiting)
  {
    std::string txt;
    std::getline(std::cin, txt);

    std::lock_guard<std::mutex> lock(msg_queue_mtx);
    msg_queue.push(std::move(txt));
  }
}

// Act on connection to our server
void RaceToSpace::connection()
{
  debug_text.print("Connected to server.");
}

// Act on disconnection to our server
void RaceToSpace::disconnection()
{
  debug_text.print("Disconnected from server.");
}

// Pass data between us and the server
void RaceToSpace::data(const enet_uint8* data, size_t data_size)
{
  std::string msg(reinterpret_cast<const char*>(data), data_size);
  debug_text.print("Recieved message from server: " + msg);
}

/**
 *   @brief   Sets the game window resolution
 *   @details This function is designed to create the window size, any
 *            aspect ratio scaling factors and safe zones to ensure the
 *            game frames when resolutions are changed in size.
 *   @return  void
 */
void RaceToSpace::setupResolution()
{
  game_width = game_config["resolution"]["width"];
  game_height = game_config["resolution"]["height"];
}

/**
 *   @brief   Processes any key inputs
 *   @details This function is added as a callback to handle the game's
 *            keyboard input. For this game, calls to this function
 *            are thread safe, so you may alter the game's state as
 *            you see fit.
 *   @param   data The event data relating to key input.
 *   @see     KeyEvent
 *   @return  void
 */
void RaceToSpace::keyHandler(const ASGE::SharedEventData data)
{
  scene_manager.keyHandler(data);
}

/**
 *   @brief   Processes any click inputs
 *   @details This function is added as a callback to handle the game's
 *            mouse button input. For this game, calls to this function
 *            are thread safe, so you may alter the game's state as you
 *            see fit.
 *   @param   data The event data relating to key input.
 *   @see     ClickEvent
 *   @return  void
 */
void RaceToSpace::clickHandler(const ASGE::SharedEventData data)
{
  scene_manager.clickHandler(data);
}

/**
 *   @brief   Updates the scene
 *   @details Prepares the renderer subsystem before drawing the
 *            current frame. Once the current frame is has finished
 *            the buffers are swapped accordingly and the image shown.
 *   @return  void
 */
void RaceToSpace::update(const ASGE::GameTime& game_time)
{
  scene_manager.update(game_time);
}

/**
 *   @brief   Renders the scene
 *   @details Renders all the game objects to the current frame.
 *            Once the current frame is has finished the buffers are
 *            swapped accordingly and the image shown.
 *   @return  void
 */
void RaceToSpace::render(const ASGE::GameTime&)
{
  scene_manager.render();
}
