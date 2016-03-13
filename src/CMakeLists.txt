fips_begin_app(VoxelTestPolyVox windowed)
    fips_files(
        Main.cc
        Volume.h Config.h
        VoxelGenerator.h VoxelGenerator.cc
        GeomPool.h GeomPool.cc
        GeomMesher.h GeomMesher.cc
        VisNode.h VisBounds.h
        VisTree.h VisTree.cc
        Perlin.h Perlin.cpp
        Camera.h Camera.cc)
    oryol_shader(shaders.shd)
    fips_deps(Gfx Input Time Dbg)
fips_end_app()

