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
#include "LoggingAmbientContextSensor.h"
#include <lib/support/BitFlags.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>

namespace chip::app::Clusters::AmbientContextSensing {

LoggingAmbientContextSensor::LoggingAmbientContextSensor(TimerDelegate & timerDelegate) :
    AmbientContextSensor(AmbientContextSensor::Config{
        .timerDelegate              = timerDelegate,
        .serviceAreaDelegate        = *this,
        .serviceAreaStorageDelegate = *this,
    }),
    mAmbientContextTypeSupportedBuf{}, mPredictActivityBuf{}, mSensorFusionSupportedBuf{}
{}

CHIP_ERROR LoggingAmbientContextSensor::RegisterOptionalClusters(EndpointId endpoint, CodeDrivenDataModelProvider & provider)
{
    Clusters::AmbientContextSensingCluster::Config ambientContextConfig(mConfig.timerDelegate);
    ambientContextConfig.WithFeatures(BitMask<AmbientContextSensing::Feature>(kFeatureAllForLog))
        .WithHoldTime(10, { .holdTimeMin = 1, .holdTimeMax = 300, .holdTimeDefault = 10 });

    mAmbientContextSensingCluster.Create(endpoint, ambientContextConfig);
    AmbientContextSensingCluster().SetDelegate(this);
    ReturnErrorOnFailure(provider.AddCluster(mAmbientContextSensingCluster.Registration()));

    mAmbientSensingUnionCluster.Create(Clusters::AmbientSensingUnionCluster::Config(endpoint)
                                           .WithUnionName("all-devices-sensing-union"_span)
                                           .WithUnionHealth(Clusters::AmbientSensingUnion::UnionHealthEnum::kFullyFunctional));
    ReturnErrorOnFailure(mAmbientSensingUnionCluster.Cluster().AddMatterContributor(
        1, 1, Clusters::AmbientSensingUnion::UnionContributorStatusEnum::kUnionContributorOnline, FabricIndex{ 1 }));
    ReturnErrorOnFailure(provider.AddCluster(mAmbientSensingUnionCluster.Registration()));

    return CHIP_NO_ERROR;
}

void LoggingAmbientContextSensor::UnregisterOptionalClusters(CodeDrivenDataModelProvider & provider)
{
    if (mAmbientSensingUnionCluster.IsConstructed())
    {
        LogErrorOnFailure(provider.RemoveCluster(&mAmbientSensingUnionCluster.Cluster()));
        mAmbientSensingUnionCluster.Destroy();
    }
    if (mAmbientContextSensingCluster.IsConstructed())
    {
        LogErrorOnFailure(provider.RemoveCluster(&mAmbientContextSensingCluster.Cluster()));
        mAmbientContextSensingCluster.Destroy();
    }
}

// AmbientContextSensingDelegate implementation
SemanticTagType * LoggingAmbientContextSensor::GetAmbientContextTypeSupportedBuf(size_t size)
{
    VerifyOrReturnError(size <= kMaxACTypeSupportedForLog, nullptr);
    return mAmbientContextTypeSupportedBuf;
}

CHIP_ERROR LoggingAmbientContextSensor::SetPredictedActivity(const Span<PredictedActivityType> & predictedActivityList)
{
    VerifyOrReturnError(predictedActivityList.size() <= kMaxPredictedActivityForLog, CHIP_ERROR_INVALID_ARGUMENT);

    // Copy the input predicted activity to local array
    for (size_t i = 0; i < predictedActivityList.size(); i++)
    {
        const auto & src = predictedActivityList[i];
        auto & dst       = mPredictActivityBuf[i];
        dst.mInfo        = src;

        if (!src.ambientContextType.HasValue())
        {
            continue;
        }

        // Copy tags
        const auto & acTypeList = src.ambientContextType.Value();
        const auto tagCount     = acTypeList.size();
        VerifyOrReturnError(tagCount <= kMaxPredictedACType, CHIP_ERROR_INVALID_ARGUMENT);
        dst.mOwnedTags = std::make_unique<SemanticTagType[]>(tagCount);

        for (size_t t = 0; t < tagCount; t++)
        {
            dst.mOwnedTags[t] = acTypeList[t];
        }

        dst.mInfo.ambientContextType.SetValue(
            DataModel::List<const SemanticTagType>(dst.mOwnedTags.get(), static_cast<uint16_t>(tagCount)));
    }

    return CHIP_NO_ERROR;
}

SemanticTagType * LoggingAmbientContextSensor::GetSensorFusionSupportedBuf(size_t size)
{
    VerifyOrReturnError(size <= kMaxSensorFusionSupportedForLog, nullptr);
    return mSensorFusionSupportedBuf;
}

AmbientContextSensed * LoggingAmbientContextSensor::AllocDetection()
{
    for (uint8_t id = 0; id < kMaxSimultaneousDetectionLimit; id++)
    {
        if (mAmbientContextTypeList[id] == nullptr)
        {
            auto ptr                    = std::make_unique<AmbientContextSensed>();
            ptr->id                     = id;
            auto raw                    = ptr.get();
            mAmbientContextTypeList[id] = std::move(ptr);
            return raw;
        }
    }
    return nullptr;
}

CHIP_ERROR LoggingAmbientContextSensor::DelDetection(AmbientContextSensed * pitem)
{
    VerifyOrReturnError(pitem != nullptr, CHIP_ERROR_INVALID_ARGUMENT);
    const uint8_t id = pitem->id;
    VerifyOrReturnError(id < kMaxSimultaneousDetectionLimit, CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError((mAmbientContextTypeList[id] != nullptr) && (mAmbientContextTypeList[id].get() == pitem),
                        CHIP_ERROR_INVALID_ARGUMENT);
    mAmbientContextTypeList[id].reset();

    return CHIP_NO_ERROR;
}

uint64_t LoggingAmbientContextSensor::GetEpochNow()
{
    using namespace chip::System::Clock;
    Milliseconds64 timestamp_ms(0);
    CHIP_ERROR err = System::SystemClock().GetClock_RealTimeMS(timestamp_ms);

    return (err == CHIP_NO_ERROR) ? timestamp_ms.count() : 0;
}

// ServiceArea::Delegate implementation
bool LoggingAmbientContextSensor::IsSetSelectedAreasAllowed(MutableCharSpan & statusText)
{
    return true;
}

bool LoggingAmbientContextSensor::IsValidSelectAreasSet(const Span<const uint32_t> & selectedAreas,
                                                        ServiceArea::SelectAreasStatus & locationStatus,
                                                        MutableCharSpan & statusText)
{
    ServiceArea::AreaStructureWrapper area;
    uint32_t listIndex = 0;
    for (uint32_t areaId : selectedAreas)
    {
        if (!ServiceAreaCluster().GetSupportedAreaById(areaId, listIndex, area))
        {
            locationStatus = ServiceArea::SelectAreasStatus::kUnsupportedArea;
            return false;
        }
    }
    return true;
}

} // namespace chip::app::Clusters::AmbientContextSensing
