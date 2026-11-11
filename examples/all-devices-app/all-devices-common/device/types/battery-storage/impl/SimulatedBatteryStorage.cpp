/*
 *    Copyright (c) 2026 Project CHIP Authors
 *    All rights reserved.
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

#include <device/types/battery-storage/impl/SimulatedBatteryStorage.h>

#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>

namespace chip::app {

namespace {

using DemFeature = Clusters::DeviceEnergyManagement::Feature;
using EsaType    = Clusters::DeviceEnergyManagement::ESATypeEnum;

// Battery Storage rev 2 component tags use the Power Source namespace (Grid / Battery).
const EndpointComposition::SemanticTag kGridComponentTag[] = {
    { .mfgCode     = DataModel::NullNullable,
      .namespaceID = to_underlying(Clusters::Globals::SemanticTagNamespaceId::kPowerSource),
      .tag         = 0x01 },
};
const EndpointComposition::SemanticTag kBatteryComponentTag[] = {
    { .mfgCode     = DataModel::NullNullable,
      .namespaceID = to_underlying(Clusters::Globals::SemanticTagNamespaceId::kPowerSource),
      .tag         = 0x03 },
};

EndpointComposition ChildComposition(EndpointId parent, Span<const EndpointComposition::SemanticTag> tags)
{
    EndpointComposition composition = EndpointComposition::WithParent(parent);
    composition.tagList             = tags;
    return composition;
}

SimulatedEnergyManagement::Config BatteryStorageEnergyManagementConfig()
{
    return SimulatedEnergyManagement::Config{
        .featureFlags   = BitMask<DemFeature>(DemFeature::kPowerAdjustment),
        .esaType        = EsaType::kBatteryStorage,
        .esaCanGenerate = true,
    };
}

} // namespace

SimulatedBatteryStorage::SimulatedBatteryStorage(const Context & context) :
    mTimerDelegate(context.timerDelegate), mEnergyManagement(context.timerDelegate, BatteryStorageEnergyManagementConfig()),
    mGridElectricalSensor(context.timerDelegate, context.testEventTriggerDelegate, {}, SimulatedElectricalSensor::Role::kGrid),
    mBatteryElectricalSensor(context.timerDelegate, context.testEventTriggerDelegate, {},
                             SimulatedElectricalSensor::Role::kBattery),
    mGridPowerSource("Battery storage grid connection"_span, Clusters::PowerSource::WiredCurrentTypeEnum::kAc, 0),
    mBatteryPowerSource("Battery storage pack"_span, Clusters::PowerSource::BatReplaceabilityEnum::kNotReplaceable,
                        context.timerDelegate, 1, BatteryPowerSource::ChargingCapability::kRechargeable)
{}

CHIP_ERROR SimulatedBatteryStorage::RegisterParts(EndpointIdAllocator & allocator, CodeDrivenDataModelProvider & provider)
{
    ReturnErrorOnFailure(mEnergyManagement.Register(allocator.Allocate(), provider, EndpointComposition::WithParent(mEndpointId)));
    ReturnErrorOnFailure(
        mGridElectricalSensor.Register(allocator.Allocate(), provider, ChildComposition(mEndpointId, Span(kGridComponentTag))));
    ReturnErrorOnFailure(mBatteryElectricalSensor.Register(allocator.Allocate(), provider,
                                                           ChildComposition(mEndpointId, Span(kBatteryComponentTag))));
    ReturnErrorOnFailure(
        mGridPowerSource.Register(allocator.Allocate(), provider, ChildComposition(mEndpointId, Span(kGridComponentTag))));
    ReturnErrorOnFailure(
        mBatteryPowerSource.Register(allocator.Allocate(), provider, ChildComposition(mEndpointId, Span(kBatteryComponentTag))));
    return CHIP_NO_ERROR;
}

void SimulatedBatteryStorage::UnregisterParts(CodeDrivenDataModelProvider & provider)
{
    if (mBatteryPowerSource.GetEndpointId() != kInvalidEndpointId)
    {
        mBatteryPowerSource.Unregister(provider);
    }
    if (mGridPowerSource.GetEndpointId() != kInvalidEndpointId)
    {
        mGridPowerSource.Unregister(provider);
    }
    if (mBatteryElectricalSensor.GetEndpointId() != kInvalidEndpointId)
    {
        mBatteryElectricalSensor.Unregister(provider);
    }
    if (mGridElectricalSensor.GetEndpointId() != kInvalidEndpointId)
    {
        mGridElectricalSensor.Unregister(provider);
    }
    if (mEnergyManagement.GetEndpointId() != kInvalidEndpointId)
    {
        mEnergyManagement.Unregister(provider);
    }
}

CHIP_ERROR SimulatedBatteryStorage::RegisterOptionalClusters(EndpointId endpoint, CodeDrivenDataModelProvider & provider)
{
    mIdentifyCluster.Create(Clusters::IdentifyCluster::Config(endpoint, mTimerDelegate));
    return provider.AddCluster(mIdentifyCluster.Registration());
}

void SimulatedBatteryStorage::UnregisterOptionalClusters(CodeDrivenDataModelProvider & provider)
{
    if (mIdentifyCluster.IsConstructed())
    {
        LogErrorOnFailure(provider.RemoveCluster(&mIdentifyCluster.Cluster()));
        mIdentifyCluster.Destroy();
    }
}

} // namespace chip::app
