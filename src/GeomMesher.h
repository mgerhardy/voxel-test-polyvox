#pragma once
//------------------------------------------------------------------------------
/**
    @class GeomMesher
    @brief meshify volumes into geoms
*/
#include "Volume.h"
#include "GeomPool.h"
#include "Config.h"
#include "glm/vec3.hpp"

using namespace Oryol;

class GeomMesher {
public:
    typedef Mesh<Vertex<MaterialDensityPair44>, DefaultIndexType> Result;

    /// setup the geom mesher
    void Setup();
    /// discard the geom mesher
    void Discard();

    /// start meshifying, resets the stbox mesh maker
    void Start();
    /// do one meshify pass, continue to call until VolumeDone
    Result Meshify(const Volume& vol, const glm::vec3& translate, const glm::vec3& scale);
};
