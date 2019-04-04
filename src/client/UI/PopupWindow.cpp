#include "PopupWindow.h"
#include "client/Core/ServiceLocator.h"
#include "gamelib/Constants.h"

/* Create the popup */
PopupWindow::PopupWindow()
{
  renderer = Locator::getRenderer();
  sprite = new ScaledSprite("data/UI/INGAME_UI/popup_blank.png");

  close_button = new ClickableButton("data/UI/INGAME_UI/close_button.png");
  close_button->setPos(Vector2(1193, 83));
}

/* Destroy the popup and all contents */
PopupWindow::~PopupWindow()
{
  delete sprite;
  sprite = nullptr;

  popup_sprites.clear();

  delete close_button;
  close_button = nullptr;
}

/* Handle key inputs */
void PopupWindow::keyHandler(KeyHandler keys)
{
  if (keys.keyReleased("Back") && timeout == -1 && is_active)
  {
    hide();
  }
}

/* Handle mouse inputs */
void PopupWindow::clickHandler(Vector2 mouse_pos)
{
  // Was manually opened, check for close request
  if (timeout == -1)
  {
    if (close_button->clicked())
    {
      hide();
    }
    return;
  }
}

/* Create a sprite for the popup */
ScaledSprite& PopupWindow::createSprite(const std::string& sprite_path)
{
  ScaledSprite* new_sprite = new ScaledSprite(sprite_path);
  popup_sprites.push_back(new_sprite);
  return *new_sprite;
}

/* Add a reference of an existing sprite for the popup */
ScaledSprite& PopupWindow::referenceSprite(ScaledSprite& ref_sprite)
{
  popup_sprites_referenced.push_back(&ref_sprite);
  return ref_sprite;
}

/* Create a button for the popup */
ClickableButton& PopupWindow::createButton(const std::string& sprite_path)
{
  ClickableButton* new_button = new ClickableButton(sprite_path);
  popup_buttons.push_back(new_button);
  return *new_button;
}

/* Add a reference of an existing button for the popup */
ClickableButton& PopupWindow::referenceButton(ClickableButton& ref_button)
{
  popup_buttons_referenced.push_back(&ref_button);
  return ref_button;
}

/* Add text to render at a specified position */
void PopupWindow::renderTextAtPosition(const std::string& text,
                                       Vector2 position)
{
  popup_text.push_back(text);
  popup_text_pos.push_back(position);
}

/* Position the close button */
void PopupWindow::positionCloseButton(Vector2 _pos)
{
  close_button->setPos(_pos);
}

/* Get position of close button */
Vector2 PopupWindow::getCloseButtonPos()
{
  return close_button->getPos();
}

/* Show for a period of time */
void PopupWindow::showForTime(float _timeout)
{
  timeout = static_cast<double>(_timeout);
  is_active = true;
  close_button->setActive(false);
}

/* Show popup until user closes it */
void PopupWindow::show()
{
  is_active = true;
  timeout = -1;
  close_button->setActive(true);
}

/* Close popup if opened with show() */
void PopupWindow::hide()
{
  // Set inactive & reset timeout
  is_active = false;
  timeout = -1;

  // Disable all buttons
  close_button->setActive(false);
  for (ClickableButton* button : getInternalButtons())
  {
    button->setActive(false);
  }
}

/* Update the popup */
void PopupWindow::update(const ASGE::GameTime& game_time)
{
  // Don't update anything if inactive
  if (!is_active)
  {
    return;
  }

  // Update buttons
  close_button->update();
  for (ClickableButton* button : popup_buttons)
  {
    button->update();
  }
  for (ClickableButton* button : popup_buttons_referenced)
  {
    button->update();
  }

  // Don't update timer if manually opened
  if (timeout == -1)
  {
    return;
  }

  // Was automatically opened, check for timeout
  timer += game_time.delta.count() / 1000;
  if (timer >= timeout)
  {
    hide();
    timer = 0;
  }
}

/* Render the popup and all contents */
void PopupWindow::render()
{
  // Don't render if inactive
  if (!is_active)
  {
    return;
  }

  // Render popup
  renderer->renderSprite(*sprite->getSprite());

  // Render close button if required
  close_button->render();

  // Render other added buttons
  for (ClickableButton* button : popup_buttons_referenced)
  {
    button->render();
  }
  for (ClickableButton* button : popup_buttons)
  {
    button->render();
  }

  // Render popup contents
  for (ScaledSprite* content : popup_sprites_referenced)
  {
    renderer->renderSprite(*content->getSprite());
  }
  for (ScaledSprite* content : popup_sprites)
  {
    renderer->renderSprite(*content->getSprite());
  }

  // Render currently added text
  int text_index = 0;
  for (std::string& text : popup_text)
  {
    Vector2 position = popup_text_pos.at(text_index);
    renderer->renderText(text,
                         static_cast<int>(position.x),
                         static_cast<int>(position.y),
                         ASGE::COLOURS::WHITE);
    text_index++;
  }
}

/* Clear all render text */
void PopupWindow::clearAllRenderText()
{
  popup_text.clear();
  popup_text_pos.clear();
}