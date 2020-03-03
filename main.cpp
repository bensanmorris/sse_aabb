#include <iostream>
#include <chrono>

#define USE_SSE
#ifdef USE_SSE
    #include <immintrin.h>
#else
    #include "GLMRequirements.h"
    #include "Vector3.h"
    #include "Matrix3.h"
    using namespace firefly;
#endif

int main()
{
    using namespace std::chrono;
    auto t1 = high_resolution_clock::now();

#ifdef USE_SSE
    // a 16 byte aligned 4 float vector
    struct alignas(16) V4
    {
        float data[4];
    };
    __m128 AABBMin, AABBMax;
#else
    Vector3 newMin, newMax;
#endif

    double dummy = 0.0;
    for(int i = 0; i < 100000000; i++)
    {
#ifdef USE_SSE

    static const V4 min { 1.f, 2.f, 3.f, 4.f };
    static const V4 max { 2.f, 3.f, 4.f, 5.f };
    static const V4 T   { 0.f, 0.f, 0.f, 0.f }; // Tx, Ty, Tz
    static const V4 mtx0{ 1.f, 0.f, 0.f, 0.f }; // Rx
    static const V4 mtx1{ 0.f, 1.f, 0.f, 0.f }; // Ry
    static const V4 mtx2{ 0.f, 0.f, 1.f, 0.f }; // Rz

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
    __m128 local_minx = _mm_set1_ps(((V4*)(&local_min))->data[0]);
    __m128 ax = _mm_mul_ps((*(__m128*)&mtx0), local_minx);

    // bX = mtx[0]*localMax[0];
    __m128 local_maxx = _mm_set1_ps(((V4*)(&local_max))->data[0]);
    __m128 bx = _mm_mul_ps((*(__m128*)&mtx0), local_max);

    // aY = mtx[1]*localMin[1];
    __m128 local_miny = _mm_set1_ps(((V4*)(&local_min))->data[1]);
    __m128 ay = _mm_mul_ps((*(__m128*)&mtx1), local_miny);

    // bY = mtx[1]*localMax[1];
    __m128 local_maxy = _mm_set1_ps(((V4*)(&local_max))->data[1]);
    __m128 by = _mm_mul_ps((*(__m128*)&mtx1), local_maxy);

    // aZ = mtx[2]*localMin[2];
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
    __m128 newMin   = sumXYZ;
    __m128 newMax   = sumXYZ;

    // b.m_vMin += (glm::min( aX, bX ) + glm::min( aY, bY ) + glm::min( aZ, bZ ));
    AABBMin         = _mm_add_ps(newMin, _mm_add_ps(_mm_add_ps(_mm_min_ps(ax, bx), _mm_min_ps(ay, by)), _mm_min_ps(az, bz)));

    // b.m_vMax += (glm::max( aX, bX ) + glm::max( aY, bY ) + glm::max( aZ, bZ ));
    AABBMax         = _mm_add_ps(newMax, _mm_add_ps(_mm_add_ps(_mm_max_ps(ax, bx), _mm_max_ps(ay, by)), _mm_max_ps(az, bz)));

    dummy+=(((V4*)&AABBMin)->data[0]);

#else

    static const Vector3 min { 1.f, 2.f, 3.f };
    static const Vector3 max { 2.f, 3.f, 4.f };
    static const Matrix3 mtx { 1.f };
    static const Vector3 t   {};

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
    newMin = newMax = t + mtx[0]*center.x + mtx[1]*center.y + mtx[2]*center.z;

    // Update the final AABB.
    newMin += (glm::min( aX, bX ) + glm::min( aY, bY ) + glm::min( aZ, bZ ));
    newMax += (glm::max( aX, bX ) + glm::max( aY, bY ) + glm::max( aZ, bZ ));

    dummy+=newMin.x;

#endif
    }
    auto t2 = high_resolution_clock::now();
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1);
    std::cout << secs.count() << std::endl;

#ifdef USE_SSE
    V4* newMin = ((V4*)&AABBMin);
    V4* newMax = ((V4*)&AABBMax);
    std::cout << "min = " << newMin->data[0] << "," << newMin->data[1] << "," << newMin->data[2] << std::endl;
    std::cout << "max = " << newMax->data[0] << "," << newMax->data[1] << "," << newMax->data[2] << std::endl;
    std::cout << dummy << std::endl;
#else
    std::cout << "min = " << newMin.x << "," << newMin.y << "," << newMin.z << std::endl;
    std::cout << "max = " << newMax.x << "," << newMax.y << "," << newMax.z << std::endl;
    std::cout << dummy << std::endl;
#endif

    return 0;
}
