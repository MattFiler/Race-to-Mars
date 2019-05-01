#include "TutorialScene.h"
#include "client/UI/Cursor.h"

/* Initialise the scene */
void TutorialScene::init()
{
  // Load sfx
  file_handler.loadSound(active_sfx, "popup_show");

  // Load slides
  for (int i = 0; i < ui_tutorial::NUMBER_OF_SLIDES; i++)
  {
    ui_manager.createSprite(i, "UI/TUTORIAL/" + std::to_string(i) + ".png");
  }

  // Setup button
  ClickableButton* cont_button = ui_manager.createButton(
    ui_tutorial_btns::CONTINUE_BUTTON, "UI/TUTORIAL/continue_button.png");
  cont_button->setPos(Vector2(1098, 674));
  cont_button->setActive(true);
}

/* Handles key inputs */
void TutorialScene::keyHandler(const ASGE::SharedEventData data)
{
  keys.registerEvent(static_cast<const ASGE::KeyEvent*>(data.get()));

  // Progress tutorial slideshow on press of "Activate"
  if (keys.keyPressed("Activate"))
  {
    Locator::getAudio()->play(active_sfx);
    current_slide++;

    // If we've reached the end, throw us back to the menu
    if (current_slide == ui_tutorial::NUMBER_OF_SLIDES)
    {
      next_scene = game_global_scenes::MAIN_MENU;
    }
  }
}
}

/* Handle clicks */
void TutorialScene::clickHandler(const ASGE::SharedEventData data)
{
  // Only handle clicks if we actually clicked!
  auto click = static_cast<const ASGE::ClickEvent*>(data.get());
  if (click->action != ASGE::E_MOUSE_CLICK)
  {
    return;
  }

  // Progress tutorial slideshow on click of continue button
  if (ui_manager.getButton(ui_tutorial_btns::CONTINUE_BUTTON)->clicked())
  {
    Locator::getAudio()->play(active_sfx);
    current_slide++;

    // If we've reached the end, throw us back to the menu
    if (current_slide == ui_tutorial::NUMBER_OF_SLIDES)
    {
      next_scene = game_global_scenes::MAIN_MENU;
    }
  }
}

/* Update function */
game_global_scenes TutorialScene::update(const ASGE::GameTime& game_time)
{
  // Update cursor hover state
  Locator::getCursor()->setCursorActive(
    ui_manager.getButton(ui_tutorial_btns::CONTINUE_BUTTON)->clicked());
  return next_scene;
}

/* Render function */
void TutorialScene::render()
{
  // Render current slide and button
  Locator::getRenderer()->renderSprite(
    *ui_manager.getSprite(current_slide)->getSprite());
  ui_manager.getButton(ui_tutorial_btns::CONTINUE_BUTTON)->render();
}