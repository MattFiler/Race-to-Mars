#ifndef PROJECT_CARD_H
#define PROJECT_CARD_H

#include "../Core/ServiceLocator.h"
#include <Engine/Sprite.h>
#include <Engine/Renderer.h>

/*
 *
 * RACE TO SPACE
 * The main cards instance to be used as a parent to all other card types (E.G. issues, objectives)
 *
 */

class Card {
public:
    Card();
    ~Card() = default;

protected:
    ASGE::Renderer* renderer = nullptr;

private:
    /* Stuff here */
};

#endif //PROJECT_CARD_H
