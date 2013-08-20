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

#include "AppDelegate.h"
#include "ScoreloopScene.h"

#include <bps/dialog.h>

/* These are the game id and game secret from the SL Oxygen game - use your credentials instead */
#define GAME_ID         "d8be0ab4-9a41-4a26-b956-f5af6a305fca"
#define GAME_SECRET     "FsTKEQEN6GnBqao/6PKc1rKuPhA8PxwWvtxqJAtQD6suu9kZcssm4g=="
#define GAME_VERSION    "1.0"
#define GAME_CURRENCY   "AAZ"
#define GAME_LANGUAGE   "en"

USING_NS_CC;

AppDelegate::AppDelegate()
: _dialog(NULL), _lastSessionState(SC_SESSION_INITIAL), _scClient(NULL), _scuiClient(NULL), _scoreloopScene(NULL), _didDeclineScoreloop(false)
{
}

AppDelegate::~AppDelegate() 
{
	// Scoreloop release calls are NULL pointer safe
	SCUI_Client_Release(_scuiClient);
	SC_Client_Release(_scClient);

    if (_dialog) {
        dialog_destroy(_dialog);
    }
}

void AppDelegate::InitializeScoreloop()
{
	SC_Error_t retCode = SC_OK;

	do {
		// initialize init-data structure
		SC_InitData_Init(&_scInitData);

		// create the Scoreloop Core client - here for game SL Oxygen
		retCode = SC_Client_New(&_scClient, &_scInitData,
				GAME_ID, GAME_SECRET, GAME_VERSION, GAME_CURRENCY, GAME_LANGUAGE);
		if (retCode != SC_OK) {
			if (retCode == SC_PAL_INITIALIZATION_FAILED || retCode == SC_DISABLED_DUE_TO_CORPORATE_LIABLE_MODE) {
				_didDeclineScoreloop = true;
			}
			break;
		}

		// now create the Scoreloop DefaultUI client
		retCode = SCUI_Client_NewWithCallback(&_scuiClient, _scClient, StaticOnScoreloopUiResultCallback, this);
		if (retCode != SC_OK) {
			break;
		}

		// we want to get view events
		retCode = SCUI_Client_SetViewEventCallback(_scuiClient, StaticOnScoreloopUIRequestCompletion, this);
		if (retCode != SC_OK) {
			break;
		}

		// the most important - redirect BPS events to the SCUI_Client:
		CCEGLView::sharedOpenGLView()->setEventHandler(this);

		// provide initialized SCUI_Client instance to ScoreloopScene so that it can open the Default UI
		_scoreloopScene->setSCUIClient(_scuiClient);
	}
	while (false);

	// Cleanup on error
	if (retCode != SC_OK) {
		// Scoreloop release calls are NULL pointer safe
		SCUI_Client_Release(_scuiClient); _scuiClient = NULL;
		SC_Client_Release(_scClient); _scClient = NULL;
	}
}

void AppDelegate::StaticOnScoreloopUiResultCallback(void *cookie, SCUI_Result_t viewResult, const void *data)
{
    AppDelegate *appDelegate = (AppDelegate*) cookie;

    appDelegate->_scoreloopScene->OnScoreloopUiResultCallback(viewResult, data);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    CCDirector *pDirector = CCDirector::sharedDirector();

    pDirector->setOpenGLView(CCEGLView::sharedOpenGLView());

    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    CCScene *pScene = CCScene::create();

    // 'layer' is an autorelease object
    _scoreloopScene = ScoreloopScene::create();

    // add layer as a child to scene
    pScene->addChild(_scoreloopScene);

    // run
    pDirector->runWithScene(pScene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    CCDirector::sharedDirector()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    CCDirector::sharedDirector()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();

    // schedule call to applicationDidShow after first swap-buffers call (should happen at least after 1/20 second)
    if (!(_scuiClient || _didDeclineScoreloop)) {
        CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(AppDelegate::applicationDidShow), this, 1.0f, 0, 0.05f, false);
    }
}

bool AppDelegate::HandleBPSEvent(bps_event_t* event)
{
    if (_scuiClient != NULL) {
        if (SC_HandleBPSEvent(&_scInitData, event) == BPS_SUCCESS) {
            OnScoreloopCoreEvent();
            return true;
        }

        if (SCUI_Client_HandleEvent(_scuiClient, event)) {
            return true;
        }
    }
    return false;
}

void AppDelegate::applicationDidShow(float value)
{
    //////////////////////////////
    // Initialize Scoreloop UI:
    // create Scoreloop after swap-buffers, so we have the main window created and shown,
    // as there is a possibility, that this game is launched for a user,
    // which has never ever launched Games app before,
    // and then the 'wizard' will popup, blocking the UI for some time.
	InitializeScoreloop();
}

void AppDelegate::StaticOnScoreloopUIRequestCompletion(void *cookie, SC_Error_t completionStatus)
{
    AppDelegate *this_ = (AppDelegate *)cookie;

    this_->OnScoreloopUiRequestCompletion(completionStatus);
}

void AppDelegate::OnScoreloopUiRequestCompletion(SC_Error_t completionStatus)
{
    /////////////////////////////
	// add code if you need to do something before view is displayed
}

void AppDelegate::OnScoreloopCoreEvent()
{
    SC_Session_h session = NULL;
    SC_SessionState_t state;

    /////////////////////////////
    // monitor, when session state changed to "authenticated":

    if (_lastSessionState != SC_SESSION_AUTHENTICATED) {
        session = SC_Client_GetSession(_scClient);
        state = SC_Session_GetState(session);
        if (state != _lastSessionState) {
            _lastSessionState = state;
            if (state == SC_SESSION_AUTHENTICATED) {
                OnScoreloopUserAuthenticated();
            }
        }
    }
}

void AppDelegate::OnScoreloopUserAuthenticated()
{
    SC_Session_h session;
    SC_User_h sessionUser;
    SC_String_h login;
    char buffer[64];

    session = SC_Client_GetSession(_scClient);
    sessionUser = SC_Session_GetUser(session);
    login = SC_User_GetLogin(sessionUser);

    /////////////////////////////
    // show toast with user login:
    snprintf(buffer, sizeof(buffer), "Welcome back %s!", SC_String_GetData(login));

    if (_dialog) {
        dialog_destroy(_dialog);
    }
    dialog_create_toast(&_dialog);
    dialog_set_toast_message_text(_dialog, buffer);
    dialog_show(_dialog);
}
