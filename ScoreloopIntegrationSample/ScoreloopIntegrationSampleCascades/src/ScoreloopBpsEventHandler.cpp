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

/*
 * ScoreloopBpsEventHandler
 * Plugs the Scoreloop Event Handler into BPS' runloop
 *
 */

#include "ScoreloopBpsEventHandler.hpp"

ScoreloopBpsEventHandler::ScoreloopBpsEventHandler(SC_InitData_t initData) {
	initData_ = initData;

	// this only works after SC_Client_New was called
	subscribe(SC_GetBPSEventDomain(&initData_));
}

void ScoreloopBpsEventHandler::event(bps_event_t *event) {
    SC_HandleBPSEvent(&initData_, event);
}

