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

#include <device/types/device-energy-management/EnergyManagement.h>
#include <devices/Types.h>

namespace chip::app {

EnergyManagement::EnergyManagement(const Context & context) : EnergyManagement(context, Config{}) {}

EnergyManagement::EnergyManagement(const Context & context, Config config) :
    SingleEndpoint(Span<const DataModel::DeviceTypeEntry>(&Device::Type::kDeviceEnergyManagement, 1)),
    mTimerDelegate(context.timerDelegate), mDelegate(context.delegate), mConfig(config)
{}

CHIP_ERROR EnergyManagement::Register(EndpointId endpoint, CodeDrivenDataModelProvider & provider, EndpointComposition composition)
{
    VerifyOrReturnError(SingleEndpoint::mEndpointId == kInvalidEndpointId, CHIP_ERROR_INCORRECT_STATE);
    DeviceRegistrationTransaction transaction(*this, provider);

    mProvider = &provider;
    ReturnErrorOnFailure(RegisterDescriptor(endpoint, provider, composition));

    mIdentifyCluster.Create(Clusters::IdentifyCluster::Config(endpoint, mTimerDelegate));
    ReturnErrorOnFailure(provider.AddCluster(mIdentifyCluster.Registration()));

    Clusters::DeviceEnergyManagementCluster::Config config(endpoint, mConfig.featureFlags, mDelegate);
    mDemCluster.Create(config);
    ReturnErrorOnFailure(provider.AddCluster(mDemCluster.Registration()));

    ReturnErrorOnFailure(provider.AddEndpoint(mEndpointRegistration));
    transaction.Commit();
    return CHIP_NO_ERROR;
}

void EnergyManagement::Unregister(CodeDrivenDataModelProvider & provider)
{
    mProvider = nullptr;

    UnregisterDescriptor(provider);

    if (mDemCluster.IsConstructed())
    {
        LogErrorOnFailure(provider.RemoveCluster(&mDemCluster.Cluster()));
        mDemCluster.Destroy();
    }

    if (mIdentifyCluster.IsConstructed())
    {
        LogErrorOnFailure(provider.RemoveCluster(&mIdentifyCluster.Cluster()));
        mIdentifyCluster.Destroy();
    }
}

CHIP_ERROR EnergyManagement::NotifyAttributeChanged(AttributeId attributeId) const
{
    VerifyOrReturnValue(mProvider != nullptr, CHIP_NO_ERROR);
    mProvider->NotifyAttributeChanged({ GetEndpointId(), Clusters::DeviceEnergyManagement::Id, attributeId },
                                      DataModel::AttributeChangeType::kReportable);
    return CHIP_NO_ERROR;
}

} // namespace chip::app
