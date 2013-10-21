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

#include <QDebug>

using namespace bb::cascades;


// A helper method to convert a SC_String_h to a QString
//! [0]
static QString asQString(SC_String_h scString) {
	if (scString) {
		const char *cp = SC_String_GetData(scString);
		if (cp) {
			return QString::fromUtf8(cp);
		}
	}

	return QString();
}
//! [0]

//! [1]
ScoreloopData::ScoreloopData(SC_Client_h client, QObject *parent) :
		QObject(parent), m_client(client), m_userController(0), m_user(0) {
	// We pass 'this' as cookie for the callback, so we can retrieve the it when in the
	// static callback
	SC_Client_CreateUserController(m_client, &m_userController,
			userControllerCallback, this);
	m_available = false;
	m_score = 0;
	m_scoreController = 0;
	m_scoreSubmitionInProgress = false;
	m_leaderboardOperationInProgress = false;
	m_leaderboardModel = new ArrayDataModel;
}
//! [1]

//! [2]
void ScoreloopData::load() {
	SC_UserController_LoadUser(m_userController);
}
//! [2]

//! [3]
void ScoreloopData::userControllerCallback(void* cookie, SC_Error_t status) {
	// This callback is registered when creating the controller
	// it will be invoked every time the controller completes a request

	// Since this is a static function, we passed our object through as cookie
	ScoreloopData* self = static_cast<ScoreloopData*>(cookie);

	bool available = (status == SC_OK);
	self->setAvailable(available);
	if (available) {
		qDebug()
				<< "ScoreloopData::userControllerCallback: Scoreloop available!";
		self->userControllerReady();
	} else {
		qDebug() << SC_MapErrorToStr(status);
	}
}
//! [3]

//! [4]
QString ScoreloopData::userName() const {
	if (m_user) {
		return asQString(SC_User_GetLogin(m_user));
	}
	return QString();
}
//! [4]

bool ScoreloopData::available() {
	return this->m_available;
}

void ScoreloopData::setAvailable(bool available) {
	if (m_available != available) {
		m_available = available;
		emit availableChanged(available);
	}
}

void ScoreloopData::submitScore(double score) {
	if (m_available) {
		if (m_scoreSubmitionInProgress) {
			qWarning()
					<< "ScoreloopData::submitScore:submitScore called while operation in progress";
		} else {
			Q_ASSERT(!m_score);
			SC_Error_t error = SC_Client_CreateScore(m_client, &m_score);
			if (error == SC_OK) {
				SC_Score_SetResult(m_score, static_cast<double>(score));
				SC_Score_SetMode(m_score, 0);
				SC_ScoreController_SubmitScore(m_scoreController, m_score);
				m_scoreSubmitionInProgress = true;
			} else {
				SC_Score_Release(m_score);
				m_score = 0;
				qDebug() << "ScoreloopData::submitScore:ERROR: " << error;
			}
			qDebug() << "ScoreloopData::submitScore:OK";
		}
	} else {
		qWarning()
				<< "ScoreloopData::submitScore:FAIL: Scoreloop is no available.";
	}
}

void ScoreloopData::submitScoreComplete(void* cookie, SC_Error_t status) {
	ScoreloopData* self = static_cast<ScoreloopData*>(cookie);
	Q_ASSERT(self->m_scoreSubmitionInProgress);
	Q_ASSERT(self->m_score);
	self->m_scoreSubmitionInProgress = false;
	if (status == SC_HTTP_SERVER_ERROR) {
		SC_User_h user = SC_UserController_GetUser(self->m_userController);
		const char *login = SC_String_GetData(SC_User_GetLogin(user));
		double score = SC_Score_GetResult(self->m_score);
		qDebug()
				<< "ScoreloopData::submitScoreComplete:WARNING: Internet connection problem.";
		qDebug()
				<< "ScoreloopData::submitScoreComplete:WARNING: Save local data - User:"
				<< login << ", Score:" << score;
	} else if (status == SC_OK) {
		qDebug()
				<< "ScoreloopData::submitScoreComplete:OK: Score submitted successfully!";
	} else if (status == SC_NO_NETWORK) {
		self->setAvailable(false);
		qDebug()
				<< "ScoreloopData::submitScoreComplete:FAILED: No internet connection!";
	} else {
		qWarning() << "ScoreloopData::submitScoreComplete:ERROR:"
				<< SC_MapErrorToStr(status);
	}
	emit self->submitScoreCompleted(status == SC_OK);
	SC_Score_Release(self->m_score);
	self->m_score = 0;
}

