//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <complex>

// complex& operator-=(const complex& rhs);

#include <complex>
#include <cassert>

template <class T>
void
test()
{
    std::complex<T> c;
    const std::complex<T> c2(1.5, 2.5);
    assert(c.real() == 0);
    assert(c.imag() == 0);
    c -= c2;
    assert(c.real() == -1.5);
    assert(c.imag() == -2.5);
    c -= c2;
    assert(c.real() == -3);
    assert(c.imag() == -5);

    std::complex<T> c3;

    c3 = c;
    std::complex<int> ic (1,1);
    c3 -= ic;
    assert(c3.real() == -4);
    assert(c3.imag() == -6);

    c3 = c;
    std::complex<float> dp (1,1);
    c3 -= dp;
    assert(c3.real() == -4);
    assert(c3.imag() == -6);
}

int main()
{
    test<float>();
    test<double>();
    test<long double>();
}
