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

#ifndef ScoreloopSampleCascades_HPP_
#define ScoreloopSampleCascades_HPP_

#include <QObject>
#include <scoreloop/scoreloopcore.h>
#include "ScoreloopBpsEventHandler.hpp"
#include "ScoreloopData.hpp"

namespace bb { namespace cascades { class Application; }}

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class ScoreloopSampleCascades : public QObject
{
    Q_OBJECT
public:
    ScoreloopSampleCascades(bb::cascades::Application *app);
    virtual ~ScoreloopSampleCascades() {}

    Q_SLOT void showMain();

private:
    SC_InitData_t initData_;
    SC_Client_h client_;

    ScoreloopBpsEventHandler* eventHandler_;
    ScoreloopData* scoreloopData_;
};

#endif /* ScoreloopSampleCascades_HPP_ */
