/**
 * @file    loop_mesh_builder.cpp
 *
 * @author  Jiri Kotek <xkotek09@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using OpenMP loops
 *
 * @date    1.12.2025
 **/

#include <iostream>
#include <math.h>
#include <limits>
#include <omp.h>

#include "loop_mesh_builder.h"

LoopMeshBuilder::LoopMeshBuilder(unsigned gridEdgeSize)
    : BaseMeshBuilder(gridEdgeSize, "OpenMP Loop")
{

}

unsigned LoopMeshBuilder::marchCubes(const ParametricScalarField &field)
{
    // 1. Compute total number of cubes in the grid.
    size_t totalCubesCount = mGridSize*mGridSize*mGridSize;

    unsigned totalTriangles = 0;

    int maxThreads = omp_get_max_threads();
    mThreadTriangles.resize(maxThreads);

    // // Vyčistíme případná stará data
    // for(auto& vec : mThreadTriangles) {
    //     vec.clear();
    // }
    // mTriangles.clear();

    // 2. Loop over each coordinate in the 3D grid.
    // #pragma omp parallel for schedule(static) reduction(+:totalTriangles)
    // #pragma omp parallel for schedule(dynamic) reduction(+:totalTriangles)
    #pragma omp parallel for schedule(guided, 16) reduction(+:totalTriangles)
    for(size_t i = 0; i < totalCubesCount; ++i)
    {
        // 3. Compute 3D position in the grid.
        Vec3_t<float> cubeOffset( i % mGridSize,
                                 (i / mGridSize) % mGridSize,
                                  i / (mGridSize*mGridSize));

        // 4. Evaluate "Marching Cube" at given position in the grid and
        //    store the number of triangles generated.
        totalTriangles += buildCube(cubeOffset, field);
    }

    mTriangles.reserve(totalTriangles);
    
    for(const auto& threadVec : mThreadTriangles) {
        mTriangles.insert(mTriangles.end(), threadVec.begin(), threadVec.end());
    }
    // 5. Return total number of triangles generated.
    return totalTriangles;
}


float LoopMeshBuilder::evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field)
{
    // NOTE: This method is called from "buildCube(...)"!

    // 1. Store pointer to and number of 3D points in the field
    //    (to avoid "data()" and "size()" call in the loop).
    const Vec3_t<float> *pPoints = field.getPoints().data();
    const unsigned count = unsigned(field.getPoints().size());

    float value = std::numeric_limits<float>::max();

    // 2. Find minimum square distance from points "pos" to any point in the
    //    field.
    // #pragma omp parallel for reduction(min:value)
    for(unsigned i = 0; i < count; ++i)
    {
        float distanceSquared  = (pos.x - pPoints[i].x) * (pos.x - pPoints[i].x);
        distanceSquared       += (pos.y - pPoints[i].y) * (pos.y - pPoints[i].y);
        distanceSquared       += (pos.z - pPoints[i].z) * (pos.z - pPoints[i].z);

        // Comparing squares instead of real distance to avoid unnecessary
        // "sqrt"s in the loop.
        value = std::min(value, distanceSquared);
    }

    // 3. Finally take square root of the minimal square distance to get the real distance
    return sqrt(value);
}

void LoopMeshBuilder::emitTriangle(const BaseMeshBuilder::Triangle_t &triangle)
{
    // NOTE: This method is called from "buildCube(...)"!

    // Store generated triangle into vector (array) of generated triangles.
    // The pointer to data in this array is return by "getTrianglesArray(...)" call
    // after "marchCubes(...)" call ends.
    int threadId = omp_get_thread_num();
    mThreadTriangles[threadId].push_back(triangle);
    
}
