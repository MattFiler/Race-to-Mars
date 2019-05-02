#include "ServiceLocator.h"

ASGE::Renderer* Locator::ref_renderer = nullptr;  //! OCLint
SoLoud::Soloud* Locator::ref_audio = nullptr;     //! OCLint
ASGE::Input* Locator::ref_input = nullptr;        //! OCLint
NetworkConnection* Locator::ref_client = nullptr; //! OCLint
Cursor* Locator::ref_cursor = nullptr;            //! OCLint
Players* Locator::ref_players = nullptr;          //! OCLint

// OCLint your documentation is TRASH, this doesn't suppress anything!