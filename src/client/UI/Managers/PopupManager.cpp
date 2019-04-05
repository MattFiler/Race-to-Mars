#include "PopupManager.h"

/* Destroy */
PopupManager::~PopupManager()
{
  popups.clear();
  popup_ids.clear();
}

/* Create a new popup with ID */
void PopupManager::createPopup(const std::string& _identifier)
{
  PopupWindow* new_popup = new PopupWindow();
  popups.push_back(new_popup);
  popup_ids.emplace_back(_identifier);
}

/* Get reference to popup by ID */
PopupWindow* PopupManager::getPopupRef(const std::string& _identifier)
{
  int index = 0;
  for (std::string& id : popup_ids)
  {
    if (id == _identifier)
    {
      return popups.at(index);
    }
    index++;
  }
  throw "Couldn't find popup with ID of " + _identifier;
}

/* Are any popups active? */
bool PopupManager::anyAreActive()
{
  for (PopupWindow* this_popup : popups)
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
  for (PopupWindow* this_popup : popups)
  {
    this_popup->hide();
  }
}

/* Show all popups - don't think we'll ever need this */
void PopupManager::showAll()
{
  for (PopupWindow* this_popup : popups)
  {
    this_popup->show();
  }
}

/* Pass out key press info to active popups */
void PopupManager::keyHandler(KeyHandler& keys)
{
  for (PopupWindow* this_popup : popups)
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
  for (PopupWindow* this_popup : popups)
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
  for (PopupWindow* this_popup : popups)
  {
    this_popup->update(game_time);
  }
}

/* Render active popups */
void PopupManager::render()
{
  for (PopupWindow* this_popup : popups)
  {
    this_popup->render();
  }
}