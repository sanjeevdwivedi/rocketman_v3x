//
//  GameLayer.h
//  RocketManCocos2dx
//
//  Sanjeev Dwivedi: Updated to v3.6-Windows 10 Universal on 5/10/15
//  Modified by Sanjeev Dwivedi, Dale Stammen and Eric Mitelette
//  Created by Carlos Pinan on 13/10/13.
//
//

#ifndef __RocketManCocos2dx__GameLayer__
#define __RocketManCocos2dx__GameLayer__
#include <vector>

#include "MainLayer.h"

using namespace cocos2d;
using namespace std;

class GameLayer : public MainLayer
{

public:
    GameLayer();
    virtual ~GameLayer();

    void update(float dt);
    static cocos2d::Scene* scene();

private:
    Point rm_position;
	Vec2  rm_velocity;
	Vec2  rm_acceleration;

	int currentPlatformTag;
	int platformCount;
	float currentPlatformY;
	float currentMaxPlatformStep;
	int currentBonusType;
	int currentBonusPlatformIndex;
	bool rm_lookingRight;
	int score;
	int fuelInTank;

	Sprite *pHealthSprite;
	ProgressTimer *pHealthBar;
	Sprite *rocketMan;
	Animate *jetpackAnimation;
	Sprite *bonus5;
	Sprite *bonus10;
	Sprite *bonus50;
	Sprite *bonus100;

	bool gameSuspended;

	void _initPlatform();
	void _initPlatforms();
	void _resetPlatform();
	void _resetPlatforms();
    void _startGame();
    void _resetRocketMan();
	void _jump();
	void _resetBonus();
	void _superJump();
	void _showHighScores();
	void _initJetPackAnimation();

	virtual void onClickBegan(cocos2d::EventKeyboard::KeyCode key, Event* event);
	virtual void onClickEnded(cocos2d::EventKeyboard::KeyCode key, Event* event);

	void onAcceleration(cocos2d::Acceleration *acc, cocos2d::Event *event);
};

#endif /* defined(__RocketManCocos2dx__GameLayer__) */
