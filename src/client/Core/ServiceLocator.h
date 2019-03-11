#ifndef PROJECT_SERVICELOCATOR_H
#define PROJECT_SERVICELOCATOR_H

#include <Engine/Input.h>
#include <Engine/Sprite.h>
#include <soloud.h>

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
  static SoLoud::Soloud* getAudio() { return ref_audio; };
  static ASGE::Input* getInput() { return ref_input; };

  static void setupRenderer(ASGE::Renderer* inst_renderer)
  {
    ref_renderer = inst_renderer;
  };

  static void setupAudio(SoLoud::Soloud* inst_audio)
  {
    ref_audio = inst_audio;
  };

  static void setupInput(ASGE::Input* inst_input) { ref_input = inst_input; };

 private:
  static ASGE::Renderer* ref_renderer;
  static SoLoud::Soloud* ref_audio;
  static ASGE::Input* ref_input;
};

/* Return a null handler here? Not sure if we want to fail gracefully. */

#endif // PROJECT_SERVICELOCATOR_H