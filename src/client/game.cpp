#include <string>

#include "Core/ServiceLocator.h"
#include <Engine/DebugPrinter.h>
#include <Engine/Input.h>
#include <Engine/InputEvents.h>
#include <Engine/Keys.h>
#include <Engine/Sprite.h>

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

  setupResolution();
  if (!initAPI())
  {
    return false;
  }

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
