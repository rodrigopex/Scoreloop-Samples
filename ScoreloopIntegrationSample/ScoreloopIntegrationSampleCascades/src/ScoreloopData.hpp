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

#ifndef SCORELOOPDATA_HPP_
#define SCORELOOPDATA_HPP_

#include <QtCore/QObject>
#include <QtDeclarative/QtDeclarative>
#include <scoreloop/scoreloopcore.h>

class ScoreloopData: public QObject {
	Q_OBJECT
	Q_PROPERTY(QString userName READ userName NOTIFY userNameChanged);
public:
	ScoreloopData(SC_Client_h client);
	QString userName();
	Q_INVOKABLE void load();

Q_SIGNALS:
	void userNameChanged();

protected:
	static void userControllerCallback(void* cookie, SC_Error_t status);
	static QString asQString(SC_String_h scString);
private:
	void updateUser();
	SC_Client_h client_;
	SC_UserController_h userController_;
	SC_User_h user_;
};

QML_DECLARE_TYPE(ScoreloopData);

#endif /* SCORELOOPDATA_HPP_ */
