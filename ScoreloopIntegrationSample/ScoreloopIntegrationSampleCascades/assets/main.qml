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

import bb.cascades 1.0

Page {
    titleBar: TitleBar {
        kind: TitleBarKind.FreeForm
        kindProperties: FreeFormTitleBarKindProperties {
            content: Container {
                layout: DockLayout {

                }
                leftPadding: 20
                rightPadding: 20
                Label {
                    horizontalAlignment: HorizontalAlignment.Left
                    verticalAlignment: VerticalAlignment.Center
                    text: qsTr("Scoreloop Sample Cascades")
                    textStyle {
                        base: SystemDefaults.TextStyles.TitleText
                        color: Color.White
                    }
                }
                ImageView {
                    id: scoreloopIcon
                    opacity: 0.25 + _scoreloop.available * 0.75
                    imageSource: "asset:///images/sl_badge256.png"
                    scalingMethod: ScalingMethod.AspectFit
                    horizontalAlignment: HorizontalAlignment.Right
                    verticalAlignment: VerticalAlignment.Center
                    onCreationCompleted: {
                        _scoreloop.submitScoreCompleted.connect(loading.stop)
                        _scoreloop.fetchLeaderboardCompleted.connect(loading.stop)
                    }
                    animations: RotateTransition {
                        id: loading
                        fromAngleZ: 0
                        toAngleZ: 180
                        duration: 500
                        repeatCount: AnimationRepeatCount.Forever
                        onStopped: {
                            if (scoreloopIcon.rotationZ > 0)
                                scoreloopIcon.rotationZ = 180;
                            else
                                scoreloopIcon.rotationZ = 0;
                        }
                    }
                }
            }
        }
    }
    Container {
        topPadding: 20
        bottomPadding: 10
        rightPadding: 10

//        layout: DockLayout {
//        }
        Container {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center

            Label {
                horizontalAlignment: HorizontalAlignment.Center
                //topMargin: 50

                text: qsTr("Your username is")
                //                textStyle {
                //                    color: Color.White
                //                }
            }

            //! [0]
            Label {
                horizontalAlignment: HorizontalAlignment.Center

                text: _scoreloop.userName
                //                textStyle {
                //                    color: Color.White
                //                }
            }
            //! [0]

            //! [1]
            Button {
                horizontalAlignment: HorizontalAlignment.Center
                //topMargin: 100

                text: qsTr("Reload")

                onClicked: {
                    console.log("scoreloop: " + _scoreloop);
                    console.log("scoreloop.userName: " + _scoreloop.userName);
                    _scoreloop.load();
                }
            }
            //! [1]
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                topPadding: 100
                Label {
                    text: "Score:"
                    verticalAlignment: VerticalAlignment.Center
                    textStyle.base: SystemDefaults.TextStyles.BigText
                }
                Label {
                    text: scoreButton.score
                    verticalAlignment: VerticalAlignment.Center
                    textStyle.base: SystemDefaults.TextStyles.BigText
                }
                Button {
                    id: scoreButton
                    property double score: 0
                    text: "Up"
                    verticalAlignment: VerticalAlignment.Center
                    preferredWidth: 60
                    onClicked: ++ score
                }
            }
            Button {
                id: submitButton
                enabled: _scoreloop.available
                horizontalAlignment: HorizontalAlignment.Fill
                text: "Submit Score"
                onClicked: {
                    _scoreloop.submitScore(scoreButton.score)
                    loading.play()
                }
            }
            Button {
                id: leaderboardButton
                enabled: _scoreloop.available
                horizontalAlignment: HorizontalAlignment.Fill
                text: "Leaderboard"
                onClicked: {
                    _scoreloop.fetchLeaderboard()
                    loading.play()
                }
            }
        }
        Header {
            title: "Leaderboard"
            topMargin: 15
        }
        ListView {
            dataModel: _scoreloop.leaderboardModel
            listItemComponents: ListItemComponent {
                StandardListItem {
                    title: ListItemData.name
                    description: ListItemData.score
                    status: ListItemData.rank
                    onCreationCompleted: {
                        console.log(ListItemData.name, ListItemData.score, ListItemData.rank)
                    }
                }
            }
        }
    }
}
