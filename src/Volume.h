#pragma once
//------------------------------------------------------------------------------
/**
    @class Volume
    @brief a chunk of voxels in a big 3D array
*/
#include "Core/Types.h"
#include "glm/vec3.hpp"
#include "PolyVox/MaterialDensityPair.h"
#include "PolyVox/CubicSurfaceExtractor.h"
#include "PolyVox/MarchingCubesSurfaceExtractor.h"
#include "PolyVox/Mesh.h"
#include "PolyVox/PagedVolume.h"

using namespace PolyVox;

struct Volume {
    typedef PagedVolume<MaterialDensityPair44> VolData;
    VolData* volData = nullptr;

    int ArraySizeX = 0;
    int ArraySizeY = 0;
    int ArraySizeZ = 0;

    int OffsetX = 0;
    int OffsetY = 0;
    int OffsetZ = 0;

    int SizeX = 0;
    int SizeY = 0;
    int SizeZ = 0;

    int TranslationX = 0;
    int TranslationY = 0;
    int TranslationZ = 0;
};
