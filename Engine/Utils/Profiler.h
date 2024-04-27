#include <tracy/Tracy.hpp>

#define FRAME_START() FrameMarkStart("Frame");
#define FRAME_END() FrameMarkEnd("Frame");

#define PROFILER_SCOPE(name) ZoneScopedN(name)