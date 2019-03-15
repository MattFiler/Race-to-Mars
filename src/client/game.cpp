#include <string>

#include "Core/ServiceLocator.h"
#include "gamelib/Constants.h"
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

  // Connect us to our server
  networked_client.connectToServer(game_config["server_hostname"],
                                   game_config["server_port"]);

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
  Locator::setupClient(&networked_client);

  // Setup keybinds
  key_handler.setup(game_config["keybinds"]);

  // Configure localisation
  localiser.configure(game_config["language"]);

  // Start out on the main menu
  scene_manager.setCurrentScene(game_global_scenes::MAIN_MENU);

  // Load font
  auto font_buffer = file_handler.openAsBuffer("UI/font_regular.ttf");
  active_font =
    renderer->loadFontFromMem("Alte Haas",
                              font_buffer.as_unsigned_char(),
                              static_cast<unsigned int>(font_buffer.length),
                              30 * static_cast<int>(GameResolution::scale));

  // Input handling functions
  inputs->use_threads = false;
  key_callback_id =
    inputs->addCallbackFnc(ASGE::E_KEY, &RaceToSpace::keyHandler, this);
  mouse_callback_id = inputs->addCallbackFnc(
    ASGE::E_MOUSE_CLICK, &RaceToSpace::clickHandler, this);

  // Start listening to the server
  networked_client.startListening(this);

  return true;
}

// Act on connection to our server
void RaceToSpace::connection()
{
  scene_manager.networkConnected();

  // debugging
  debug_text.print("Connected to server.");
  has_connected_to_server = true;
}

// Act on disconnection to our server
void RaceToSpace::disconnection()
{
  scene_manager.networkDisconnected();

  // debugging
  debug_text.print("Disconnected from server.");
  has_connected_to_server = false;
}

// Function called when data is received from the server
void RaceToSpace::data(const enet_uint8* data, size_t data_size)
{
  scene_manager.networkDataReceived(data, data_size);

  // Debugging with message send/receive
  std::string msg(reinterpret_cast<const char*>(data), data_size);
  debug_text.print("received message from server: " + msg);
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
  GameResolution::width = game_width;
  game_height = game_config["resolution"]["height"];
  GameResolution::height = game_height;
  GameResolution::scale = static_cast<float>(game_height) / 720;
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
  renderer->setFont(active_font);

  scene_manager.render();

  // Server connection debug
  if (has_connected_to_server)
  {
    std::string server_ip(game_config["server_hostname"]);
    renderer->renderText("CONNECTED: " + server_ip, game_width - 250, 50, 0.5f);
    renderer->renderText("PING: " + std::to_string(networked_client.getClient()
                                                     ->get_statistics()
                                                     ._round_trip_time_in_ms),
                         game_width - 250,
                         75,
                         0.5f);
  }
  else
  {
    renderer->renderText("NOT CONNECTED", game_width - 250, 50, 0.5f);
  }
}
