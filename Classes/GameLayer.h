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
    cocos2d::Point rm_position;
    
	Sprite *rocketMan;
	Animate *jetpackAnimation;

    void _startGame();
    void _resetRocketMan();

	

	void _initJetPackAnimation();
};

#endif /* defined(__RocketManCocos2dx__GameLayer__) */
