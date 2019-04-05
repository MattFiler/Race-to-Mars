#ifndef PROJECT_INTERFACE_SOLOUD_H
#define PROJECT_INTERFACE_SOLOUD_H

#include <soloud.h>

/* The idea here is that this will replace Soloud::Soloud, the same for other
 * interfaces */

class Interface_Soloud
{
 public:
  Interface_Soloud() = default;
  ~Interface_Soloud() = default;

  void play();
  void stop();

 private:
  SoLoud::Soloud audio;
};

#endif // PROJECT_INTERFACE_SOLOUD_H
