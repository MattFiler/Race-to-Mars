#include "ServiceLocator.h"

ASGE::Renderer* Locator::ref_renderer = nullptr;
SoLoud::Soloud* Locator::ref_audio = nullptr;
ASGE::Input* Locator::ref_input = nullptr;
NetworkConnection* Locator::ref_client = nullptr;
Cursor* Locator::ref_cursor = nullptr;
Players* Locator::ref_players = nullptr;