#include <iostream>
#include <chrono>
#include <immintrin.h>

using namespace std;

int main()
{
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
