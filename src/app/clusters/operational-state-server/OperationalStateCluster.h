/*
 *
 *    Copyright (c) 2023-2025 Project CHIP Authors
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

#include "OperationalStateDelegate.h"
#include <app/cluster-building-blocks/QuieterReporting.h>
#include <app/server-cluster/DefaultServerCluster.h>
#include <app/server-cluster/OptionalAttributeSet.h>
#include <clusters/OperationalState/Metadata.h>
#include <clusters/OvenCavityOperationalState/Metadata.h>
#include <clusters/RvcOperationalState/Metadata.h>

namespace chip {
namespace app {
namespace Clusters {
namespace OperationalState {

static constexpr uint8_t DerivedClusterNumberSpaceStart = 0x40;
static constexpr uint8_t VendorNumberSpaceStart         = 0x80;

/**
 * OperationalStateCluster is the code-driven implementation of the Operational State cluster.
 *
 * It provides a base implementation for any derivation (e.g. RVC, OvenCavity).
 */
class OperationalStateCluster : public DefaultServerCluster
{
public:
    using OptionalAttributeSet = app::OptionalAttributeSet<OperationalState::Attributes::CountdownTime::Id>;

    static constexpr uint8_t kMaxPhaseNameLength = 64;

    struct Config
    {
        OptionalAttributeSet optionalAttributes;
    };

    /**
     * Construct an OperationalState cluster for the given endpoint.
     *
     * @param endpointId        The endpoint on which this cluster exists.
     * @param delegate          Application delegate providing state lists and command callbacks.
     *                          The caller must ensure the delegate outlives this object.
     * @param config            Configuration including optional attributes to expose.
     */
    OperationalStateCluster(EndpointId endpointId, Delegate * delegate, const Config & config = {});

    ~OperationalStateCluster() override;

    // ---- Application-facing API (preserved from legacy Instance) ----

    CHIP_ERROR SetCurrentPhase(const DataModel::Nullable<uint8_t> & aPhase);
    CHIP_ERROR SetOperationalState(uint8_t aOpState);

    DataModel::Nullable<uint8_t> GetCurrentPhase() const { return mCurrentPhase; }
    uint8_t GetCurrentOperationalState() const { return mOperationalState; }
    void GetCurrentOperationalError(GenericOperationalError & error) const;

    void UpdateCountdownTimeFromDelegate() { UpdateCountdownTime(/* fromDelegate = */ true); }

    void OnOperationalErrorDetected(const Structs::ErrorStateStruct::Type & aError);
    void OnOperationCompletionDetected(uint8_t aCompletionErrorCode,
                                       const Optional<DataModel::Nullable<uint32_t>> & aTotalOperationalTime = NullOptional,
                                       const Optional<DataModel::Nullable<uint32_t>> & aPausedTime           = NullOptional);

    void ReportOperationalStateListChange();
    void ReportPhaseListChange();

    bool IsSupportedPhase(uint8_t aPhase);
    bool IsSupportedOperationalState(uint8_t aState);

    // ---- DefaultServerCluster overrides ----

    DataModel::ActionReturnStatus ReadAttribute(const DataModel::ReadAttributeRequest & request,
                                                AttributeValueEncoder & encoder) override;

    CHIP_ERROR Attributes(const ConcreteClusterPath & path,
                          ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder) override;

    std::optional<DataModel::ActionReturnStatus> InvokeCommand(const DataModel::InvokeRequest & request,
                                                               chip::TLV::TLVReader & input_arguments,
                                                               CommandHandler * handler) override;

    CHIP_ERROR AcceptedCommands(const ConcreteClusterPath & path,
                                ReadOnlyBufferBuilder<DataModel::AcceptedCommandEntry> & builder) override;

protected:
    /**
     * Constructor for derived cluster variants (RVC, OvenCavity) with a different cluster ID
     * and spec revision.
     */
    OperationalStateCluster(EndpointId endpointId, ClusterId clusterId, uint32_t revision, Delegate * delegate,
                            const Config & config = {});

    virtual bool IsDerivedClusterStatePauseCompatible(uint8_t aState) { return false; }
    virtual bool IsDerivedClusterStateResumeCompatible(uint8_t aState) { return false; }

