#ifndef PROJECT_SERVICELOCATOR_H
#define PROJECT_SERVICELOCATOR_H

#include <Engine/Sprite.h>

/*
 *
 * RACE TO SPACE
 * Our service locator to delegate common functionalities (E.G. the renderer).
 *
 */

class Locator
{
 public:
  static ASGE::Renderer* getRenderer() { return ref_renderer; };

  static void setupRenderer(ASGE::Renderer* inst_renderer)
  {
    ref_renderer = inst_renderer;
  };

 private:
  static ASGE::Renderer* ref_renderer;
};

/* Return a null handler here? Not sure if we want to fail gracefully. */

#endif // PROJECT_SERVICELOCATOR_H
