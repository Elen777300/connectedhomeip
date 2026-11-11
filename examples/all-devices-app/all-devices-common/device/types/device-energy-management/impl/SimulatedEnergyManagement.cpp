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

#include <device/types/device-energy-management/impl/SimulatedEnergyManagement.h>

using namespace chip::app::Clusters::DeviceEnergyManagement;

namespace {

constexpr Structs::PowerAdjustStruct::Type kDefaultPowerAdjustEntry = {
    .minPower    = 0,
    .maxPower    = 1000,
    .minDuration = 0,
    .maxDuration = 3600,
};

} // namespace

namespace chip::app {

SimulatedEnergyManagement::SimulatedEnergyManagement(TimerDelegate & timerDelegate, const Config & config) :
    EnergyManagement(EnergyManagement::Context{ timerDelegate, *this }, config)
{
    if (!GetConfig().featureFlags.Has(Feature::kPowerAdjustment))
    {
        return;
    }

    Structs::PowerAdjustCapabilityStruct::Type capabilityStruct;
    capabilityStruct.powerAdjustCapability.SetNonNull(
        DataModel::List<const Structs::PowerAdjustStruct::Type>(&kDefaultPowerAdjustEntry, 1));
    capabilityStruct.cause = PowerAdjustReasonEnum::kNoAdjustment;
    mPowerAdjustmentCapability.SetNonNull(capabilityStruct);
}

void SimulatedEnergyManagement::Unregister(CodeDrivenDataModelProvider & provider)
{
    EnergyManagement::Unregister(provider);

    mESAState = ESAStateEnum::kOnline;
    if (!mPowerAdjustmentCapability.IsNull())
    {
        mPowerAdjustmentCapability.Value().cause = PowerAdjustReasonEnum::kNoAdjustment;
    }
}

Protocols::InteractionModel::Status SimulatedEnergyManagement::PowerAdjustRequest(const int64_t power, const uint32_t duration,
                                                                                  AdjustmentCauseEnum cause)
{
    VerifyOrReturnValue(SetESAState(ESAStateEnum::kPowerAdjustActive) == CHIP_NO_ERROR,
                        Protocols::InteractionModel::Status::Failure);
    VerifyOrReturnValue(SetPowerAdjustmentCause(AdjustmentCauseToPowerAdjustReason(cause)) == CHIP_NO_ERROR,
                        Protocols::InteractionModel::Status::Failure);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status SimulatedEnergyManagement::CancelPowerAdjustRequest()
{
    VerifyOrReturnValue(SetESAState(ESAStateEnum::kOnline) == CHIP_NO_ERROR, Protocols::InteractionModel::Status::Failure);
    VerifyOrReturnValue(SetPowerAdjustmentCause(PowerAdjustReasonEnum::kNoAdjustment) == CHIP_NO_ERROR,
                        Protocols::InteractionModel::Status::Failure);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status SimulatedEnergyManagement::StartTimeAdjustRequest(const uint32_t requestedStartTime,
                                                                                      AdjustmentCauseEnum cause)
{
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status SimulatedEnergyManagement::PauseRequest(const uint32_t duration, AdjustmentCauseEnum cause)
{
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status SimulatedEnergyManagement::ResumeRequest()
{
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status SimulatedEnergyManagement::ModifyForecastRequest(
    const uint32_t forecastID, const DataModel::DecodableList<Structs::SlotAdjustmentStruct::Type> & slotAdjustments,
    AdjustmentCauseEnum cause)
{
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status SimulatedEnergyManagement::RequestConstraintBasedForecast(
    const DataModel::DecodableList<Structs::ConstraintsStruct::Type> & constraints, AdjustmentCauseEnum cause)
{
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status SimulatedEnergyManagement::CancelRequest()
{
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status SimulatedEnergyManagement::PowerRangeAdjustRequest(const Optional<int64_t> minPower,
                                                                                       const Optional<int64_t> maxPower,
                                                                                       const uint32_t duration,
                                                                                       AdjustmentCauseEnum cause)
{
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status SimulatedEnergyManagement::CancelPowerRangeAdjustRequest()
{
    return Protocols::InteractionModel::Status::Success;
}

ESATypeEnum SimulatedEnergyManagement::GetESAType()
{
    return GetConfig().esaType;
}

bool SimulatedEnergyManagement::GetESACanGenerate()
{
    return GetConfig().esaCanGenerate;
}

ESAStateEnum SimulatedEnergyManagement::GetESAState()
{
    return mESAState;
}

int64_t SimulatedEnergyManagement::GetAbsMinPower()
{
    return kDefaultPowerAdjustEntry.minPower;
}

int64_t SimulatedEnergyManagement::GetAbsMaxPower()
{
    return kDefaultPowerAdjustEntry.maxPower;
}

OptOutStateEnum SimulatedEnergyManagement::GetOptOutState()
{
    return OptOutStateEnum::kNoOptOut;
}

const DataModel::Nullable<Structs::PowerAdjustCapabilityStruct::Type> & SimulatedEnergyManagement::GetPowerAdjustmentCapability()
{
    return mPowerAdjustmentCapability;
}

const DataModel::Nullable<Structs::ForecastStruct::Type> & SimulatedEnergyManagement::GetForecast()
{
    return mForecast;
}

const DataModel::Nullable<Structs::PowerRangeAdjustStruct::Type> & SimulatedEnergyManagement::GetPowerRangeAdjustment()
{
    return mPowerRangeAdjustment;
}

CHIP_ERROR SimulatedEnergyManagement::SetESAState(ESAStateEnum state)
{
    VerifyOrReturnValue(mESAState != state, CHIP_NO_ERROR);
    mESAState = state;
    return NotifyAttributeChanged(Attributes::ESAState::Id);
}

CHIP_ERROR SimulatedEnergyManagement::SetPowerAdjustmentCause(PowerAdjustReasonEnum cause)
{
    VerifyOrReturnError(!mPowerAdjustmentCapability.IsNull(), CHIP_ERROR_INCORRECT_STATE);
    VerifyOrReturnValue(mPowerAdjustmentCapability.Value().cause != cause, CHIP_NO_ERROR);
    mPowerAdjustmentCapability.Value().cause = cause;
    return NotifyAttributeChanged(Attributes::PowerAdjustmentCapability::Id);
}

} // namespace chip::app
