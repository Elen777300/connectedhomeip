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

#include <device/types/device-energy-management/EnergyManagement.h>

namespace chip::app {

/// Energy Management device whose ESA state is driven by the adjustment commands:
/// power adjustment moves the state between Online and PowerAdjustActive.
class SimulatedEnergyManagement : public Clusters::DeviceEnergyManagement::Delegate, public EnergyManagement
{
public:
    using Config  = EnergyManagement::Config;
    using Feature = EnergyManagement::Feature;

    explicit SimulatedEnergyManagement(TimerDelegate & timerDelegate, const Config & config = {});
    ~SimulatedEnergyManagement() override = default;

    void Unregister(CodeDrivenDataModelProvider & provider) override;

    // Clusters::DeviceEnergyManagement::Delegate
    Protocols::InteractionModel::Status PowerAdjustRequest(const int64_t power, const uint32_t duration,
                                                           Clusters::DeviceEnergyManagement::AdjustmentCauseEnum cause) override;
    Protocols::InteractionModel::Status CancelPowerAdjustRequest() override;
    Protocols::InteractionModel::Status
    StartTimeAdjustRequest(const uint32_t requestedStartTime, Clusters::DeviceEnergyManagement::AdjustmentCauseEnum cause) override;
    Protocols::InteractionModel::Status PauseRequest(const uint32_t duration,
                                                     Clusters::DeviceEnergyManagement::AdjustmentCauseEnum cause) override;
    Protocols::InteractionModel::Status ResumeRequest() override;
    Protocols::InteractionModel::Status ModifyForecastRequest(
        const uint32_t forecastID,
        const DataModel::DecodableList<Clusters::DeviceEnergyManagement::Structs::SlotAdjustmentStruct::Type> & slotAdjustments,
        Clusters::DeviceEnergyManagement::AdjustmentCauseEnum cause) override;
    Protocols::InteractionModel::Status RequestConstraintBasedForecast(
        const DataModel::DecodableList<Clusters::DeviceEnergyManagement::Structs::ConstraintsStruct::Type> & constraints,
        Clusters::DeviceEnergyManagement::AdjustmentCauseEnum cause) override;
    Protocols::InteractionModel::Status CancelRequest() override;
    Protocols::InteractionModel::Status
    PowerRangeAdjustRequest(const Optional<int64_t> minPower, const Optional<int64_t> maxPower, const uint32_t duration,
                            Clusters::DeviceEnergyManagement::AdjustmentCauseEnum cause) override;
    Protocols::InteractionModel::Status CancelPowerRangeAdjustRequest() override;

    Clusters::DeviceEnergyManagement::ESATypeEnum GetESAType() override;
    bool GetESACanGenerate() override;
    Clusters::DeviceEnergyManagement::ESAStateEnum GetESAState() override;
    int64_t GetAbsMinPower() override;
    int64_t GetAbsMaxPower() override;
    Clusters::DeviceEnergyManagement::OptOutStateEnum GetOptOutState() override;
    const DataModel::Nullable<Clusters::DeviceEnergyManagement::Structs::PowerAdjustCapabilityStruct::Type> &
    GetPowerAdjustmentCapability() override;
    const DataModel::Nullable<Clusters::DeviceEnergyManagement::Structs::ForecastStruct::Type> & GetForecast() override;
    const DataModel::Nullable<Clusters::DeviceEnergyManagement::Structs::PowerRangeAdjustStruct::Type> &
    GetPowerRangeAdjustment() override;
    CHIP_ERROR SetESAState(Clusters::DeviceEnergyManagement::ESAStateEnum state) override;

private:
    CHIP_ERROR SetPowerAdjustmentCause(Clusters::DeviceEnergyManagement::PowerAdjustReasonEnum cause);

    Clusters::DeviceEnergyManagement::ESAStateEnum mESAState = Clusters::DeviceEnergyManagement::ESAStateEnum::kOnline;
    DataModel::Nullable<Clusters::DeviceEnergyManagement::Structs::PowerAdjustCapabilityStruct::Type> mPowerAdjustmentCapability;
    DataModel::Nullable<Clusters::DeviceEnergyManagement::Structs::ForecastStruct::Type> mForecast;
    DataModel::Nullable<Clusters::DeviceEnergyManagement::Structs::PowerRangeAdjustStruct::Type> mPowerRangeAdjustment;
};

} // namespace chip::app
