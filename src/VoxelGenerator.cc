//------------------------------------------------------------------------------
//  VoxelGenerator.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Assertion.h"
#include "glm/vec2.hpp"
#include "glm/gtc/noise.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/trigonometric.hpp"
#include "Core/Memory/Memory.h"
#include "VoxelGenerator.h"
#include "Volume.h"
#include <algorithm>
#include "Perlin.h"

using namespace Oryol;

/**
 * Generates data using Perlin noise.
 */
class PerlinNoisePager: public PagedVolume<MaterialDensityPair44>::Pager {
public:
    PerlinNoisePager() :
            PagedVolume<MaterialDensityPair44>::Pager() {
    }

    virtual ~PerlinNoisePager() {
    }

    virtual void pageIn(const PolyVox::Region& region, PagedVolume<MaterialDensityPair44>::Chunk* pChunk) override {
        Perlin perlin(2, 2, 1, 234);

        for (int x = region.getLowerX(); x <= region.getUpperX(); x++) {
            for (int y = region.getLowerY(); y <= region.getUpperY(); y++) {
                float perlinVal = perlin.Get(x / static_cast<float>(255 - 1), y / static_cast<float>(255 - 1));
                perlinVal += 1.0f;
                perlinVal *= 0.5f;
                perlinVal *= 255;
                for (int z = region.getLowerZ(); z <= region.getUpperZ(); z++) {
                    MaterialDensityPair44 voxel;
                    if (z < perlinVal) {
                        const int xpos = 50;
                        const int zpos = 100;
                        if ((x - xpos) * (x - xpos) + (z - zpos) * (z - zpos) < 200) {
                            // tunnel
                            voxel.setMaterial(0);
                            voxel.setDensity(MaterialDensityPair44::getMinDensity());
                        } else {
                            // solid
                            voxel.setMaterial(245);
                            voxel.setDensity(MaterialDensityPair44::getMaxDensity());
                        }
                    } else {
                        voxel.setMaterial(0);
                        voxel.setDensity(MaterialDensityPair44::getMinDensity());
                    }

                    // Voxel position within a chunk always start from zero. So if a chunk represents region (4, 8, 12) to (11, 19, 15)
                    // then the valid chunk voxels are from (0, 0, 0) to (7, 11, 3). Hence we subtract the lower corner position of the
                    // region from the volume space position in order to get the chunk space position.
                    pChunk->setVoxel(x - region.getLowerX(), y - region.getLowerY(), z - region.getLowerZ(), voxel);
                }
            }
        }
    }

    virtual void pageOut(const Region& region, PagedVolume<MaterialDensityPair44>::Chunk* /*pChunk*/) override {
        const Vector3DInt32& lower = region.getLowerCorner();
        const Vector3DInt32& upper = region.getUpperCorner();
        Log::Info("warning unloading region: %i:%i:%i -> %i:%i:%i\n", lower.getX(), lower.getY(), lower.getZ(), upper.getX(), upper.getY(), upper.getZ());
    }
};

//------------------------------------------------------------------------------
VoxelGenerator::VoxelGenerator() :
        volData(new PerlinNoisePager(), 256 * 1024 * 1024, Config::ChunkSizeXY) {
}

//------------------------------------------------------------------------------
Volume
VoxelGenerator::initVolume() {
    Volume vol;
    vol.volData = &volData;
    vol.ArraySizeX = vol.ArraySizeY = VolumeSizeXY;
    vol.ArraySizeZ = VolumeSizeZ;
    vol.SizeX = vol.SizeY = Config::ChunkSizeXY;
    vol.SizeZ = Config::ChunkSizeZ;
    vol.OffsetX = vol.OffsetY = vol.OffsetZ = 1;
    return vol;
}

//------------------------------------------------------------------------------
Volume
VoxelGenerator::GenSimplex(const VisBounds& bounds) {
    PolyVox::Region reg(Vector3DInt32(bounds.x0, bounds.y0, 0), Vector3DInt32(bounds.x1, bounds.y1, VolumeSizeZ));
    const Vector3DInt32& lower = reg.getLowerCorner();
    const Vector3DInt32& upper = reg.getUpperCorner();
    Log::Dbg("Prefetch region: %i:%i:%i -> %i:%i:%i\n", lower.getX(), lower.getY(), lower.getZ(), upper.getX(), upper.getY(), upper.getZ());
    volData.prefetch(reg);
    Log::Dbg("Memory usage: %fMB\n", volData.calculateSizeInBytes() / 1024.0 / 1024.0);
    Volume vol = this->initVolume();
    return vol;
}

