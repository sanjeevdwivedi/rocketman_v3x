//
//  MainLayer.h
//  RocketManCocos2dx
//
//  Sanjeev Dwivedi: Updated to v3.6-Windows 10 Universal on 5/10/15
//  Modified by Sanjeev Dwivedi, Dale Stammen and Eric Mitelette
//  Created by Carlos Pinan on 13/10/13.
//
//

#ifndef __RocketManCocos2dx__MainLayer__
#define __RocketManCocos2dx__MainLayer__

#include "cocos2d.h"
#include "GameConfig.h"

class MainLayer : public cocos2d::Layer
{
public:
    
    MainLayer();

    static cocos2d::Scene* scene();
	virtual void update(float dt);
    
protected:
    
};

#endif /* defined(__RocketManCocos2dx__MainLayer__) */
