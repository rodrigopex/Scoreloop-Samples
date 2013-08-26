/*
* This file originates from templates from http://www.cocos2d-x.org/
* See LICENSE for details of cocos2d-x license.
*
* Portions Copyright (c) 2012 Research In Motion Limited.
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

#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "cocos2d.h"
#include "ScoreloopScene.h"
#include <scoreloop/scoreloopcore.h>
#include <scoreloop/scui_client.h>
#include <bps/dialog.h>

/**
@brief    The cocos2d Application.

The reason for implement as private inheritance is to hide some interface call by CCDirector.
*/
class  AppDelegate : public cocos2d::CCObject, private cocos2d::CCApplication, public cocos2d::CCEGLView::EventHandler
{
public:
    AppDelegate();
    virtual ~AppDelegate();

    /**
    @brief    Implement CCDirector and CCScene init code here.
    @return true    Initialize success, app continue.
    @return false   Initialize failed, app terminate.
    */
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  The function be called when the application enter background
    @param  the pointer of the application
    */
    virtual void applicationDidEnterBackground();

    /**
    @brief  The function be called when the application enter foreground
    @param  the pointer of the application
    */
    virtual void applicationWillEnterForeground();

    /**
    @brief  The function be called to handle bps event
    @param  event the bps event to be handled
    @return true if success, false otherwise.
    */
    virtual bool HandleBPSEvent(bps_event_t* event);

    void applicationDidShow(float value);

private:
    void InitializeScoreloop();
    static void StaticOnScoreloopUiResultCallback(void *cookie, SCUI_Result_t viewResult, const void *data);
    static void StaticOnScoreloopUIRequestCompletion(void *cookie, SC_Error_t completionStatus);
    void OnScoreloopUiRequestCompletion(SC_Error_t completionStatus);
    void OnScoreloopCoreEvent();
    void OnScoreloopUserAuthenticated();

private:
    dialog_instance_t _dialog;
    SC_SessionState_t _lastSessionState;
    SC_InitData_t _scInitData;
    SC_Client_h _scClient;
    SCUI_Client_h _scuiClient;
    ScoreloopScene *_scoreloopScene;
    bool _didDeclineScoreloop;
};

#endif // _APP_DELEGATE_H_

