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

    // min
    V4 min { 1.f, 2.f, 3.f, 4.f };
    __m128* min_mm = (__m128*)&min;

    // max
    V4 max { 2.f, 3.f, 4.f, 5.f };
    __m128* max_mm = (__m128*)&max;

    // min + max
    __m128  min_max_sum_mm = _mm_add_ps(*min_mm, *max_mm);

    // center i.e. (min + max) * 0.5
    V4 half{ 0.5f, 0.5f, 0.5f, 0.5f };
    __m128* half_mm = (__m128*)&half;
    __m128  center_mm = _mm_mul_ps(min_max_sum_mm, *half_mm);
    V4&     center = (V4&)center_mm;

    // min - center
    __m128 local_min = _mm_sub_ps(*min_mm, center_mm);

    // max - center
    __m128 local_max = _mm_sub_ps(*max_mm, center_mm);

    // todo
    // ...

    return 0;
}
