#ifndef PROJECT_SERVICELOCATOR_H
#define PROJECT_SERVICELOCATOR_H

#include <stdexcept>

class Cursor;
class NetworkConnection;
namespace ASGE
{
class Renderer;
class Input;
}
namespace SoLoud
{
class Soloud;
}
struct Players;

/*
 *
 * RACE TO SPACE
 * Our service locator to delegate common functionalities (E.G. the renderer).
 *
 */

class Locator
{
 public:
  static ASGE::Renderer* getRenderer()
  {
    if (ref_renderer == nullptr)
    {
      throw std::runtime_error("Renderer was called before it was "
                               "initialised.");
    }
    return ref_renderer;
  };
  static SoLoud::Soloud* getAudio()
  {
    if (ref_audio == nullptr)
    {
      throw std::runtime_error("Audio was called before it was initialised.");
    }
    return ref_audio;
  };
  static ASGE::Input* getInput()
  {
    if (ref_input == nullptr)
    {
      throw std::runtime_error("Input was called before it was initialised.");
    }
    return ref_input;
  };
  static NetworkConnection* getNetworkInterface()
  {
    if (ref_client == nullptr)
    {
      throw std::runtime_error("Network data was called before it was "
                               "initialised.");
    }
    return ref_client;
  };
  static Cursor* getCursor()
  {
    if (ref_cursor == nullptr)
    {
      throw std::runtime_error("Cursor was called before it was initialised.");
    }
    return ref_cursor;
  };
  static Players* getPlayers()
  {
    if (ref_players == nullptr)
    {
      throw std::runtime_error("Players were called before they were "
                               "initialised.");
    }
    return ref_players;
  };

  static void setupRenderer(ASGE::Renderer* inst_renderer)
  {
    ref_renderer = inst_renderer;
  };
  static void setupAudio(SoLoud::Soloud* inst_audio)
  {
    ref_audio = inst_audio;
  };
  static void setupInput(ASGE::Input* inst_input) { ref_input = inst_input; };
  static void setupNetworkInterface(NetworkConnection* inst_client)
  {
    ref_client = inst_client;
  };
  static void setupCursor(Cursor* inst_cursor) { ref_cursor = inst_cursor; }
  static void setupPlayers(Players* inst_players)
  {
    ref_players = inst_players;
  }

 private:
  static ASGE::Renderer* ref_renderer;
  static SoLoud::Soloud* ref_audio;
  static ASGE::Input* ref_input;
  static NetworkConnection* ref_client;
  static Cursor* ref_cursor;
  static Players* ref_players;
};

/* Return a null handler here? Not sure if we want to fail gracefully. */

#endif // PROJECT_SERVICELOCATOR_H
