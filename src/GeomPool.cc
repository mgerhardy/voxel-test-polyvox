//------------------------------------------------------------------------------
//  GeomPool.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "GeomPool.h"
#include "Config.h"
#include "Gfx/Gfx.h"
#include "glm/geometric.hpp"
#include "glm/gtc/random.hpp"

using namespace Oryol;

//------------------------------------------------------------------------------
void
GeomPool::Setup(const GfxSetup& gfxSetup) {

    // setup shader params template
    Shader::VSParams vsParams;
    vsParams.NormalTable[0] = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    vsParams.NormalTable[1] = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    vsParams.NormalTable[2] = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
    vsParams.NormalTable[3] = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
    vsParams.NormalTable[4] = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
    vsParams.NormalTable[5] = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    for (int i = 0; i < int(sizeof(vsParams.ColorTable)/sizeof(glm::vec4)); i++) {
        vsParams.ColorTable[i] = glm::linearRand(glm::vec4(0.25f), glm::vec4(1.0f));
    }

    // setup shader and drawstate
    Id shd = Gfx::CreateResource(Shader::Setup());
    auto pips = PipelineSetup::FromShader(shd);
    pips.Layouts[0]
        .Add(VertexAttr::Position, VertexFormat::UByte4N)
        .Add(VertexAttr::Normal, VertexFormat::UByte4N);
    pips.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    pips.DepthStencilState.DepthWriteEnabled = true;
    pips.RasterizerState.CullFaceEnabled = true;
    pips.RasterizerState.CullFace = Face::Front;
    pips.RasterizerState.SampleCount = gfxSetup.SampleCount;
    this->Pipeline = Gfx::CreateResource(pips);

    // setup items
    auto meshSetup = MeshSetup::Empty(Config::GeomMaxNumVertices, Usage::Dynamic, IndexType::Index32, Config::GeomMaxNumIndices, Usage::Dynamic);
    meshSetup.Layout = pips.Layouts[0];
    for (auto& geom : this->Geoms) {
        geom.VSParams = vsParams;
        geom.NumVertices = 0;
        geom.Mesh = Gfx::CreateResource(meshSetup);
    }
    this->freeGeoms.Reserve(NumGeoms);
    this->FreeAll();
}

//------------------------------------------------------------------------------
void
GeomPool::Discard() {
    this->Pipeline.Invalidate();
    this->freeGeoms.Clear();
}

//------------------------------------------------------------------------------
void
GeomPool::FreeAll() {
    this->freeGeoms.Clear();
    for (int i = 0; i < NumGeoms; i++) {
        this->freeGeoms.Add(i);
    }
}
