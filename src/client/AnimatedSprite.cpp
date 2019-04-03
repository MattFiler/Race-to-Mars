//
// Created by h2-sollis on 03/04/2019.
//

#include "AnimatedSprite.h"

AnimatedSprite::AnimatedSprite()
{

}

AnimatedSprite::~AnimatedSprite()
{
    delete sprite;
    sprite = nullptr;
}

void AnimatedSprite::setNewWidth(float new_width)
{
    sprite->scale(1);
    sprite->width(new_width * GameResolution::scale);
    bounding_box.width = sprite->width();
}

void AnimatedSprite::setNewHeight(float new_height)
{
    sprite->scale(1);
    sprite->height(new_height * GameResolution::scale);
    bounding_box.height = sprite->height();
}

void AnimatedSprite::setNewLoc(Vector2 new_loc)
{
    sprite->xPos(new_loc.x * GameResolution::scale);
    sprite->yPos(new_loc.y * GameResolution::scale);
    bounding_box.pos = Vector2(sprite->xPos(), sprite->yPos());
}