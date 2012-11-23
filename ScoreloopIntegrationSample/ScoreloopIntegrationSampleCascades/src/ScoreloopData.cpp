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

#include "ScoreloopData.hpp"
#include <qdebug.h>

ScoreloopData::ScoreloopData(SC_Client_h client) {
	client_ = client;
	user_ = 0;

	// create controller

	// we pass 'this' as cookie for the callback, so we can retrieve the it when in the
	// static callback
	SC_Client_CreateUserController(client_, &userController_, userControllerCallback, this);
}

void ScoreloopData::load() {
	SC_UserController_LoadUser(userController_);
}

void ScoreloopData::userControllerCallback(void* cookie, SC_Error_t status) {
	// this callback is registered when creating the controller
	// it will be invoked every time the controller completes a request

	// since this is a static function, we passed our object through as cookie
	ScoreloopData* self = static_cast<ScoreloopData*>(cookie);

	if(status == SC_OK) {
		// loading went ok, we can update our properties
		self->updateUser();
	}
	else {
		// You might want to add proper error handling.
		qDebug() << SC_MapErrorToStr(status);
	}

}

void ScoreloopData::updateUser() {
	// retrieves the user object from the controller
	user_ = SC_UserController_GetUser(userController_);
	Q_EMIT userNameChanged();
}

QString ScoreloopData::userName() {
	if(user_) {
		return asQString(SC_User_GetLogin(user_));
	}
	return "";
}

QString ScoreloopData::asQString(SC_String_h scString)
{
    if (scString) {
        const char *cp = SC_String_GetData(scString);
        if (cp) {
            return QString::fromUtf8(cp);
        }
    }
    return "";
}
