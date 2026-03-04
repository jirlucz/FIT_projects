/**
 * @file    tree_mesh_builder.h
 *
 * @author  Jiri Kotek <xkotek09@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using OpenMP tasks + octree early elimination
 *
 * @date    1.12.2025
 **/

#ifndef TREE_MESH_BUILDER_H
#define TREE_MESH_BUILDER_H

#include "base_mesh_builder.h"

const unsigned CUT_OFF = 12;

class TreeMeshBuilder : public BaseMeshBuilder
{
public:
    TreeMeshBuilder(unsigned gridEdgeSize);

protected:
    unsigned marchCubes(const ParametricScalarField &field) override;
    float evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field) override;
    void emitTriangle(const Triangle_t &triangle) override;
    const Triangle_t *getTrianglesArray() const override { return mTriangles.data(); }

private:
    unsigned processOctree(const ParametricScalarField &field, size_t x, size_t y, size_t z, size_t size);

    std::vector<std::vector<Triangle_t>> mThreadTriangles;

    std::vector<Triangle_t> mTriangles;
};

#endif // TREE_MESH_BUILDER_H
