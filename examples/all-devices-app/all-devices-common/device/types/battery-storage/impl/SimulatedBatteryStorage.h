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

#pragma once

#include <app/TestEventTriggerDelegate.h>
#include <app/clusters/identify-server/IdentifyCluster.h>
#include <device/types/battery-storage/BatteryStorage.h>
#include <device/types/device-energy-management/impl/SimulatedEnergyManagement.h>
#include <device/types/electrical-sensor/impl/SimulatedElectricalSensor.h>
#include <device/types/power-source/BatteryPowerSource.h>
#include <device/types/power-source/WiredPowerSource.h>
#include <lib/support/TimerDelegate.h>

namespace chip::app {

/// Simulated Battery Storage: parent endpoint with Identify, plus the mandatory composed
/// Device Energy Management, Electrical Sensor and Power Source endpoint pairs.
class SimulatedBatteryStorage : public BatteryStorage
{
public:
    struct Context
    {
        TimerDelegate & timerDelegate;
        TestEventTriggerDelegate & testEventTriggerDelegate;
    };

    explicit SimulatedBatteryStorage(const Context & context);
    ~SimulatedBatteryStorage() override = default;

    Clusters::IdentifyCluster & IdentifyCluster() { return mIdentifyCluster.Cluster(); }

    SimulatedEnergyManagement & GetEnergyManagement() { return mEnergyManagement; }
    SimulatedElectricalSensor & GetGridElectricalSensor() { return mGridElectricalSensor; }
    SimulatedElectricalSensor & GetBatteryElectricalSensor() { return mBatteryElectricalSensor; }
    WiredPowerSource & GetGridPowerSource() { return mGridPowerSource; }
    BatteryPowerSource & GetBatteryPowerSource() { return mBatteryPowerSource; }

protected:
    CHIP_ERROR RegisterParts(EndpointIdAllocator & allocator, CodeDrivenDataModelProvider & provider) override;
    void UnregisterParts(CodeDrivenDataModelProvider & provider) override;

    CHIP_ERROR RegisterOptionalClusters(EndpointId endpoint, CodeDrivenDataModelProvider & provider) override;
    void UnregisterOptionalClusters(CodeDrivenDataModelProvider & provider) override;

private:
    TimerDelegate & mTimerDelegate;

    SimulatedEnergyManagement mEnergyManagement;
    SimulatedElectricalSensor mGridElectricalSensor;
    SimulatedElectricalSensor mBatteryElectricalSensor;
    WiredPowerSource mGridPowerSource;
    BatteryPowerSource mBatteryPowerSource;

    LazyRegisteredServerCluster<Clusters::IdentifyCluster> mIdentifyCluster;
};

} // namespace chip::app
