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

	// This is to create the score
	LabelBMFont* scoreLabel = LabelBMFont::create("0", "bitmapFont.fnt");
	addChild(scoreLabel, 5, kScoreLabel);
	scoreLabel->setPosition(ccp(160, 430));

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
	score = 0;
	_resetPlatforms();
    _resetRocketMan();
}

// RocketMan logic
void GameLayer::_resetRocketMan()
{
	rm_position.x = SCREEN_WIDTH * 0.5f;
	rm_position.y = SCREEN_WIDTH * 0.5f;
	rocketMan->setPosition(rm_position);

	rm_velocity.x = 0;
	rm_velocity.y = 0;

	rm_acceleration.x = 0;

	rm_acceleration.y = -550.0f;

	rm_lookingRight = true;
	rocketMan->setScaleX(1.0f);
}


void GameLayer::update(float dt)
{
	if (gameSuspended)
		return;

	rm_position.x += rm_velocity.x * dt;
	// rm_lookingRight/Left is used to flip RocketMan in the right direction i.e. direction of the velocity
	// so RocketMan does not travel backwards
	if (rm_velocity.x < -30.0f && rm_lookingRight)
	{
		rm_lookingRight = false;

		// what is the point of setting scaleX?
		rocketMan->setScaleX(-1.0f);
	}
	else if (rm_velocity.x > 30.0f && !rm_lookingRight)
	{
		rm_lookingRight = true;
		rocketMan->setScaleX(1.0f);
	}

	Size rm_size = rocketMan->getContentSize();
	float max_x = SCREEN_WIDTH + rm_size.width * 0.5f;
	float min_x = -rm_size.width * 0.5f;

	if (rm_position.x > max_x)
		rm_position.x = min_x;

	if (rm_position.x < min_x)
		rm_position.x = max_x;

	rm_velocity.y += rm_acceleration.y * dt;
	rm_position.y += rm_velocity.y * dt;

	int platformTag;
	if (rm_velocity.y < 0)
	{
		for (platformTag = kPlatformsStartTag; platformTag < kPlatformsStartTag + K_NUM_PLATFORMS; platformTag++)
		{
			Sprite* platform = dynamic_cast<Sprite*>(this->getChildByTag(platformTag));
			Size platform_size = platform->getContentSize();
			Point platform_position = platform->getPosition();

			max_x = platform_position.x - platform_size.width * 0.5f - 10;
			min_x = platform_position.x + platform_size.width * 0.5f + 10;
			float min_y = platform_position.y + (platform_size.height + rm_size.height) * 0.5f - K_PLATFORM_TOP_PADDING;

			// check if RocketMan and the platform is colliding, if so, make the Rocketman jump
			if (rm_position.x > max_x && rm_position.x < min_x &&
				rm_position.y > platform_position.y && rm_position.y < min_y)
				_jump();


			if (rm_position.y < -rm_size.height)
			{
				// TODO: (exit the game here)
			}
		}

	}
	else if (rm_position.y > SCREEN_HEIGHT * 0.5f)
	{
		// If the rocketman is going past half the screen, we move the platforms down
		// and add new platforms at the top to make it feel like the rocketman is moving up
		// Obviously, the platforms cannot go down, can they :)
		float delta = rm_position.y - SCREEN_HEIGHT * 0.5f;
		rm_position.y = SCREEN_HEIGHT * 0.5f;
		currentPlatformY -= delta;

		for (platformTag = kPlatformsStartTag; platformTag < kPlatformsStartTag + K_NUM_PLATFORMS; platformTag++)
		{
			Sprite* platform = dynamic_cast<Sprite*>(this->getChildByTag(platformTag));


			Point position = platform->getPosition();
			position = ccp(position.x, position.y - delta);

			// If the platform just became invisible, reset it to just above the screen
			if (position.y < -platform->getContentSize().height * 0.5f)
			{
				currentPlatformTag = platformTag;
				_resetPlatform();
			}
			else
			{
				// If the platform is still visible, decrease its Y coordinates so it looks like the scene is scrolling
				platform->setPosition(position);
			}
		}

		score += (int)delta;
		__String* scoreStr = __String::createWithFormat("%d", score);
		LabelBMFont* scoreLabel = dynamic_cast<LabelBMFont*>(getChildByTag(kScoreLabel));
		scoreLabel->setString(scoreStr->getCString());
	}

	//// draw RocketMan at its new position
	rocketMan->setPosition(rm_position);
}


// when RocketMan is jumping, this is  its velocity
void GameLayer::_jump()
{
	// play sound effect when player jumps
#if K_PLAY_SOUND_EFFECTS
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("Sounds/jump.wav");
#endif
	rm_velocity.y = 350.0f + fabsf(rm_velocity.x);
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