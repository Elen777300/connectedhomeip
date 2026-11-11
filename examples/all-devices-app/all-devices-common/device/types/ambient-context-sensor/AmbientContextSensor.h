/*
 *
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
#pragma once

#include <app/clusters/identify-server/IdentifyCluster.h>
#include <app/clusters/service-area-server/ServiceAreaCluster.h>
#include <device/api/SingleEndpoint.h>
#include <lib/support/TimerDelegate.h>

namespace chip::app {

/// Ambient Context Sensor device type (0x0150) rev 2.
///
/// The Ambient Context Sensing and Ambient Sensing Union clusters are optional in the
/// device type, so subclasses add them through RegisterOptionalClusters().
class AmbientContextSensor : public SingleEndpoint
{
public:
    struct Config
    {
        TimerDelegate & timerDelegate;
        Clusters::ServiceArea::Delegate & serviceAreaDelegate;
        Clusters::ServiceArea::StorageDelegate & serviceAreaStorageDelegate;
    };

    explicit AmbientContextSensor(const Config & config);
    ~AmbientContextSensor() override = default;

    // DeviceInterface pure virtual lifecycle hooks
    CHIP_ERROR Register(chip::EndpointId endpoint, CodeDrivenDataModelProvider & provider,
                        EndpointComposition composition = {}) override;
    void Unregister(CodeDrivenDataModelProvider & provider) override;

    Clusters::IdentifyCluster & IdentifyCluster() { return mIdentifyCluster.Cluster(); }
    Clusters::ServiceArea::ServiceAreaCluster & ServiceAreaCluster() { return mServiceAreaCluster.Cluster(); }

protected:
    virtual CHIP_ERROR RegisterOptionalClusters(EndpointId endpoint, CodeDrivenDataModelProvider & provider)
    {
        return CHIP_NO_ERROR;
    }
    virtual void UnregisterOptionalClusters(CodeDrivenDataModelProvider & provider) {}

    Config mConfig;

private:
    LazyRegisteredServerCluster<Clusters::IdentifyCluster> mIdentifyCluster;
    LazyRegisteredServerCluster<Clusters::ServiceArea::ServiceAreaCluster> mServiceAreaCluster;
};

} // namespace chip::app
