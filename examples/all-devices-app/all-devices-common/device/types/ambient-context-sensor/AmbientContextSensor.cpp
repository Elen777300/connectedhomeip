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
#include <device/types/ambient-context-sensor/AmbientContextSensor.h>
#include <devices/Types.h>
#include <lib/support/logging/CHIPLogging.h>

using namespace chip::app::Clusters;

namespace chip::app {

AmbientContextSensor::AmbientContextSensor(const Config & config) :
    SingleEndpoint(Span<const DataModel::DeviceTypeEntry>(&Device::Type::kAmbientContextSensor, 1)), mConfig(config)
{}

CHIP_ERROR AmbientContextSensor::Register(chip::EndpointId endpoint, CodeDrivenDataModelProvider & provider,
                                          EndpointComposition composition)
{
    VerifyOrReturnError(mEndpointId == kInvalidEndpointId, CHIP_ERROR_INCORRECT_STATE);
    DeviceRegistrationTransaction transaction(*this, provider);

    ReturnErrorOnFailure(RegisterDescriptor(endpoint, provider, composition));

    mIdentifyCluster.Create(IdentifyCluster::Config(endpoint, mConfig.timerDelegate));
    ReturnErrorOnFailure(provider.AddCluster(mIdentifyCluster.Registration()));

    mServiceAreaCluster.Create(endpoint, mConfig.serviceAreaStorageDelegate, mConfig.serviceAreaDelegate,
                               BitMask<ServiceArea::Feature>{});
    ReturnErrorOnFailure(provider.AddCluster(mServiceAreaCluster.Registration()));

    ReturnErrorOnFailure(RegisterOptionalClusters(endpoint, provider));

    ReturnErrorOnFailure(provider.AddEndpoint(mEndpointRegistration));
    transaction.Commit();
    return CHIP_NO_ERROR;
}

void AmbientContextSensor::Unregister(CodeDrivenDataModelProvider & provider)
{
    UnregisterDescriptor(provider);

    UnregisterOptionalClusters(provider);

    if (mServiceAreaCluster.IsConstructed())
    {
        LogErrorOnFailure(provider.RemoveCluster(&mServiceAreaCluster.Cluster()));
        mServiceAreaCluster.Destroy();
    }
    if (mIdentifyCluster.IsConstructed())
    {
        LogErrorOnFailure(provider.RemoveCluster(&mIdentifyCluster.Cluster()));
        mIdentifyCluster.Destroy();
    }
}

} // namespace chip::app