//------------------------------------------------------------------------------
void
VoxelGenerator::createCirclePlane(const glm::ivec3& center, int width, int depth, double radius, Volume& volume) {
    const double xRadius = (width - 1) / 2.0;
    const double yRadius = (depth - 1) / 2.0;
    const double minRadius = std::min(xRadius, yRadius);
    const double xRatio = xRadius / (float) minRadius;
    const double zRatio = yRadius / (float) minRadius;

    for (double y = -yRadius; y <= yRadius; ++y) {
        for (double x = -xRadius; x <= xRadius; ++x) {
            const double xP = x / xRatio;
            const double zP = y / zRatio;
            const double distance = sqrt(pow(xP, 2) + pow(zP, 2));
            if (distance < radius) {
                const int _x = center.x + x;
                const int _y = center.y + y;
                const int _z = center.z;
                if (_x < VolumeSizeXY && _z < VolumeSizeZ) {
                    //this->volData.setVoxel_x, _y, _z, 1);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
void
VoxelGenerator::createCube(const glm::ivec3& pos, int width, int height, int depth, Volume& volume) {
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            for (int z = 0; z < depth; ++z) {
                const int _x = pos.x + x;
                const int _y = pos.y + y;
                const int _z = pos.z + z;
                if (_x < VolumeSizeXY && _y < VolumeSizeXY && _z < VolumeSizeZ) {
                	//this->volData.setVoxel_x, _y, _z, 1);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
void
VoxelGenerator::createPlane(const glm::ivec3& pos, int width, int depth, Volume& volume) {
    createCube(pos, width, 1, depth, volume);
}

//------------------------------------------------------------------------------
void
VoxelGenerator::createEllipse(const glm::ivec3& pos, int width, int height, int depth, Volume& volume) {
    const double heightRadius = (height - 1.0) / 2.0;
    const double minDimension = std::min(width, depth);
    const double adjustedMinRadius = (minDimension - 1.0) / 2.0;
    const double heightFactor = heightRadius / adjustedMinRadius;
    for (double z = -heightRadius; z <= heightRadius; ++z) {
        const double adjustedHeight = abs(z / heightFactor);
        const double circleRadius = sqrt(pow(adjustedMinRadius + 0.5, 2.0) - pow(adjustedHeight, 2.0));
        const glm::ivec3 planePos(pos.x, pos.y, pos.z + z);
        createCirclePlane(planePos, width, depth, circleRadius, volume);
    }
}

//------------------------------------------------------------------------------
void
VoxelGenerator::createCone(const glm::ivec3& pos, int width, int height, int depth, Volume& volume) {
    const double heightRadius = height - 0.5;
    const double minDimension = std::min(width, depth);
    const double minRadius = minDimension / 2.0;
    for (double z = 0.5; z <= heightRadius; z++) {
        const double percent = 1 - (z / height);
        const double circleRadius = percent * minRadius;
        const glm::ivec3 planePos(pos.x, pos.y, pos.z + z);
        createCirclePlane(planePos, width, depth, circleRadius, volume);
    }
}

//------------------------------------------------------------------------------
void
VoxelGenerator::createDome(const glm::ivec3& pos, int width, int height, int depth, Volume& volume) {
    // TODO:
}

//------------------------------------------------------------------------------
void
VoxelGenerator::addTree(const glm::ivec3& pos, TreeType type, int trunkHeight, Volume& volume) {
    const int top = (int) pos.z + trunkHeight;
    const int sizeX = VolumeSizeXY;
    const int sizeY = VolumeSizeXY;
    const int sizeZ = VolumeSizeZ;

    Log::Dbg("generate tree at %i:%i:%i\n", pos.x, pos.z, pos.y);

    for (int z = pos.z; z < top; ++z) {
        const int width = std::max(1, 3 - (z - pos.z));
        for (int x = pos.x - width; x < pos.x + width; ++x) {
            for (int y = pos.y - width; y < pos.y + width; ++y) {
                if ((x >= pos.x + 1 || x < pos.x - 1) && (y >= pos.y + 1 || y < pos.y - 1))
                    continue;
                if (x < 0 || z < 0 || y < 0 || x >= sizeX || z >= sizeY || y >= sizeZ)
                    continue;

                //this->volData.setVoxel_x, _y, _z, 1);
            }
        }
    }

    const int width = 16;
    const int depth = 16;
    const int height = 12;
    if (type == TreeType::ELLIPSIS) {
        const int centerLeavesPos = top + height / 2;
        const glm::ivec3 leafesPos(pos.x, pos.y, centerLeavesPos);
        createEllipse(leafesPos, width, height, depth, volume);
    } else if (type == TreeType::CONE) {
        const glm::ivec3 leafesPos(pos.x, pos.y, top);
        createCone(leafesPos, width, height, depth, volume);
    } else if (type == TreeType::DOME) {
        const glm::ivec3 leafesPos(pos.x, pos.y, top);
        createDome(leafesPos, width, height, depth, volume);
    }
}

//------------------------------------------------------------------------------
void
VoxelGenerator::createTrees(Volume& volume) {
#if 0
    Log::Dbg("generate trees\n");
    glm::ivec3 pos(volume.SizeX / 2, volume.SizeY / 2, -1);
    for (int i = VolumeSizeZ - 1; i > 0; --i) {
        if (this->volData.getVoxel(pos.x, pos.y, i) != 0) {
            pos.z = i + 1;
            break;
        }
    }
    if (pos.z == -1)
        return;
    addTree(pos, TreeType::ELLIPSIS, 6, volume);
#endif
}

//------------------------------------------------------------------------------
void
VoxelGenerator::createClouds(Volume& volume) {
#if 0
    Log::Dbg("generate clouds\n");
    const int height = 10;
    glm::ivec3 pos(volume.SizeX / 2, volume.SizeY / 2, VolumeSizeZ - 3);
    createEllipse(pos, 10, height, 10, volume);
    pos.x -= 5;
    pos.y -= 5;
    createEllipse(pos, 20, height, 35, volume);
#endif
}
