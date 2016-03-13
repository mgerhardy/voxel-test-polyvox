//------------------------------------------------------------------------------
//  GeomMesher.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "GeomMesher.h"

//------------------------------------------------------------------------------
void
GeomMesher::Setup() {
}

//------------------------------------------------------------------------------
void
GeomMesher::Discard() {
    // nothing to so here
}

//------------------------------------------------------------------------------
void
GeomMesher::Start() {
}

//------------------------------------------------------------------------------
GeomMesher::Result
GeomMesher::Meshify(const Volume& vol, const glm::vec3& translate, const glm::vec3& scale) {
    PolyVox::Region region(Vector3DInt32(translate.x, translate.y, translate.z), Vector3DInt32(translate.x + Config::ChunkSizeXY, translate.y + Config::ChunkSizeXY, translate.z + Config::ChunkSizeZ));
    const Vector3DInt32& lower = region.getLowerCorner();
    const Vector3DInt32& upper = region.getUpperCorner();
    Log::Info("meshify region: %i:%i:%i -> %i:%i:%i\n", lower.getX(), lower.getY(), lower.getZ(), upper.getX(), upper.getY(), upper.getZ());
    const auto& mesh = extractCubicMesh(vol.volData, region);
    const Result& decodedMesh = decodeMesh(mesh);
    Log::Info("Indices: %i, vertices: %i\n", decodedMesh.getNoOfIndices(), decodedMesh.getNoOfVertices());
    return decodedMesh;
}
