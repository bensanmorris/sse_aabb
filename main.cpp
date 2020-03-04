#define PICOBENCH_IMPLEMENT_WITH_MAIN
#include "picobench/picobench.hpp"

#include <iostream>
#include <chrono>
#include <immintrin.h>
#include "GLMRequirements.h"
typedef glm::vec3 Vector3;
typedef glm::mat3 Matrix3;

struct alignas(16) Vector4SIMD
{
    float data[4];
};
void UpdateAABB_SIMD(Vector4SIMD min, Vector4SIMD max, Vector4SIMD T, Vector4SIMD mtx0, Vector4SIMD mtx1, Vector4SIMD mtx2, Vector4SIMD& newMin, Vector4SIMD& newMax)
{
    // min + max
    __m128  min_max_sum_mm = _mm_add_ps((*(__m128*)&min), (*(__m128*)&max));

    // center i.e. (min + max) * 0.5
    Vector4SIMD half{ 0.5f, 0.5f, 0.5f, 0.5f };
    __m128 center_mm = _mm_mul_ps(min_max_sum_mm, (*(__m128*)&half));

    // min - center
    __m128 local_min = _mm_sub_ps((*(__m128*)&min), center_mm);

    // max - center
    __m128 local_max = _mm_sub_ps((*(__m128*)&max), center_mm);

    // aX = mtx[0]*localMin[0];
    __m128 local_minx = _mm_set1_ps(((Vector4SIMD*)(&local_min))->data[0]);
    __m128 ax = _mm_mul_ps((*(__m128*)&mtx0), local_minx);

    // bX = mtx[0]*localMax[0];
    __m128 local_maxx = _mm_set1_ps(((Vector4SIMD*)(&local_max))->data[0]);
    __m128 bx = _mm_mul_ps((*(__m128*)&mtx0), local_max);

    // aY = mtx[1]*localMin[1];
    __m128 local_miny = _mm_set1_ps(((Vector4SIMD*)(&local_min))->data[1]);
    __m128 ay = _mm_mul_ps((*(__m128*)&mtx1), local_miny);

    // bY = mtx[1]*localMax[1];
    __m128 local_maxy = _mm_set1_ps(((Vector4SIMD*)(&local_max))->data[1]);
    __m128 by = _mm_mul_ps((*(__m128*)&mtx1), local_maxy);

    // aZ = mtx[2]*localMin[2];
    __m128 local_minz = _mm_set1_ps(((Vector4SIMD*)(&local_min))->data[2]);
    __m128 az = _mm_mul_ps((*(__m128*)&mtx2), local_minz);

    // bZ = mtx[2]*localMax[2];
    __m128 local_maxz = _mm_set1_ps(((Vector4SIMD*)(&local_max))->data[2]);
    __m128 bz = _mm_mul_ps((*(__m128*)&mtx2), local_maxz);

    // b.m_vMin = b.m_vMax = t + mtx[0]*center.x + mtx[1]*center.y + mtx[2]*center.z;
    __m128 centerX  = _mm_set1_ps(((Vector4SIMD*)(&center_mm))->data[0]);
    __m128 centerXX = _mm_mul_ps(centerX, (*(__m128*)&mtx0));
    __m128 centerY  = _mm_set1_ps(((Vector4SIMD*)(&center_mm))->data[1]);
    __m128 centerYY = _mm_mul_ps(centerY, (*(__m128*)&mtx1));
    __m128 centerZ  = _mm_set1_ps(((Vector4SIMD*)(&center_mm))->data[2]);
    __m128 centerZZ = _mm_mul_ps(centerZ, (*(__m128*)&mtx2));
    __m128 sumXYZ   = _mm_add_ps(_mm_add_ps(_mm_add_ps(centerXX, centerYY), centerZZ), (*(__m128*)&T));
    newMin          = (*(Vector4SIMD*)&sumXYZ);
    newMax          = (*(Vector4SIMD*)&sumXYZ);

    // b.m_vMin += (glm::min( aX, bX ) + glm::min( aY, bY ) + glm::min( aZ, bZ ));
    (*(__m128*)&newMin) = _mm_add_ps((*(__m128*)&newMin), _mm_add_ps(_mm_add_ps(_mm_min_ps(ax, bx), _mm_min_ps(ay, by)), _mm_min_ps(az, bz)));

    // b.m_vMax += (glm::max( aX, bX ) + glm::max( aY, bY ) + glm::max( aZ, bZ ));
    (*(__m128*)&newMax) = _mm_add_ps((*(__m128*)&newMax), _mm_add_ps(_mm_add_ps(_mm_max_ps(ax, bx), _mm_max_ps(ay, by)), _mm_max_ps(az, bz)));
}

void UpdateAABB(Vector3 min, Vector3 max, Vector3 T, Matrix3 mtx, Vector3& newMin, Vector3& newMax)
{
    // original non-sse code:
    // Move the box to the origin and scale it.
    const Vector3 center   = (min + max) * Vector3(0.5f);
    const Vector3 localMin = min - center;
    const Vector3 localMax = max - center;

    // Compute the extents along each world axis.
    Vector3 aX = mtx[0]*localMin[0];
    Vector3 bX = mtx[0]*localMax[0];
    Vector3 aY = mtx[1]*localMin[1];
    Vector3 bY = mtx[1]*localMax[1];
    Vector3 aZ = mtx[2]*localMin[2];
    Vector3 bZ = mtx[2]*localMax[2];

    // Start at the world-space center of the box.
    newMin = newMax = T + mtx[0]*center.x + mtx[1]*center.y + mtx[2]*center.z;

    // Update the final AABB.
    newMin += (glm::min( aX, bX ) + glm::min( aY, bY ) + glm::min( aZ, bZ ));
    newMax += (glm::max( aX, bX ) + glm::max( aY, bY ) + glm::max( aZ, bZ ));
}

static const int ITERATION_FACTOR = 10000;

static void benchmark_normal(picobench::state& s)
{
    static const Vector3 min { 1.f, 2.f, 3.f };
    static const Vector3 max { 2.f, 3.f, 4.f };
    static const Matrix3 mtx { 1.f };
    static const Vector3 t   {};

    Vector3 newMin, newMax;
    for(auto _ : s)
    {
        UpdateAABB(min, max, t, mtx, newMin, newMax);
    }
}
PICOBENCH(benchmark_normal);

static void benchmark_simd(picobench::state& s)
{
    static Vector4SIMD min { 1.f, 2.f, 3.f, 4.f };
    static Vector4SIMD max { 2.f, 3.f, 4.f, 5.f };
    static Vector4SIMD T   { 0.f, 0.f, 0.f, 0.f }; // Tx, Ty, Tz
    static Vector4SIMD mtx0{ 1.f, 0.f, 0.f, 0.f }; // Rx
    static Vector4SIMD mtx1{ 0.f, 1.f, 0.f, 0.f }; // Ry
    static Vector4SIMD mtx2{ 0.f, 0.f, 1.f, 0.f }; // Rz

    Vector4SIMD newMin, newMax;
    for(auto _ : s)
    {
        UpdateAABB_SIMD(min, max, T, mtx0, mtx1, mtx2, newMin, newMax);
    }
}
PICOBENCH(benchmark_simd);
