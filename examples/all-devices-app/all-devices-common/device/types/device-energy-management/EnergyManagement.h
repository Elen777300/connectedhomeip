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

#pragma once

#include <app/clusters/device-energy-management-server/DeviceEnergyManagementCluster.h>
#include <app/clusters/identify-server/IdentifyCluster.h>
#include <app/server-cluster/ServerClusterInterfaceRegistry.h>
#include <device/api/SingleEndpoint.h>
#include <lib/support/TimerDelegate.h>

namespace chip::app {

class EnergyManagement : public SingleEndpoint
{
public:
    using Feature  = Clusters::DeviceEnergyManagement::Feature;
    using Delegate = Clusters::DeviceEnergyManagement::Delegate;

    struct Config
    {
        BitMask<Feature> featureFlags;
        Clusters::DeviceEnergyManagement::ESATypeEnum esaType = Clusters::DeviceEnergyManagement::ESATypeEnum::kEvse;
        bool esaCanGenerate                                   = false;
    };

    struct Context
    {
        TimerDelegate & timerDelegate;
        Delegate & delegate;
    };

    explicit EnergyManagement(const Context & context);
    EnergyManagement(const Context & context, Config config);
    ~EnergyManagement() override = default;

    CHIP_ERROR Register(EndpointId endpoint, CodeDrivenDataModelProvider & provider, EndpointComposition composition = {}) override;
    void Unregister(CodeDrivenDataModelProvider & provider) override;

    // Public getters for programmatic control
    using SingleEndpoint::GetEndpointId;

    Clusters::IdentifyCluster & IdentifyCluster() { return mIdentifyCluster.Cluster(); }
    Clusters::DeviceEnergyManagementCluster & EnergyManagementCluster() { return mDemCluster.Cluster(); }

protected:
    const Config & GetConfig() const { return mConfig; }

    /// Reports a change of an attribute of the Device Energy Management cluster.
    /// The device reports nothing until it is registered, so this is a no-op then.
    CHIP_ERROR NotifyAttributeChanged(AttributeId attributeId) const;

private:
    TimerDelegate & mTimerDelegate;
    Delegate & mDelegate;
    Config mConfig;
    CodeDrivenDataModelProvider * mProvider = nullptr;

    LazyRegisteredServerCluster<Clusters::IdentifyCluster> mIdentifyCluster;
    LazyRegisteredServerCluster<Clusters::DeviceEnergyManagementCluster> mDemCluster;
};

} // namespace chip::app
