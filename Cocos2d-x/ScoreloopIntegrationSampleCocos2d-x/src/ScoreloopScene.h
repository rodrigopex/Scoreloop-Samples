#ifndef __SCORELOOP_SCENE_H__
#define __SCORELOOP_SCENE_H__

#include "cocos2d.h"
#include <bps/dialog.h>
#include <scoreloop/scui_client.h>

class ScoreloopScene : public cocos2d::CCLayer
{
public:
    ScoreloopScene();
    ~ScoreloopScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    /**
    @brief    The function to be called for cards invocation
    @param scuiClient A handle for the current SCUI_Client instance.
    */
    void setSCUIClient(SCUI_Client_h scuiClient);

    // a selector callback: close menu.
    void menuCloseCallback(CCObject* pSender);
    // a selector callback: go to favorite.
    void menuFavoriteCallback(CCObject *pSender);
    // a selector callback: go to leaderboard.
    void menuLeaderboardCallback(CCObject *pSender);
    // a selector callback: go to achievements.
    void menuAchievementsCallback(CCObject *pSender);
    // a selector callback: go to profile page.
    void menuProfileCallback(CCObject *pSender);
    // a selector callback: go to challenges
    void menuChallengesCallback(CCObject *pSender);


    void OnScoreloopUiResultCallback(SCUI_Result_t viewResult, const void *data);

    // implement the "static node()" method manually
    CREATE_FUNC(ScoreloopScene);

private:
    static void StaticOnScoreloopUiResultCallback(void *cookie, SCUI_Result_t viewResult, const void *data);

    void setButtonsEnabled(bool value);
    void showError(SC_Error_t errorCode);

private:
    SCUI_Client_h _scuiClient;
    cocos2d::CCMenuItem *_favoriteMenuItem;
    cocos2d::CCMenuItem *_leaderboardMenuItem;
    cocos2d::CCMenuItem *_achievementsMenuItem;
    cocos2d::CCMenuItem *_challengesMenuItem;
    cocos2d::CCMenuItem *_profileMenuItem;
    dialog_instance_t _dialog;
};

#endif // __SCORELOOP_SCENE_H__
