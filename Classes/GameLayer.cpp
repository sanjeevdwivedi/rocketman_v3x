//
//  GameLayer.cpp
//  RocketManCocos2dx
//
//  Sanjeev Dwivedi: Updated to v3.6-Windows 10 Universal on 5/10/15
//  Modified by Sanjeev Dwivedi, Dale Stammen and Eric Mitelette
//  Created by Carlos Pinan on 13/10/13.
//
//

#include "GameLayer.h"
#include "SimpleAudioEngine.h"

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

	gameSuspended = true;

	// Initialize all the platforms
	_initPlatforms();

    _initJetPackAnimation();
    rocketMan = Sprite::create();
    rocketMan->runAction(jetpackAnimation);
    this->addChild(rocketMan, 4, kRocketMan);

    _startGame();

	// run the update loop
	scheduleUpdate();

	// We don't want touch interaction
	setTouchEnabled(false);

	//setAccelerometerEnabled(true);
	Device::setAccelerometerEnabled(true);
	auto listener = EventListenerAcceleration::create(CC_CALLBACK_2(GameLayer::onAcceleration, this));
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

#if K_PLAY_BACKGROUND_MUSIC
	// play and loop background music during game
	auto soundEngine = CocosDenshion::SimpleAudioEngine::sharedEngine();
	soundEngine->playBackgroundMusic("Sounds/background.wav", true);
	soundEngine->setBackgroundMusicVolume(K_PLAY_BACKGROUND_MUSIC_VOLUME);
#endif
}

void GameLayer::onAcceleration(cocos2d::Acceleration *pAccelerationValue, cocos2d::Event *event)
{
	if (gameSuspended)
		return;

	// RocketMan's acceleration, left and right
	float accel_filter = 0.1f;

	// bug in Cocos2d-x, on this device it is inverting the axes and directions
	//rm_velocity.x = rm_velocity.x * accel_filter + pAccelerationValue->x * (1.0f - accel_filter) * 500.0f;
	rm_velocity.x = rm_velocity.x * accel_filter + -1*pAccelerationValue->y * (1.0f - accel_filter) * 500.0f;
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
	CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
	CocosDenshion::SimpleAudioEngine::sharedEngine()->stopAllEffects();
}


void GameLayer::_startGame()
{
	_resetPlatforms();
    _resetRocketMan();
}

// RocketMan logic
void GameLayer::_resetRocketMan()
{
	rm_position.x = SCREEN_WIDTH * 0.5f;
	rm_position.y = SCREEN_HEIGHT * 0.5f;
	rocketMan->setPosition(rm_position);

	rm_velocity.x = 0;
	rm_velocity.y = 0;

	rm_acceleration.x = 0;

	//rm_acceleration.y = -550.0f;

	rm_lookingRight = true;
	rocketMan->setScaleX(1.0f);
}


void GameLayer::update(float dt)
{
	if (rm_position.x <= 0) rm_position.x = 0;
	if (rm_position.x >= 320) rm_position.x = 320;

	//// draw RocketMan at its new position
	rm_position.x += rm_velocity.x * dt;
	rocketMan->setPosition(rm_position);
}

void GameLayer::_initPlatform()
{
	Rect rect;
	Sprite* platform;
	switch ((int)(CCRANDOM_0_1() * 2))
	{
	case 0:
		platform = Sprite::createWithSpriteFrameName("truss.png");
		break;
	case 1:
		platform = Sprite::createWithSpriteFrameName("meteoroid.png");
		break;

	default:
		return;
	}
	this->addChild(platform, 3, currentPlatformTag);
}

void GameLayer::_initPlatforms()
{
	currentPlatformTag = kPlatformsStartTag;
	while (currentPlatformTag < kPlatformsStartTag + K_NUM_PLATFORMS)
	{
		_initPlatform();
		currentPlatformTag++;
	}
}
void GameLayer::_resetPlatform()
{
	if (currentPlatformY < 0)
		currentPlatformY = 30.0f;
	else
	{
		currentPlatformY += CCRANDOM_0_1() * (int)(currentMaxPlatformStep - K_MIN_PLATFORM_STEP) + K_MIN_PLATFORM_STEP;
		if (currentMaxPlatformStep < K_MAX_PLATFORM_STEP)
			currentMaxPlatformStep += 0.5f;
	}

	Sprite* platform = dynamic_cast<Sprite*>(this->getChildByTag(currentPlatformTag));

	if (CCRANDOM_0_1() * 2 == 1)
		platform->setScaleX(-1.0f);

	float x;

	Size size = platform->getContentSize();

	if (currentPlatformY == 30.0f)
		x = SCREEN_WIDTH * 0.5f;
	else
		x = CCRANDOM_0_1() * (SCREEN_WIDTH - (int)size.width) + size.width * 0.5f;

	platform->setPosition(ccp(x, currentPlatformY));
	platformCount++;

	if (platformCount == currentBonusPlatformIndex && platformCount != K_MAX_PLATFORMS_IN_GAME)
	{
		Sprite* bonus = dynamic_cast<Sprite*>(this->getChildByTag(kBonusStartTag + currentBonusType));
		bonus->setPosition(ccp(x, currentPlatformY + 30));
		bonus->setVisible(true);
	}
}

void GameLayer::_resetPlatforms()
{
	currentPlatformY = -1;
	currentPlatformTag = kPlatformsStartTag;
	currentMaxPlatformStep = 60.0f;
	currentBonusPlatformIndex = 0;
	currentBonusType = 0;
	platformCount = 0;

	while (currentPlatformTag < kPlatformsStartTag + K_NUM_PLATFORMS)
	{
		_resetPlatform();
		currentPlatformTag++;
	}

	gameSuspended = false;
}