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

    struct alignas(16) V4
    {
        float data[4];
    };

    // add
    V4 a { 1.f, 2.f, 3.f, 4.f };
    V4 b { 2.f, 3.f, 4.f, 5.f };
    __m128* pa = (__m128*)&a;
    __m128* pb = (__m128*)&b;
    __m128  cc = _mm_add_ps(*pa, *pb);
    V4&     pc = (V4&)cc;
    std::cout << pc.data[0] << "," << pc.data[1] << "," << pc.data[2] << "," << pc.data[3] << std::endl;

    //

    return 0;
}
