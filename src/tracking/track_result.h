#pragma once

#include "track_types.h"

#include "../core/types/id.h"

#include <vector>


namespace visionlab::tracking {


struct TrackResult
{

    core::types::Id frame_id;

    std::vector<Track> tracks;

};


}
