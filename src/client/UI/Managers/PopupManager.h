#ifndef PROJECT_POPUPMANAGER_H
#define PROJECT_POPUPMANAGER_H

/*
 *
 * This class will be used to group all required popups for one scene. The idea
 * being that it will cut down on conditional logic in game scenes that requires
 * manually adding a new popup instance to check if any popups are open, etc. It
 * will also handle the rendering of all available and active popups, and the
 * ordering of them when required. Much better than the current implementation!
 *
 */

#include "../PopupWindow.h"
#include <vector>

class PopupManager
{
 public:
  PopupManager() = default;
  ~PopupManager();

  void createPopup(const std::string& _identifier);
  PopupWindow* getPopupRef(const std::string& _identifier);

  bool anyAreActive();

  void hideAll();
  void showAll();

  void keyHandler(KeyHandler& keys);
  void clickHandler();

  void update(const ASGE::GameTime& game_time);
  void render();

 private:
  std::vector<std::string> popup_ids;
  std::vector<PopupWindow*> popups;
};

#endif // PROJECT_POPUPMANAGER_H
