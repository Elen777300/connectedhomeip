/*
 *
 *    Copyright (c) 2025 Project CHIP Authors
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

#include <device/types/chime/impl/LoggingChime.h>
#include <lib/support/logging/CHIPLogging.h>

namespace chip {
namespace app {

LoggingChime::LoggingChime(TimerDelegate & timerDelegate, Span<const Sound> sounds) :
    Chime(timerDelegate, static_cast<Clusters::ChimeDelegate &>(*this)), mSounds(sounds)
{}

CHIP_ERROR LoggingChime::GetChimeSoundByIndex(uint8_t chimeIndex, uint8_t & chimeID, MutableCharSpan & name)
{
    if (chimeIndex >= mSounds.size())
    {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }

    const auto & sound = mSounds[chimeIndex];
    chimeID            = sound.id;
    return CopyCharSpanToMutableCharSpan(sound.name, name);
}

CHIP_ERROR LoggingChime::GetChimeIDByIndex(uint8_t chimeIndex, uint8_t & chimeID)
{
    if (chimeIndex >= mSounds.size())
    {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }

    chimeID = mSounds[chimeIndex].id;
    return CHIP_NO_ERROR;
}

Protocols::InteractionModel::Status LoggingChime::PlayChimeSound(uint8_t chimeID)
{
    CharSpan soundName = "Unknown"_span;
    for (const auto & sound : mSounds)
    {
        if (sound.id == chimeID)
        {
            soundName = sound.name;
            break;
        }
    }

    ChipLogProgress(AppServer, "Chime: Playing sound %s", chip::NullTerminated(soundName).c_str());

    return Protocols::InteractionModel::Status::Success;
}

} // namespace app
} // namespace chip
