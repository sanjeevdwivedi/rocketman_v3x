//
//  HighScoreLayer.cpp
//  RocketManCocos2dx
//
//  Modified by Sanjeev Dwivedi, Dale Stammen and Eric Mitelette
//  Created by Carlos Pinan on 13/10/13.
//
//

#include "HighScoreLayer.h"
#include "GameLayer.h"
#include "cocos-ext.h"


using namespace cocos2d;
using namespace extension;
using namespace std;

Scene* HighScoreLayer::scene(int lastScore)
{
    Scene* scene = Scene::create();
    HighScoreLayer* layer = new HighScoreLayer(lastScore);
    scene->addChild(layer);
    layer->autorelease();
    return scene;
}


HighScoreLayer::HighScoreLayer(int lastScore) : currentScore(lastScore)
{
    _loadCurrentPlayer();
    _loadHighScores();
    _updateHighScores();

    Sprite* title = Sprite::createWithSpriteFrameName("high_scores.png");
    this->addChild(title, 5);
    title->setPosition(ccp(160, 420));

    float start_y = 360.0f;
    float step = 27.0f;
    int count = 0;

    //
    UserDefault* defaults = UserDefault::sharedUserDefault();
    string highscores = defaults->getStringForKey("highscores", "");
    vector<string> vectorScores = split(highscores, "\n");
    if (vectorScores.size() > 0)
    {
        for (unsigned int i = 0; i < vectorScores.size(); i++)
        {
            vector<string> data = split(vectorScores[i], ";");
            if (data.size() > 0)
            {
                string player = data[0];
                int score = atoi(data[1].c_str());

                LabelTTF* label1 = LabelTTF::create(CCString::createWithFormat("%d", (count + 1))->getCString(), "Arial", 14, CCSizeMake(30, 40), kCCTextAlignmentRight, kCCVerticalTextAlignmentCenter);
                addChild(label1, 5);
                label1->setColor(Color3B::WHITE);
                label1->setOpacity(200);
                label1->setPosition(ccp(15, start_y - count * step - 2.0f));

                LabelTTF* label2 = LabelTTF::create(player.c_str(), "Arial", 16, CCSizeMake(240, 40), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter);
                addChild(label2, 5);
                label2->setColor(Color3B::WHITE);
                label2->setPosition(ccp(160, start_y - count * step));

                LabelTTF* label3 = LabelTTF::create(CCString::createWithFormat("%d", score)->getCString(), "Arial", 16, CCSizeMake(290, 40), kCCTextAlignmentRight, kCCVerticalTextAlignmentCenter);
                addChild(label3, 5);
                label3->setColor(Color3B::WHITE);
                label3->setOpacity(200);
                label3->setPosition(ccp(160, start_y - count * step));

                count++;
                if (count == 10)
                    break;

            }
        }
    }

    auto playAgainButtonOn = Sprite::createWithSpriteFrameName("playAgainButton.png");
    auto playAgainButtonOff = Sprite::createWithSpriteFrameName("playAgainButton.png");

    auto playAgainItem = MenuItemSprite::create(playAgainButtonOff, playAgainButtonOn, CC_CALLBACK_1(HighScoreLayer::button1Callback, this));

    auto mainMenu = Menu::create(playAgainItem, nullptr);
    mainMenu->alignItemsVerticallyWithPadding(9.0f);
    mainMenu->setPosition(ccp(160, 58));
    this->addChild(mainMenu);
}

void HighScoreLayer::_loadCurrentPlayer()
{
    currentPlayer = "";
    currentPlayer = UserDefault::sharedUserDefault()->getStringForKey("player", "anonymous");
}

void HighScoreLayer::_loadHighScores()
{
    UserDefault* defaults = UserDefault::sharedUserDefault();

#ifdef RESET_DEFAULTS
    defaults->setStringForKey("highscores", "");
    defaults->flush();
#endif

    string highscores = defaults->getStringForKey("highscores", "");

    if (highscores.length() == 0)
    {
        string save_scores = "";
        save_scores += "RocketMan;750000\n";
        save_scores += "RocketMan;500000\n";
        save_scores += "RocketMan;250000\n";
        save_scores += "RocketMan;100000\n";
        save_scores += "RocketMan;50000\n";
        save_scores += "RocketMan;20000\n";
        save_scores += "RocketMan;10000\n";
        save_scores += "RocketMan;5000\n";
        save_scores += "RocketMan;1000\n";
        defaults->setStringForKey("highscores", save_scores);
        defaults->flush();
    }
}

void HighScoreLayer::_updateHighScores()
{
    UserDefault* defaults = UserDefault::sharedUserDefault();

    currentScorePosition = -1;
    int count = 0;
    string highscores = defaults->getStringForKey("highscores", "");
    if (highscores.length() > 0)
    {
        vector<string> vectorScores = split(highscores, "\n");
        if (vectorScores.size() > 0)
        {
            for (unsigned int i = 0; i < vectorScores.size(); i++)
            {
                vector<string> data = split(vectorScores[i], ";");
                if (data.size() > 0)
                {
                    int score = atoi(data[1].c_str());
                    if (currentScore >= score)
                    {
                        currentScorePosition = count;
                        break;
                    }
                    count++;
                }
            }
        }

        if (currentScorePosition >= 0)
        {
            highscores = defaults->getStringForKey("highscores", "");
            vectorScores = split(highscores, "\n");
            CCString* dataScore = CCString::createWithFormat("%s;%d", currentPlayer.c_str(), currentScore);

            string tmpScore = "";
            for (int i = vectorScores.size() - 2; i > currentScorePosition; i--)
                vectorScores[i + 1] = vectorScores[i];

            vectorScores[currentScorePosition] = string(dataScore->getCString());
            string scores = "";
            for (unsigned int i = 0; i < vectorScores.size(); i++)
                scores += vectorScores[i] + "\n";

            defaults->setStringForKey("highscores", scores);
            defaults->flush();
        }

    }

}

vector<string> HighScoreLayer::split(string str, string value)
{
    vector<string> split;
    char *line = strtok(const_cast<char*>(str.c_str()), value.c_str());
    while (line) {
        split.push_back(line);
        line = strtok(NULL, value.c_str());
    }
    return split;
}

void HighScoreLayer::_saveCurrentPlayer()
{
    UserDefault* defaults = UserDefault::sharedUserDefault();
    defaults->setStringForKey("player", currentPlayer);
    defaults->flush();
}

void HighScoreLayer::draw(Renderer *renderer, const kmMat4& transform, uint32_t flags)
{
	_customCommand.init(_globalZOrder + 10);
	_customCommand.func = CC_CALLBACK_0(HighScoreLayer::onDraw, this, transform, flags);
	renderer->addCommand(&_customCommand);
}
void HighScoreLayer::onDraw(const kmMat4 &transform, bool transformUpdated)
{
	if (currentScorePosition < 0)
		return;

	float width = 320.0f;
	float height = 27.0f;
	float x = (320.0f - width) / 2.0f;
	float y = 348 - currentScorePosition * height;
    DrawPrimitives::drawSolidRect(Vec2(x, y), Vec2(x + width, y + height), ccc4f(1.0f, 0.0f, 0.0f, 0.4f));
}

void HighScoreLayer::button1Callback(CCObject *pSender)
{
    TransitionFade* scene = TransitionFade::create(0.5f, GameLayer::scene(), Color3B::WHITE);
    Director::getInstance()->replaceScene(scene);
}


