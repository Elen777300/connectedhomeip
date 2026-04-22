/*
 *
 *    Copyright (c) 2023-2026 Project CHIP Authors
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

#pragma once

#include "OperationalStateCluster.h"
#include <app/server-cluster/ServerClusterInterfaceRegistry.h>

namespace chip {
namespace app {
namespace Clusters {
namespace OperationalState {

/**
 * Legacy wrapper around OperationalStateCluster for backwards compatibility with existing applications
 * that construct Instance objects directly and call Init().
 *
 * NEW CODE should use OperationalStateCluster directly, which integrates cleanly with the
 * code-driven data model and does not carry the Ember/ZAP compatibility overhead.
 */
class Instance
{
public:
    Instance(Delegate * aDelegate, EndpointId aEndpointId);
    virtual ~Instance();

    /**
     * Register the cluster instance with the codegen data model provider.
     */
    CHIP_ERROR Init();

    /**
     * Unregister the cluster instance.
     */
    void Shutdown();

    // Forwarding API — delegates to the underlying OperationalStateCluster.
    CHIP_ERROR SetCurrentPhase(const DataModel::Nullable<uint8_t> & aPhase) { return Cluster().SetCurrentPhase(aPhase); }
    CHIP_ERROR SetOperationalState(uint8_t aOpState) { return Cluster().SetOperationalState(aOpState); }
    DataModel::Nullable<uint8_t> GetCurrentPhase() const { return Cluster().GetCurrentPhase(); }
    uint8_t GetCurrentOperationalState() const { return Cluster().GetCurrentOperationalState(); }
    void GetCurrentOperationalError(GenericOperationalError & error) const { Cluster().GetCurrentOperationalError(error); }
    void UpdateCountdownTimeFromDelegate() { Cluster().UpdateCountdownTimeFromDelegate(); }
    void OnOperationalErrorDetected(const Structs::ErrorStateStruct::Type & aError) { Cluster().OnOperationalErrorDetected(aError); }
    void OnOperationCompletionDetected(uint8_t aCompletionErrorCode,
                                       const Optional<DataModel::Nullable<uint32_t>> & aTotalOperationalTime = NullOptional,
                                       const Optional<DataModel::Nullable<uint32_t>> & aPausedTime           = NullOptional)
    {
        Cluster().OnOperationCompletionDetected(aCompletionErrorCode, aTotalOperationalTime, aPausedTime);
    }
    void ReportOperationalStateListChange() { Cluster().ReportOperationalStateListChange(); }
    void ReportPhaseListChange() { Cluster().ReportPhaseListChange(); }
    bool IsSupportedPhase(uint8_t aPhase) { return Cluster().IsSupportedPhase(aPhase); }
    bool IsSupportedOperationalState(uint8_t aState) { return Cluster().IsSupportedOperationalState(aState); }

protected:
    // For derived Instance classes (RvcOperationalState::Instance, OvenCavityOperationalState::Instance)
    // that own a different RegisteredServerCluster<DerivedCluster> (via the "private base" idiom).
    // cluster and registration must outlive this object — guaranteed by the derived class ordering.
    Instance(OperationalStateCluster & cluster, ServerClusterRegistration & registration, Delegate * aDelegate);

    OperationalStateCluster & Cluster() { return *mClusterPtr; }
    const OperationalStateCluster & Cluster() const { return *mClusterPtr; }

    bool mRegistered = false;

private:
    Delegate * mDelegate;
    OperationalStateCluster * mClusterPtr;
    ServerClusterRegistration * mRegPtr;
    // Heap-allocated for the base case only; nullptr when a derived class supplies the cluster.
    RegisteredServerCluster<OperationalStateCluster> * mOwnedCluster;
};

} // namespace OperationalState

namespace RvcOperationalState {

namespace detail {
// Private base initialized before OperationalState::Instance in the derived class's
// initialization list, ensuring the cluster exists when the base constructor runs.
struct RvcInstanceBase
{
    RegisteredServerCluster<RvcOperationalStateCluster> mCluster;
    RvcInstanceBase(Delegate * aDelegate, EndpointId aEndpointId) : mCluster(aEndpointId, aDelegate) {}
};
} // namespace detail

class Instance : private detail::RvcInstanceBase, public OperationalState::Instance
{
public:
    Instance(Delegate * aDelegate, EndpointId aEndpointId) :
        detail::RvcInstanceBase(aDelegate, aEndpointId),
        OperationalState::Instance(detail::RvcInstanceBase::mCluster.Cluster(),
                                    detail::RvcInstanceBase::mCluster.Registration(),
                                    static_cast<OperationalState::Delegate *>(aDelegate))
    {}
};

} // namespace RvcOperationalState

namespace OvenCavityOperationalState {

namespace detail {
struct OvenInstanceBase
{
    RegisteredServerCluster<OvenCavityOperationalStateCluster> mCluster;
    OvenInstanceBase(OperationalState::Delegate * aDelegate, EndpointId aEndpointId) : mCluster(aEndpointId, aDelegate) {}
};
} // namespace detail

class Instance : private detail::OvenInstanceBase, public OperationalState::Instance
{
public:
    Instance(OperationalState::Delegate * aDelegate, EndpointId aEndpointId) :
        detail::OvenInstanceBase(aDelegate, aEndpointId),
        OperationalState::Instance(detail::OvenInstanceBase::mCluster.Cluster(),
                                    detail::OvenInstanceBase::mCluster.Registration(), aDelegate)
    {}
};

} // namespace OvenCavityOperationalState

} // namespace Clusters
} // namespace app
} // namespace chip
