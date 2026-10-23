#pragma once
/*
    AU channel layouts <-> CLAP channel maps

    An AU channel layout names the speaker of each channel in the layout's own order,
    by a layout tag, a bitmap or channel labels. A CLAP channel map lists the
    CLAP_SURROUND_* position of each channel. The wrapper hands a plugin its channels
    in the order of their CLAP positions, which is the order a VST3 host gives them
    (see src/detail/vst3/channelmap.h), so a plugin sees every layout the same way in
    both formats. An AU layout in another order -- Atmos 7.1.4 puts the side pair
    before the rear pair -- is put into that order by handing the plugin the AU's
    channel pointers rearranged, which copies nothing.

    The surround labels say "surround" for both the side pair of 7.1 and the rear
    pair of 5.1, as the AU layouts name them: Ls and Rs are the rear pair (CLAP's
    back left and right) unless the layout has a rear pair of its own, Rls and Rrs,
    which makes them the side pair.
*/

#include <AudioToolbox/AudioToolbox.h>

#include <cstdint>
#include <vector>

namespace Clap::AUv2::ChannelLayouts
{
// The CLAP position of each channel of a layout, in the layout's own channel order.
// Returns false when a channel's speaker has no CLAP position, or when two channels
// have the same one. The wide pair has the positions of clapwrapper/wide_speakers.h,
// only for a plugin that takes them.
bool positionsOf(const AudioChannelLayout &layout, std::vector<uint8_t> &positions,
                 bool wideSpeakers = false);
bool positionsOf(AudioChannelLayoutTag tag, std::vector<uint8_t> &positions, bool wideSpeakers = false);

// The layout tags a plugin's main busses are asked about, mono to Atmos 9.1.6, several
// of them in more than one channel order, as hosts such as Logic use them.
const std::vector<AudioChannelLayoutTag> &candidateTags();

// Positions put in CLAP's order, and for each channel in that order the channel of the
// layout it came from. \p from is left empty when the order is already CLAP's.
void inClapOrder(const std::vector<uint8_t> &positions, std::vector<uint8_t> &ordered,
                 std::vector<uint32_t> &from);
}  // namespace Clap::AUv2::ChannelLayouts
