#ifndef PROJECT_CONSTANTS_H
#define PROJECT_CONSTANTS_H

enum game_global_scenes
{
  DEFAULT = -1,
  MAIN_MENU = 0,
  IN_GAME,
  QUIT_GAME
};

// Maybe have game sections here? E.G. assigning counters, syncing, etc.
enum game_local_scenes
{
  PLAYING_GAME,
  IS_PAUSED
};

// Resolution (set at runtime)
struct GameResolution
{
 public:
  static int height;
  static int width;
  static float scale;
};

#endif // PROJECT_CONSTANTS_H
