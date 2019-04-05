#include "PopupManager.h"

/* Create a new popup with ID */
void PopupManager::createPopup(const std::string& _identifier)
{
  PopupWindow* new_popup = new PopupWindow();
  popup.push_back(new_popup);
  popup_identifier.emplace_back(_identifier);
}

/* Get reference to popup by ID */
PopupWindow* PopupManager::getPopupRef(const std::string& _identifier)
{
  int index = 0;
  for (std::string& id : popup_identifier)
  {
    if (id == _identifier)
    {
      return popup.at(index);
    }
    index++;
  }
  throw "Couldn't find popup with ID of " + _identifier;
}

/* Are any popups active? */
bool PopupManager::anyAreActive()
{
  for (PopupWindow* this_popup : popup)
  {
    if (this_popup->isVisible())
    {
      return true;
    }
  }
  return false;
}

/* Hide all popups */
void PopupManager::hideAll()
{
  for (PopupWindow* this_popup : popup)
  {
    this_popup->hide();
  }
}

/* Show all popups - don't think we'll ever need this */
void PopupManager::showAll()
{
  for (PopupWindow* this_popup : popup)
  {
    this_popup->show();
  }
}

/* Pass out key press info to active popups */
void PopupManager::keyHandler(KeyHandler& keys)
{
  for (PopupWindow* this_popup : popup)
  {
    if (this_popup->isVisible())
    {
      this_popup->keyHandler(keys);
    }
  }
}

/* Trigger click events on active popups */
void PopupManager::clickHandler()
{
  for (PopupWindow* this_popup : popup)
  {
    if (this_popup->isVisible())
    {
      this_popup->clickHandler();
    }
  }
}

/* Update active popups */
void PopupManager::update(const ASGE::GameTime& game_time)
{
  for (PopupWindow* this_popup : popup)
  {
    this_popup->update(game_time);
  }
}

/* Render active popups */
void PopupManager::render()
{
  for (PopupWindow* this_popup : popup)
  {
    this_popup->render();
  }
}