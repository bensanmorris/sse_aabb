#include <iostream>
#include <chrono>
#include <immintrin.h>

using namespace std;

int main()
{
    /*
    // original non-sse code:
    // Move the box to the origin and scale it.
    const Vector3 center   = (a.m_vMin + a.m_vMax) * Vector3(0.5f);
    const Vector3 localMin = a.m_vMin - center;
    const Vector3 localMax = a.m_vMax - center;

    // Compute the extents along each world axis.
    Vector3 aX = mtx[0]*localMin[0];
    Vector3 bX = mtx[0]*localMax[0];
    Vector3 aY = mtx[1]*localMin[1];
    Vector3 bY = mtx[1]*localMax[1];
    Vector3 aZ = mtx[2]*localMin[2];
    Vector3 bZ = mtx[2]*localMax[2];

    // Start at the world-space center of the box.
    b.m_vMin = b.m_vMax = t + mtx[0]*center.x + mtx[1]*center.y + mtx[2]*center.z;

    // Update the final AABB.
    b.m_vMin += (glm::min( aX, bX ) + glm::min( aY, bY ) + glm::min( aZ, bZ ));
    b.m_vMax += (glm::max( aX, bX ) + glm::max( aY, bY ) + glm::max( aZ, bZ ));
    */

    // a 16 byte aligned 4 float vector
    struct alignas(16) V4
    {
        float data[4];
    };

    V4 min { 1.f, 2.f, 3.f, 4.f };
    V4 max { 2.f, 3.f, 4.f, 5.f };
    V4 T   { 0.f, 0.f, 0.f, 0.f };

    // min + max
    __m128  min_max_sum_mm = _mm_add_ps((*(__m128*)&min), (*(__m128*)&max));

    // center i.e. (min + max) * 0.5
    V4 half{ 0.5f, 0.5f, 0.5f, 0.5f };
    __m128 center_mm = _mm_mul_ps(min_max_sum_mm, (*(__m128*)&half));

    // min - center
    __m128 local_min = _mm_sub_ps((*(__m128*)&min), center_mm);

    // max - center
    __m128 local_max = _mm_sub_ps((*(__m128*)&max), center_mm);

    // aX = mtx[0]*localMin[0];
    V4 mtx0 = { 1.f, 0.f, 0.f, 0.f };
    __m128 local_minx = _mm_set1_ps(((V4*)(&local_min))->data[0]);
    __m128 ax = _mm_mul_ps((*(__m128*)&mtx0), local_minx);

    // bX = mtx[0]*localMax[0];
    __m128 local_maxx = _mm_set1_ps(((V4*)(&local_max))->data[0]);
    __m128 bx = _mm_mul_ps((*(__m128*)&mtx0), local_max);

    // aY = mtx[1]*localMin[1];
    V4 mtx1 = { 0.f, 1.f, 0.f, 0.f };
    __m128 local_miny = _mm_set1_ps(((V4*)(&local_min))->data[1]);
    __m128 ay = _mm_mul_ps((*(__m128*)&mtx1), local_miny);

    // bY = mtx[1]*localMax[1];
    __m128 local_maxy = _mm_set1_ps(((V4*)(&local_max))->data[1]);
    __m128 by = _mm_mul_ps((*(__m128*)&mtx1), local_maxy);

    // aZ = mtx[2]*localMin[2];
    V4 mtx2 = { 0.f, 0.f, 1.f, 0.f };
    __m128 local_minz = _mm_set1_ps(((V4*)(&local_min))->data[2]);
    __m128 az = _mm_mul_ps((*(__m128*)&mtx2), local_minz);

    // bZ = mtx[2]*localMax[2];
    __m128 local_maxz = _mm_set1_ps(((V4*)(&local_max))->data[2]);
    __m128 bz = _mm_mul_ps((*(__m128*)&mtx2), local_maxz);

    // b.m_vMin = b.m_vMax = t + mtx[0]*center.x + mtx[1]*center.y + mtx[2]*center.z;
    __m128 centerX  = _mm_set1_ps(((V4*)(&center_mm))->data[0]);
    __m128 centerXX = _mm_mul_ps(centerX, (*(__m128*)&mtx0));
    __m128 centerY  = _mm_set1_ps(((V4*)(&center_mm))->data[1]);
    __m128 centerYY = _mm_mul_ps(centerY, (*(__m128*)&mtx1));
    __m128 centerZ  = _mm_set1_ps(((V4*)(&center_mm))->data[2]);
    __m128 centerZZ = _mm_mul_ps(centerZ, (*(__m128*)&mtx2));
    __m128 sumXYZ   = _mm_add_ps(_mm_add_ps(_mm_add_ps(centerXX, centerYY), centerZZ), (*(__m128*)&T));

    return 0;
}
