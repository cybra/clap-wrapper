#ifndef CLAPWRAPPER_WIDE_SPEAKERS_H
#define CLAPWRAPPER_WIDE_SPEAKERS_H

#include <stdint.h>

#include "clap/private/macros.h"

/*
  clap_wrapper_wide_speakers

  CLAP's surround positions end at CLAP_SURROUND_TSR and have no wide left and right
  speakers, which Dolby's 9.1.4 and 9.1.6 have and which VST3 (Lw, Rw) and AU
  (LeftWide, RightWide) name. A wrapper whose format names them offers this extension
  as a host, and a plugin that takes them offers it too. Only when both do may a channel
  map hold CLAP_WRAPPER_SURROUND_LW and CLAP_WRAPPER_SURROUND_RW: in a configuration
  request, from get_channel_map and in is_channel_mask_supported's mask. A plugin in a
  host without the extension never uses them, so a CLAP host never sees them.

  The extension has no functions; offering it is all it says.

    const bool wides = host->get_extension(host, CLAP_WRAPPER_WIDE_SPEAKERS) != nullptr;
*/

#ifdef __cplusplus
extern "C"
{
#endif

  static const CLAP_CONSTEXPR char CLAP_WRAPPER_WIDE_SPEAKERS[] = "clap-wrapper.wide-speakers/0";

  enum
  {
    CLAP_WRAPPER_SURROUND_LW = 20,  // Wide Left
    CLAP_WRAPPER_SURROUND_RW = 21,  // Wide Right
  };

  typedef struct clap_wrapper_wide_speakers
  {
    // unused: a pointer to this is what the host or the plugin offers
    uint32_t reserved;
  } clap_wrapper_wide_speakers_t;

#ifdef __cplusplus
}
#endif

#endif
