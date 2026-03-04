/**
 * @file    loop_mesh_builder.h
 *
 * @author  Jiri Kotek <xkotek09@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using OpenMP loops
 *
 * @date    1.12.2025
 **/

#ifndef LOOP_MESH_BUILDER_H
#define LOOP_MESH_BUILDER_H

#include <vector>
#include "base_mesh_builder.h"

class LoopMeshBuilder : public BaseMeshBuilder
{
public:
    LoopMeshBuilder(unsigned gridEdgeSize);

protected:
    unsigned marchCubes(const ParametricScalarField &field) override;
    float evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field) override;
    void emitTriangle(const Triangle_t &triangle) override;
    const Triangle_t *getTrianglesArray() const override { return mTriangles.data(); }

private:
    std::vector<std::vector<Triangle_t>> mThreadTriangles;
    
    std::vector<Triangle_t> mTriangles;
};


#endif // LOOP_MESH_BUILDER_H
