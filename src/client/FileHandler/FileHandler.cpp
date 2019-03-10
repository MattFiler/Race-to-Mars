#include "FileHandler.h"

/* Open the file as a JSON data structure */
json FileHandler::openAsJSON(const std::string& filename)
{
  // Load file into json structure and return
  json json_file;
  auto buffer = openAsBuffer(filename);
  std::stringstream(std::string(buffer.as_char(), buffer.length)) >> json_file;
  return json_file;
}

/* Load a requested portion of a game config */
json FileHandler::loadConfig(const std::string& config,
                             const std::string& request)
{
  // Load our config and assign default values.
  json temp_config = openAsJSON("CONFIGS/" + config);
  return loadConfigFromExisting(temp_config, request, config);
}

/* Load required JSON from an existing JSON object */
json FileHandler::loadConfigFromExisting(json temp_config,
                                         const std::string& request,
                                         const std::string& original_filename)
{
  // TODO: This is an ugly temp fix for DEFAULT enum. Improve!
  std::string default_text = "DEFAULT";
  if (original_filename == "characters_core.json")
  {
    default_text = "0";
  }
  json final_config = temp_config[default_text];

  // If we're requesting default, we can stop here.
  if (request == default_text)
  {
    return final_config;
  }

  // If not, continue to override the selected character's details.
  // Please note: only supports ONE layer of recursion into objects.
  temp_config = temp_config[request];
  for (json::iterator i = temp_config.begin(); i != temp_config.end(); ++i)
  {
    if (!temp_config[i.key()].is_null() && !temp_config[i.key()].is_object())
    {
      final_config[i.key()] = i.value();
    }
    else if (temp_config[i.key()].is_object())
    {
      for (json::iterator x = temp_config[i.key()].begin();
           x != temp_config[i.key()].end();
           ++x)
      {
        if (!temp_config[i.key()][x.key()].is_null())
        {
          final_config[i.key()][x.key()] = x.value();
        }
      }
    }
  }
  return final_config;
}

/* Open the file as a buffer */
std::string FileHandler::openAsString(const std::string& filename)
{
  auto buffer = openAsBuffer(filename);
  return std::string(buffer.as_char(), buffer.length);
}

/* Load a sound into a SoLoud WavStream */
void FileHandler::loadSound(SoLoud::Wav& sound_file,
                            const std::string& filename,
                            float volume,
                            bool looping,
                            bool single_instance)
{
  auto buffer = openAsBuffer("SOUNDS/" + filename + ".wav");
  sound_file.loadMem(buffer.as_unsigned_char(),
                     static_cast<unsigned int>(buffer.length),
                     false,
                     false);
  sound_file.setVolume(volume);
  sound_file.setLooping(looping);
  if (single_instance)
  {
    sound_file.setSingleInstance(1);
  }
}

/* Load a file as a buffer */
ASGE::FILEIO::IOBuffer FileHandler::openAsBuffer(const std::string& filename)
{
  debug_text.print("ACCESSING FILE - " + filename);

  auto file = ASGE::FILEIO::File();
  if (file.open("/data/" + filename))
  {
    auto buffer = file.read();
    if (buffer.length > 0)
    {
      return buffer;
    }
  }
  debug_text.print("FAILED LOADING OF - " + filename);
  throw "An unhandled exception occurred while loading " + filename + ".";
}