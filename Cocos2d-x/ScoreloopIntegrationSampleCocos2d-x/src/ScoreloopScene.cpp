/*
* Copyright (c) 2012 Research In Motion Limited.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "ScoreloopScene.h"
#include "AppDelegate.h"

#include <scoreloop/sc_client.h>
#include <scoreloop/sc_scores_controller.h>

USING_NS_CC;

ScoreloopScene::ScoreloopScene() :
		_scuiClient(NULL),
        _favoriteMenuItem(NULL),
        _leaderboardMenuItem(NULL),
        _achievementsMenuItem(NULL),
        _challengesMenuItem(NULL),
        _profileMenuItem(NULL),
        _dialog(NULL)
{
}

ScoreloopScene::~ScoreloopScene()
{
	if (_dialog) {
		dialog_destroy(_dialog);
	}
}

static CCMenuItemLabel* createMenuItem(const char* text, CCObject *target, SEL_MenuHandler selector)
{
	CCMenuItemLabel *pMenuItemLabel;
	CCLabelTTF* pLabel;

    pLabel = CCLabelTTF::create(text, "Arial", 64);
    pLabel->setDimensions(CCSize(pLabel->getDimensions().width, 100));
    pMenuItemLabel = CCMenuItemLabel::create(pLabel, target, selector);

    return pMenuItemLabel;
}

// on "init" you need to initialize your instance
bool ScoreloopScene::init()
{
	CCLabelTTF* pLabel;

	//////////////////////////////
    // super init first
    if (!CCLayer::init()) {
        return false;
    }

	//////////////////////////////
    // create menu items for DefaultUI views
    _favoriteMenuItem = createMenuItem("Favorite Game", this, menu_selector(ScoreloopScene::menuFavoriteCallback));

    _leaderboardMenuItem = createMenuItem("Leaderboards", this, menu_selector(ScoreloopScene::menuLeaderboardCallback));

    _achievementsMenuItem = createMenuItem("Achievements", this, menu_selector(ScoreloopScene::menuAchievementsCallback));

    _challengesMenuItem = createMenuItem("Challenges", this, menu_selector(ScoreloopScene::menuChallengesCallback));

    _profileMenuItem = createMenuItem("Edit Profile", this, menu_selector(ScoreloopScene::menuProfileCallback));

    // create menu item that will close the application
    CCMenuItemLabel *closeMenuItem = createMenuItem("Close", this, menu_selector(ScoreloopScene::menuCloseCallback));

	//////////////////////////////
    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(_favoriteMenuItem, _leaderboardMenuItem, _achievementsMenuItem, _challengesMenuItem, _profileMenuItem, closeMenuItem, NULL);
    pMenu->alignItemsVertically();

    // add menu as a child to this layer
    this->addChild(pMenu, 1);

    // disable buttons until SCUI_Client is provided
    this->setButtonsEnabled(false);

    return true;
}

void ScoreloopScene::setSCUIClient(SCUI_Client_h scuiClient)
{
	_scuiClient = scuiClient;
	setButtonsEnabled(true);
}

void ScoreloopScene::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void ScoreloopScene::menuFavoriteCallback(CCObject *pSender)
{
    /////////////////////////////
    // request show DefaultUI - Favorite card for current game...

    setButtonsEnabled(false);

    SC_Error_t retCode = SCUI_Client_ShowFavoritingView(_scuiClient);
    if (retCode) {
        showError(retCode);
        setButtonsEnabled(true);
    }
}

void ScoreloopScene::menuLeaderboardCallback(CCObject *pSender)
{
    /////////////////////////////
    // request show DefaultUI - Leaderboard card for current game...
    setButtonsEnabled(false);

    SC_Error_t retCode = SCUI_Client_ShowLeaderboardView(_scuiClient, 0, SCUI_LEADERBOARD_TYPE_GLOBAL, NULL);
    if (retCode) {
        showError(retCode);
        setButtonsEnabled(true);
    }
}

void ScoreloopScene::menuAchievementsCallback(CCObject *pSender)
{
    /////////////////////////////
    // request show DefaultUI - Achievements card for current game...
    setButtonsEnabled(false);

    SC_Error_t retCode = SCUI_Client_ShowAchievementsView(_scuiClient);
    if (retCode) {
        showError(retCode);
        setButtonsEnabled(true);
    }
}

void ScoreloopScene::menuChallengesCallback(CCObject *pSender)
{
    /////////////////////////////
    // request show DefaultUI - Achievements card for current game...
    setButtonsEnabled(false);

    SC_Error_t retCode = SCUI_Client_ShowChallengesView(_scuiClient);
    if (retCode) {
        showError(retCode);
        setButtonsEnabled(true);
    }
}

void ScoreloopScene::menuProfileCallback(CCObject *pSender)
{
    /////////////////////////////
    // request show DefaultUI - User profile card for current game...

    setButtonsEnabled(false);

    SC_Error_t retCode = SCUI_Client_ShowUserProfileView(_scuiClient);
    if (retCode) {
        showError(retCode);
        setButtonsEnabled(true);
    }
}

void ScoreloopScene::StaticOnScoreloopUiResultCallback(void *cookie, SCUI_Result_t viewResult, const void *data)
{
    ScoreloopScene *this_ = (ScoreloopScene *) cookie;
    this_->OnScoreloopUiResultCallback(viewResult, data);
}

void ScoreloopScene::OnScoreloopUiResultCallback(SCUI_Result_t viewResult, const void *data)
{
    /////////////////////////////
    // request of DefaultUI completed - card dismissed, say "thank you" to the player...

	if (_dialog) {
		dialog_destroy(_dialog);
	}
    dialog_create_toast(&_dialog);
    dialog_set_toast_message_text(_dialog, SCUI_MapResultCodeToStr(viewResult));
    dialog_show(_dialog);

    setButtonsEnabled(true);
}

void ScoreloopScene::setButtonsEnabled(bool value)
{
    _favoriteMenuItem->setEnabled(value);
    _leaderboardMenuItem->setEnabled(value);
    _achievementsMenuItem->setEnabled(value);
    _challengesMenuItem->setEnabled(value);
    _profileMenuItem->setEnabled(value);
}

void ScoreloopScene::showError(SC_Error_t errorCode)
{
    char buffer[256];

    /////////////////////////////
    // show alert dialog with error code

    if (_dialog) {
		dialog_destroy(_dialog);
	}

    dialog_create_alert(&_dialog);
    dialog_set_title_text(_dialog, "Error");
    snprintf(buffer, sizeof(buffer), "View invocation error: %s!", SC_MapErrorToStr(errorCode));
    dialog_set_alert_message_text(_dialog, buffer);
    dialog_add_button(_dialog, "OK", true, NULL, true);
    dialog_show(_dialog);
}
