//
//  GameLayer.cpp
//  RocketManCocos2dx
//
//  Modified by Sanjeev Dwivedi, Dale Stammen and Eric Mitelette
//  Created by Carlos Pinan on 13/10/13.
//
//

#include "GameLayer.h"

using namespace cocos2d;

Scene* GameLayer::scene()
{
    Scene* scene = Scene::create();
    GameLayer* gameLayer = new GameLayer();
    gameLayer->autorelease();
    scene->addChild(gameLayer);
    return scene;
}

GameLayer::GameLayer()
{

    _initJetPackAnimation();
    rocketMan = Sprite::create();
    rocketMan->runAction(jetpackAnimation);
    this->addChild(rocketMan, 4, kRocketMan);

    _startGame();

}


void GameLayer::_initJetPackAnimation()
{
	Animation* animation;
	animation = Animation::create();
	SpriteFrame * frame;
	int i;
	for (i = 1; i <= 3; i++) {
		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(String::createWithFormat("rocketman_flying_%i.png", i)->getCString());
		animation->addSpriteFrame(frame);
	}

	animation->setDelayPerUnit(0.2f / 3.0f);
	animation->setRestoreOriginalFrame(false);
	animation->setLoops(-1);
	jetpackAnimation = Animate::create(animation);
	jetpackAnimation->retain();
}


GameLayer::~GameLayer()
{
}


void GameLayer::_startGame()
{
    _resetRocketMan();
}

// RocketMan logic
void GameLayer::_resetRocketMan()
{
    rm_position.x = SCREEN_WIDTH * 0.5f;
    rm_position.y = SCREEN_HEIGHT * 0.5f;
    rocketMan->setPosition(rm_position);
}


void GameLayer::update(float dt)
{
    
}

