#include "TutorialScene.h"

/* Initialise the scene */
void TutorialScene::init()
{
  ui_manager.createSprite(ui_tutorial::TUTORIAL_INTRO, "UI/TUTORIAL/intro.png");

  ui_manager.createButton(ui_tutorial_btns::CONTINUE_BUTTON,
                          "UI/TUTORIAL/continue_button.png");
}

/* Handles receiving data from the server */
void TutorialScene::networkDataReceived(const enet_uint8* data,
                                        size_t data_size)
{
}

/* Handles key inputs */
void TutorialScene::keyHandler(const ASGE::SharedEventData data)
{
  keys.registerEvent(static_cast<const ASGE::KeyEvent*>(data.get()));
  if (keys.keyPressed("Activate"))
  {
    current_slide++;

    // If we've reached the end, throw us back to the menu
    if (current_slide == ui_tutorial::NUMBER_OF_SLIDES)
    {
      next_scene = game_global_scenes::MAIN_MENU;
    }
  }
}

/* Handle clicks */
void TutorialScene::clickHandler(const ASGE::SharedEventData data) {}

/* Update function */
game_global_scenes TutorialScene::update(const ASGE::GameTime& game_time)
{
  return next_scene;
}

/* Render function */
void TutorialScene::render()
{
  // main_menu.render();
}