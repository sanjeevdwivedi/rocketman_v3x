//
//  HighScoreLayer.h
//  RocketManCocos2dx
//
//  Modified by Sanjeev Dwivedi, Dale Stammen and Eric Mitelette
//  Created by Carlos Pinan on 13/10/13.
//
//

#ifndef __RocketManCocos2dx__HighScoreLayer__
#define __RocketManCocos2dx__HighScoreLayer__

#include "MainLayer.h"
using namespace cocos2d;

class HighScoreLayer : public MainLayer
{
public:
    static cocos2d::Scene* scene(int lastScore);
    HighScoreLayer(int lastScore);
	CustomCommand _customCommand;

	virtual void draw(Renderer *renderer, const kmMat4& transform, uint32_t flags);

	virtual void onDraw(const kmMat4 &transform, bool transformUpdated);


private:
    cocos2d::LayerColor* _playerPopup;
    std::string currentPlayer;
    int currentScore;
    int currentScorePosition;

private:
    std::vector<std::string> split(std::string str, std::string value);
    void _loadCurrentPlayer();
    void _loadHighScores();
    void _updateHighScores();
    void _saveCurrentPlayer();

    void button1Callback(cocos2d::Object* pSender);

};

#endif /* defined(__RocketManCocos2dx__HighScoreLayer__) */