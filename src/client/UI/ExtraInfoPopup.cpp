#include "ExtraInfoPopup.h"

/* Create the popup */
ExtraInfoPopup::ExtraInfoPopup()
{
  renderer = Locator::getRenderer();
  sprite = new ScaledSprite("data/UI/INGAME_UI/popup_blank.png");

  file_handler.loadSound(active_sfx, "popup_show");
  file_handler.loadSound(inactive_sfx, "popup_hide");
}

/* Destroy the popup and all contents */
ExtraInfoPopup::~ExtraInfoPopup()
{
  delete sprite;
  sprite = nullptr;
}

/* Set the popup sprite */
ScaledSprite& ExtraInfoPopup::setSprite(const std::string& popup_name)
{
  delete sprite;
  sprite =
    new ScaledSprite("data/UI/INGAME_UI/EI_POPUP_" + popup_name + ".png");
  return *sprite;
}

/* Show for a period of time */
void ExtraInfoPopup::showForTime(float _timeout)
{
  timeout = static_cast<double>(_timeout);
  is_active = true;
  timer = 0;
}

/* Show popup until user closes it */
void ExtraInfoPopup::show()
{
  Locator::getAudio()->play(active_sfx);
  is_active = true;
  timeout = -1;
}

/* Close popup if opened with show() */
void ExtraInfoPopup::hide()
{
  Locator::getAudio()->play(inactive_sfx);

  // Set inactive & reset timeout
  is_active = false;
  timeout = -1;
}

/* Update the popup */
void ExtraInfoPopup::update(const ASGE::GameTime& game_time)
{
  // Don't update anything if inactive
  if (!is_active)
  {
    return;
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
void ExtraInfoPopup::render()
{
  // Don't render if inactive
  if (!is_active)
  {
    return;
  }

  // Render popup if set
  if (sprite != nullptr)
  {
    renderer->renderSprite(*sprite->getSprite());
  }
}