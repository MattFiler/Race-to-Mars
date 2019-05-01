#ifndef PROJECT_TUTORIALSCENE_H
#define PROJECT_TUTORIALSCENE_H

#include "Scene.h"
#include "client/UI/Managers/GameSceneUI.h"
#include <enet/enet.h>
#include <soloud.h>
#include <soloud_wav.h>

class TutorialScene : public Scene
{
public:
    TutorialScene() = default;
    ~TutorialScene() override = default;

    void init() override;
    void networkConnected() override {};
    void networkDisconnected() override {};
    void networkDataReceived(const enet_uint8* data, size_t data_size) override {};

    void keyHandler(const ASGE::SharedEventData data) override;
    void clickHandler(const ASGE::SharedEventData data) override;
    game_global_scenes update(const ASGE::GameTime& game_time) override;
    void render() override;

private:
    SceneUI ui_manager;
    FileHandler file_handler;

    int current_slide = 0;
    SoLoud::Wav active_sfx;
};

#endif //PROJECT_TUTORIALSCENE_H