void ScoreloopData::userControllerReady() {
	qDebug() << "ScoreloopData::userControllerReady:OK";
	this->m_user = SC_UserController_GetUser(this->m_userController);
	emit this->userNameChanged();
	SC_Error_t status = SC_Client_CreateScoreController(m_client,
			&m_scoreController, submitScoreComplete, this);
	if (status == SC_OK)
		qDebug()
				<< "ScoreloopData::userControllerReady:SC_Client_CreateScoreController:OK";
	status = SC_Client_CreateScoresController(m_client, &m_scoresController,
			fetchLeaderboardComplete, this);
	if (status == SC_OK) {
		qDebug()
				<< "ScoreloopData::userControllerReady:SC_Client_CreateScoresController:OK";
	}
}

void ScoreloopData::scoreControllerReady() {

}

void ScoreloopData::fetchLeaderboard() {
	const SC_Range_t scores = { 0, 10 };

	if (m_leaderboardOperationInProgress) {
		// It is a GameLogic error to call fetchLeaderboard more than once
		// before fetchLeaderboardComplete is executed.
		Q_ASSERT(
				!"fetchLeaderboard called while leaderboard operation in progress");
		return;
	}

	SC_Error_t rc = SC_ScoresController_LoadScores(m_scoresController, scores);
	if (rc != SC_OK) {
		fprintf(stderr, "Error loading leaderboard score range: %d\n", rc);
		return;
	}

	m_leaderboardOperationInProgress = true;
}

void ScoreloopData::fetchLeaderboardComplete(void * cookie, SC_Error_t result) {
	ScoreloopData* self = static_cast<ScoreloopData*>(cookie);
	Q_ASSERT(self->m_leaderboardOperationInProgress);
	self->m_leaderboardOperationInProgress = false;

	self->m_leaderboardModel->clear();

	if (result == SC_OK) {
		SC_ScoreList_h scoreList = SC_ScoresController_GetScores(
				self->m_scoresController);
		if (scoreList == NULL) {
			fprintf(stderr, "Could not copy results to the score list.\n");
			return;
		}

		const unsigned int numScores = SC_ScoreList_GetCount(scoreList);

		for (unsigned int i = 0; i < numScores; i++) {
			SC_Score_h score = SC_ScoreList_GetAt(scoreList, i);
			SC_User_h user = SC_Score_GetUser(score);

			QString login = "Unknown";

			if (user) {
				login = QString(SC_String_GetData(SC_User_GetLogin(user)));
				qDebug() << "ScoreloopData::fetchLeaderboardComplete: found user:" << login;
			}

			//QObject * leaderboardScore = new Score(SC_Score_GetRank(score), login, static_cast<int>(SC_Score_GetResult(score)));
			QVariantMap scoreMap;
			scoreMap.insert("score", QVariant::fromValue(SC_Score_GetRank(score)));
			scoreMap.insert("name", QVariant::fromValue(login));
			scoreMap.insert("rank", QVariant::fromValue(static_cast<int>(SC_Score_GetResult(score))));
			self->m_leaderboardModel->append(scoreMap);
		}
		qDebug()
				<< "ScoreloopData::fetchLeaderboardComplete: leaderboard loaded successfully!";
		emit self->leaderboardModelChanged();
	} else if (result == SC_HTTP_SERVER_ERROR) {
		//fetchLocalScores(m_leaderboard);
		qDebug()
				<< "ScoreloopData::fetchLeaderboardComplete: you must to store locally.";
	} else {
		qWarning()
				<< "ScoreloopData::fetchLeaderboardComplete: Fetching leaderboard failed "
				<< result;
	}
	emit self->fetchLeaderboardCompleted();
}

bb::cascades::ArrayDataModel * ScoreloopData::leaderboardModel() {
	return m_leaderboardModel;
}
