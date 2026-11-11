/*
 *    Copyright (c) 2026 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <device/types/mounted-dimmable-load-control/impl/LoggingMountedDimmableLoadControl.h>

namespace chip::app {

LoggingMountedDimmableLoadControl::LoggingMountedDimmableLoadControl(const Context & context, const Config & config) :
    MountedDimmableLoadControl(context,
                               Delegates{ .onOff        = static_cast<Clusters::OnOffDelegate &>(*this),
                                          .levelControl = static_cast<Clusters::LevelControlDelegate &>(*this),
                                          .effect       = static_cast<Clusters::OnOffEffectDelegate &>(*this),
                                          .identify     = context.identifyDelegate },
                               config)
{}

} // namespace chip::app
