#include "ServiceLocator.h"

ASGE::Renderer* Locator::ref_renderer = nullptr;  //! OCLINT
SoLoud::Soloud* Locator::ref_audio = nullptr;     //! OCLINT
ASGE::Input* Locator::ref_input = nullptr;        //! OCLINT
NetworkConnection* Locator::ref_client = nullptr; //! OCLINT
Cursor* Locator::ref_cursor = nullptr;            //! OCLINT
Players* Locator::ref_players = nullptr;          //! OCLINT