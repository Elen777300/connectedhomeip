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

#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>
#include <device/api/Interface.h>
#include <device/api/allocator/EndpointIdAllocator.h>

namespace chip::app {

/// Battery Storage device type (0x0018) rev 2 parent endpoint.
///
/// The composed endpoints (Device Energy Management, Electrical Sensor and Power Source pairs)
/// are registered by the implementation through RegisterParts().
class BatteryStorage : public DeviceInterface
{
public:
    ~BatteryStorage() override = default;

    CHIP_ERROR Register(EndpointIdAllocator & allocator, CodeDrivenDataModelProvider & provider,
                        EndpointComposition composition = {}) override;
    void Unregister(CodeDrivenDataModelProvider & provider) override;

    EndpointId GetEndpointId() const { return mEndpointId; }

protected:
    BatteryStorage();

    virtual CHIP_ERROR RegisterParts(EndpointIdAllocator & allocator, CodeDrivenDataModelProvider & provider) = 0;
    virtual void UnregisterParts(CodeDrivenDataModelProvider & provider)                                      = 0;

    virtual CHIP_ERROR RegisterOptionalClusters(EndpointId endpoint, CodeDrivenDataModelProvider & provider)
    {
        return CHIP_NO_ERROR;
    }
    virtual void UnregisterOptionalClusters(CodeDrivenDataModelProvider & provider) {}

    EndpointId mEndpointId = kInvalidEndpointId;
};

} // namespace chip::app
