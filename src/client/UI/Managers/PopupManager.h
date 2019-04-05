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

class PopupManager
{
 public:
  PopupManager();
  ~PopupManager();

 private:
};

#endif // PROJECT_POPUPMANAGER_H
