#include "channellayouts.h"

#include <clap/clap.h>

#include <algorithm>
#include <cstddef>
#include <numeric>

namespace Clap::AUv2::ChannelLayouts
{
namespace
{
int positionOf(AudioChannelLabel label, bool hasRearPair)
{
  switch (label)
  {
    case kAudioChannelLabel_Left:
      return CLAP_SURROUND_FL;
    case kAudioChannelLabel_Right:
      return CLAP_SURROUND_FR;
    case kAudioChannelLabel_Center:
    case kAudioChannelLabel_Mono:
      return CLAP_SURROUND_FC;
    case kAudioChannelLabel_LFEScreen:
      return CLAP_SURROUND_LFE;
    case kAudioChannelLabel_LeftSurround:
      return hasRearPair ? CLAP_SURROUND_SL : CLAP_SURROUND_BL;
    case kAudioChannelLabel_RightSurround:
      return hasRearPair ? CLAP_SURROUND_SR : CLAP_SURROUND_BR;
    case kAudioChannelLabel_LeftCenter:
      return CLAP_SURROUND_FLC;
    case kAudioChannelLabel_RightCenter:
      return CLAP_SURROUND_FRC;
    case kAudioChannelLabel_CenterSurround:
      return CLAP_SURROUND_BC;
    case kAudioChannelLabel_LeftSurroundDirect:
      return CLAP_SURROUND_SL;
    case kAudioChannelLabel_RightSurroundDirect:
      return CLAP_SURROUND_SR;
    case kAudioChannelLabel_TopCenterSurround:
      return CLAP_SURROUND_TC;
    case kAudioChannelLabel_VerticalHeightLeft:
      return CLAP_SURROUND_TFL;
    case kAudioChannelLabel_VerticalHeightCenter:
      return CLAP_SURROUND_TFC;
    case kAudioChannelLabel_VerticalHeightRight:
      return CLAP_SURROUND_TFR;
    case kAudioChannelLabel_TopBackLeft:
    case kAudioChannelLabel_LeftTopRear:
      return CLAP_SURROUND_TBL;
    case kAudioChannelLabel_TopBackCenter:
    case kAudioChannelLabel_CenterTopRear:
      return CLAP_SURROUND_TBC;
    case kAudioChannelLabel_TopBackRight:
    case kAudioChannelLabel_RightTopRear:
      return CLAP_SURROUND_TBR;
    case kAudioChannelLabel_RearSurroundLeft:
      return CLAP_SURROUND_BL;
    case kAudioChannelLabel_RearSurroundRight:
      return CLAP_SURROUND_BR;
    case kAudioChannelLabel_LeftTopMiddle:
      return CLAP_SURROUND_TSL;
    case kAudioChannelLabel_RightTopMiddle:
      return CLAP_SURROUND_TSR;
    default:
      return -1;
  }
}

// The layout as channel descriptions, which the system gives for a tag or a bitmap.
bool describe(const AudioChannelLayout &layout, std::vector<std::byte> &storage,
              const AudioChannelLayout *&described)
{
  if (layout.mChannelLayoutTag == kAudioChannelLayoutTag_UseChannelDescriptions)
  {
    described = &layout;
    return true;
  }

  const bool bitmap = layout.mChannelLayoutTag == kAudioChannelLayoutTag_UseChannelBitmap;
  const AudioFormatPropertyID property =
      bitmap ? kAudioFormatProperty_ChannelLayoutForBitmap : kAudioFormatProperty_ChannelLayoutForTag;
  const void *specifier = bitmap ? static_cast<const void *>(&layout.mChannelBitmap)
                                 : static_cast<const void *>(&layout.mChannelLayoutTag);
  const UInt32 specifierSize = bitmap ? sizeof(layout.mChannelBitmap) : sizeof(layout.mChannelLayoutTag);

  UInt32 size = 0;
  if (AudioFormatGetPropertyInfo(property, specifierSize, specifier, &size) != noErr ||
      size < offsetof(AudioChannelLayout, mChannelDescriptions))
    return false;
  // AudioChannelLayout's own alignment, which a vector of bytes from new has.
  storage.resize(std::max<size_t>(size, sizeof(AudioChannelLayout)));
  if (AudioFormatGetProperty(property, specifierSize, specifier, &size, storage.data()) != noErr)
    return false;
  described = reinterpret_cast<const AudioChannelLayout *>(storage.data());
  return true;
}
}  // namespace

bool positionsOf(const AudioChannelLayout &layout, std::vector<uint8_t> &positions)
{
  std::vector<std::byte> storage;
  const AudioChannelLayout *described = nullptr;
  if (!describe(layout, storage, described)) return false;

  const auto count = described->mNumberChannelDescriptions;
  const AudioChannelDescription *descriptions = described->mChannelDescriptions;

  bool hasRearPair = false;
  for (UInt32 i = 0; i < count; ++i)
  {
    const auto label = descriptions[i].mChannelLabel;
    hasRearPair |=
        label == kAudioChannelLabel_RearSurroundLeft || label == kAudioChannelLabel_RearSurroundRight;
  }

  positions.clear();
  uint64_t seen = 0;
  for (UInt32 i = 0; i < count; ++i)
  {
    const int position = positionOf(descriptions[i].mChannelLabel, hasRearPair);
    if (position < 0 || ((seen >> position) & 1u)) return false;
    seen |= uint64_t{1} << position;
    positions.push_back(static_cast<uint8_t>(position));
  }
  return !positions.empty();
}

bool positionsOf(AudioChannelLayoutTag tag, std::vector<uint8_t> &positions)
{
  AudioChannelLayout layout{};
  layout.mChannelLayoutTag = tag;
  return positionsOf(layout, positions);
}

const std::vector<AudioChannelLayoutTag> &candidateTags()
{
  static const std::vector<AudioChannelLayoutTag> tags{
      kAudioChannelLayoutTag_Mono,
      kAudioChannelLayoutTag_Stereo,
      kAudioChannelLayoutTag_MPEG_3_0_A,
      kAudioChannelLayoutTag_Quadraphonic,
      kAudioChannelLayoutTag_MPEG_5_0_A,
      kAudioChannelLayoutTag_MPEG_5_0_B,
      kAudioChannelLayoutTag_MPEG_5_0_C,
      kAudioChannelLayoutTag_MPEG_5_0_D,
      kAudioChannelLayoutTag_MPEG_5_1_A,
      kAudioChannelLayoutTag_MPEG_5_1_B,
      kAudioChannelLayoutTag_MPEG_5_1_C,
      kAudioChannelLayoutTag_MPEG_5_1_D,
      kAudioChannelLayoutTag_AudioUnit_7_0,
      kAudioChannelLayoutTag_AudioUnit_7_0_Front,
      kAudioChannelLayoutTag_MPEG_7_1_C,
      kAudioChannelLayoutTag_MPEG_7_1_A,
      kAudioChannelLayoutTag_Logic_7_1_B,
      kAudioChannelLayoutTag_Atmos_5_1_2,
      kAudioChannelLayoutTag_Atmos_5_1_4,
      kAudioChannelLayoutTag_Atmos_7_1_2,
      kAudioChannelLayoutTag_Atmos_7_1_4,
      kAudioChannelLayoutTag_Logic_Atmos_7_1_4_B,
      kAudioChannelLayoutTag_Logic_Atmos_7_1_6,
  };
  return tags;
}

void inClapOrder(const std::vector<uint8_t> &positions, std::vector<uint8_t> &ordered,
                 std::vector<uint32_t> &from)
{
  from.resize(positions.size());
  std::iota(from.begin(), from.end(), 0u);
  std::stable_sort(from.begin(), from.end(),
                   [&positions](uint32_t a, uint32_t b) { return positions[a] < positions[b]; });

  ordered.clear();
  bool same = true;
  for (size_t i = 0; i < from.size(); ++i)
  {
    ordered.push_back(positions[from[i]]);
    same &= from[i] == i;
  }
  if (same) from.clear();
}
}  // namespace Clap::AUv2::ChannelLayouts
