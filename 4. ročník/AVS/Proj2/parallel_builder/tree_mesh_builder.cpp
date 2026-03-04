/**
 * @file    tree_mesh_builder.cpp
 *
 * @author  Jiri Kotek <xkotek09@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using OpenMP tasks + octree early elimination
 *
 * @date    1.12.2025
 **/

#include <iostream>
#include <math.h>
#include <limits>
#include <omp.h>

#include "tree_mesh_builder.h"

TreeMeshBuilder::TreeMeshBuilder(unsigned gridEdgeSize)
    : BaseMeshBuilder(gridEdgeSize, "Octree")
{

}

unsigned TreeMeshBuilder::marchCubes(const ParametricScalarField &field)
{
    int maxThreads = omp_get_max_threads();
    mThreadTriangles.resize(maxThreads);

    unsigned totalTriangles = 0;

    #pragma omp parallel default(none) shared(totalTriangles, field)
    {
        #pragma omp single
        {
            totalTriangles = processOctree(field, 0, 0, 0, mGridSize);
        }
    }

    // 3. Sloučení výsledků z vláken (Merge)
    // Nejdříve realokujeme paměť pro finální vektor, abychom se vyhnuli kopírování
    mTriangles.reserve(totalTriangles);
    
    for(const auto& threadVec : mThreadTriangles) {
        mTriangles.insert(mTriangles.end(), threadVec.begin(), threadVec.end());
    }

    return totalTriangles;
}

unsigned TreeMeshBuilder::processOctree(const ParametricScalarField &field, size_t x, size_t y, size_t z, size_t size)
{
    float midOffset = size / 2.0f;
    Vec3_t<float> centerGrid(x + midOffset, y + midOffset, z + midOffset);
    
    // Převedeme střed do "fyzických" souřadnic (pro vyhodnocení pole)
    Vec3_t<float> centerPhys(centerGrid.x * mGridResolution, 
                             centerGrid.y * mGridResolution, 
                             centerGrid.z * mGridResolution);

    float F_p = evaluateFieldAt(centerPhys, field);
    
    // Vzorec (5.3): F(p) > isoLevel + (sqrt(3)/2) * a 
    float a = size * mGridResolution;
    float threshold = mIsoLevel + a * (sqrt(3.0f) / 2.0f);

    // Vyhodnocení prázdnosti bloku
    if (F_p > threshold) {
        return 0;
    }

    // Vyhodnocení velikosti hloubky neprázdného potomka 
    if (size <= CUT_OFF) {
        unsigned count = 0;
        // Projdeme všechny elementární krychle v tomto bloku
        for (size_t i = 0; i < size; ++i) {
            for (size_t j = 0; j < size; ++j) {
                for (size_t k = 0; k < size; ++k) {
                    // buildCube zpracovává vždy jen 1 krychli na dané souřadnici
                    count += buildCube(Vec3_t<float>(x + i, y + j, z + k), field);
                }
            }
        }
        return count;
    }

    // Rekurzivní dělení Octree
    unsigned totalSubTriangles = 0;
    size_t half = size / 2;

    for (size_t dx = 0; dx < size; dx += half) {
        for (size_t dy = 0; dy < size; dy += half) {
            for (size_t dz = 0; dz < size; dz += half) {
                
                #pragma omp task default(none) firstprivate(x, y, z, dx, dy, dz, half) shared(totalSubTriangles, field)
                {
                    unsigned count = processOctree(field, x + dx, y + dy, z + dz, half);
                
                    if (count > 0) {
                        #pragma omp atomic
                        totalSubTriangles += count;
                    }
                }
            }
        }
    }

    // Vyčkání na do dokončení potomků Octree
    #pragma omp taskwait
    return totalSubTriangles;
}

float TreeMeshBuilder::evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field)
{
    const Vec3_t<float> *pPoints = field.getPoints().data();
    const unsigned count = unsigned(field.getPoints().size());

    float value = std::numeric_limits<float>::max();

    for(unsigned i = 0; i < count; ++i)
    {
        float distanceSquared  = (pos.x - pPoints[i].x) * (pos.x - pPoints[i].x);
        distanceSquared       += (pos.y - pPoints[i].y) * (pos.y - pPoints[i].y);
        distanceSquared       += (pos.z - pPoints[i].z) * (pos.z - pPoints[i].z);

        value = std::min(value, distanceSquared);
    }

    return sqrt(value);
}

void TreeMeshBuilder::emitTriangle(const BaseMeshBuilder::Triangle_t &triangle)
{
    int threadId = omp_get_thread_num();
    mThreadTriangles[threadId].push_back(triangle);
}