    /**
     * Override to handle commands that are specific to a derived cluster (e.g. GoHome for RVC).
     * Return std::nullopt for unrecognised commands.
     */
    virtual std::optional<DataModel::ActionReturnStatus> HandleDerivedClusterCommand(const DataModel::InvokeRequest & request,
                                                                                     chip::TLV::TLVReader & input,
                                                                                     CommandHandler * handler)
    {
        return Protocols::InteractionModel::Status::UnsupportedCommand;
    }

    void UpdateCountdownTime(bool fromDelegate);
    void UpdateCountdownTimeFromClusterLogic() { UpdateCountdownTime(/* fromDelegate = */ false); }

    Delegate * mDelegate;

private:
    const uint32_t mRevision;
    const Config mConfig;

    DataModel::Nullable<uint8_t> mCurrentPhase;
    uint8_t mOperationalState                 = 0;
    GenericOperationalError mOperationalError = to_underlying(ErrorStateEnum::kNoError);
    QuieterReportingAttribute<uint32_t> mCountdownTime{ DataModel::NullNullable };

    std::optional<DataModel::ActionReturnStatus> HandlePauseState(const DataModel::InvokeRequest & request,
                                                                  chip::TLV::TLVReader & input, CommandHandler * handler);
    std::optional<DataModel::ActionReturnStatus> HandleStopState(const DataModel::InvokeRequest & request,
                                                                 chip::TLV::TLVReader & input, CommandHandler * handler);
    std::optional<DataModel::ActionReturnStatus> HandleStartState(const DataModel::InvokeRequest & request,
                                                                  chip::TLV::TLVReader & input, CommandHandler * handler);
    std::optional<DataModel::ActionReturnStatus> HandleResumeState(const DataModel::InvokeRequest & request,
                                                                    chip::TLV::TLVReader & input, CommandHandler * handler);
    };

    } // namespace OperationalState
namespace RvcOperationalState {

class RvcOperationalStateCluster : public OperationalState::OperationalStateCluster
{
public:
    RvcOperationalStateCluster(EndpointId endpointId, Delegate * delegate,
                               const OperationalState::OperationalStateCluster::Config & config = {});

    CHIP_ERROR AcceptedCommands(const ConcreteClusterPath & path,
                                ReadOnlyBufferBuilder<DataModel::AcceptedCommandEntry> & builder) override;

protected:
    bool IsDerivedClusterStatePauseCompatible(uint8_t aState) override;
    bool IsDerivedClusterStateResumeCompatible(uint8_t aState) override;

    std::optional<DataModel::ActionReturnStatus> HandleDerivedClusterCommand(const DataModel::InvokeRequest & request,
                                                                             chip::TLV::TLVReader & input,
                                                                             CommandHandler * handler) override;

private:
    std::optional<DataModel::ActionReturnStatus> HandleGoHomeCommand(const DataModel::InvokeRequest & request,
                                                                    chip::TLV::TLVReader & input, CommandHandler * handler);
};

/**
 * Backward compatibility wrapper for RvcOperationalState::Instance.
 */
class Instance : public RvcOperationalStateCluster
{
public:
    Instance(Delegate * aDelegate, EndpointId aEndpointId) : RvcOperationalStateCluster(aEndpointId, aDelegate) {}
    CHIP_ERROR Init() { return CHIP_NO_ERROR; }
};

} // namespace RvcOperationalState

namespace OvenCavityOperationalState {

class OvenCavityOperationalStateCluster : public OperationalState::OperationalStateCluster
{
public:
    OvenCavityOperationalStateCluster(EndpointId endpointId, OperationalState::Delegate * delegate,
                                      const OperationalState::OperationalStateCluster::Config & config = {});

    CHIP_ERROR AcceptedCommands(const ConcreteClusterPath & path,
                                ReadOnlyBufferBuilder<DataModel::AcceptedCommandEntry> & builder) override;
};

/**
 * Backward compatibility wrapper for OvenCavityOperationalState::Instance.
 */
class Instance : public OvenCavityOperationalStateCluster
{
public:
    Instance(OperationalState::Delegate * aDelegate, EndpointId aEndpointId) :
        OvenCavityOperationalStateCluster(aEndpointId, aDelegate)
    {}
    CHIP_ERROR Init() { return CHIP_NO_ERROR; }
};

} // namespace OvenCavityOperationalState

} // namespace Clusters
} // namespace app
} // namespace chip
