#include "ServiceLocator.h"

#include "client/NetworkConnection/NetworkConnection.h"
#include "client/UI/Cursor.h"
#include <Engine/Input.h>
#include <Engine/Sprite.h>
#include <enetpp/client.h>
#include <soloud.h>

ASGE::Renderer* Locator::ref_renderer = nullptr;
SoLoud::Soloud* Locator::ref_audio = nullptr;
ASGE::Input* Locator::ref_input = nullptr;
NetworkConnection* Locator::ref_client = nullptr;
Cursor* Locator::ref_cursor = nullptr;