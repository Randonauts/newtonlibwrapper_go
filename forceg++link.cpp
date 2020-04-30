/*
 * Avoid the following errors on go build/go install by having a dummy CPP file which forces linking with g++.
 * Ref: https://github.com/golang/go/issues/18460#issuecomment-270304872
 *
# newtonlibwrapper_go
./libAttract.a(export.o): In function `getHandle':
export.cpp:(.text+0x8e): undefined reference to `std::__throw_system_error(int)'
./libAttract.a(export.o): In function `getDotsBySpotRadius':
export.cpp:(.text+0x101): undefined reference to `round'
./libAttract.a(export.o): In function `Attractors::getPseudorandomCircularCoords(unsigned long, unsigned int)':
export.cpp:(.text+0x1bd): undefined reference to `operator delete[](void*)'
export.cpp:(.text+0x1e3): undefined reference to `std::__throw_system_error(int)'
export.cpp:(.text+0x1f3): undefined reference to `operator delete(void*)'
./libAttract.a(export.o): In function `getEngineCoordsLength':
export.cpp:(.text+0x30f): undefined reference to `std::__throw_system_error(int)'
./libAttract.a(export.o): In function `Attractors::randomBytesToCircular(unsigned char const*, unsigned long)':
export.cpp:(.text+0x35c): undefined reference to `operator delete(void*)'
./libAttract.a(export.o): In function `Attractors::randomHexToCircular(char const*, unsigned long)':
export.cpp:(.text+0x3ac): undefined reference to `operator delete(void*)'
./libAttract.a(export.o): In function `Attractors::findAttractorsTest(unsigned long, double, double)':
export.cpp:(.text+0x485): undefined reference to `operator delete(void*)'
export.cpp:(.text+0x4e2): undefined reference to `operator new(unsigned long)'
export.cpp:(.text+0x52d): undefined reference to `operator new(unsigned long)'
export.cpp:(.text+0x554): undefined reference to `operator delete(void*)'
export.cpp:(.text+0x60d): undefined reference to `std::__throw_bad_alloc()'
export.cpp:(.text+0x63b): undefined reference to `operator delete(void*)'
export.cpp:(.text+0x648): undefined reference to `operator delete(void*)'
export.cpp:(.text+0x651): undefined reference to `std::__throw_system_error(int)'
./libAttract.a(export.o): In function `findAttractors':
export.cpp:(.text+0x725): undefined reference to `operator delete(void*)'
export.cpp:(.text+0x783): undefined reference to `operator new(unsigned long)'
export.cpp:(.text+0x7ce): undefined reference to `operator new(unsigned long)'
export.cpp:(.text+0x7f6): undefined reference to `operator delete(void*)'
export.cpp:(.text+0x83b): undefined reference to `std::__throw_system_error(int)'
export.cpp:(.text+0x840): undefined reference to `std::__throw_bad_alloc()'
export.cpp:(.text+0x86e): undefined reference to `operator delete(void*)'
export.cpp:(.text+0x87b): undefined reference to `operator delete(void*)'
./libAttract.a(export.o): In function `getAttractorsLength':
export.cpp:(.text+0x932): undefined reference to `std::__throw_system_error(int)'
./libAttract.a(export.o): In function `getFinalCoordsLength':
export.cpp:(.text+0xa4f): undefined reference to `std::__throw_system_error(int)'
./libAttract.a(export.o): In function `Attractors::getAttractors(unsigned long, double, LatLng, unsigned long long)':
export.cpp:(.text+0xb78): undefined reference to `std::__throw_system_error(int)'
./libAttract.a(export.o): In function `getAttractors':
export.cpp:(.text+0xc89): undefined reference to `operator delete(void*)'
export.cpp:(.text+0xcb6): undefined reference to `operator delete[](void*)'
export.cpp:(.text+0xce3): undefined reference to `operator new[](unsigned long)'
export.cpp:(.text+0xd48): undefined reference to `std::__throw_system_error(int)'
export.cpp:(.text+0xd59): undefined reference to `operator delete(void*)'
export.cpp:(.text+0xd66): undefined reference to `__cxa_throw_bad_array_new_length'
./libAttract.a(export.o): In function `Attractors::getAttractorsExt(unsigned long, double, LatLng, unsigned long long)':
export.cpp:(.text+0xe78): undefined reference to `std::__throw_system_error(int)'
./libAttract.a(export.o): In function `printCoords':
export.cpp:(.text+0xf7c): undefined reference to `operator new(unsigned long)'
export.cpp:(.text+0xfb7): undefined reference to `operator delete(void*)'
export.cpp:(.text+0xfd5): undefined reference to `std::__throw_bad_alloc()'
export.cpp:(.text+0xfe6): undefined reference to `operator delete(void*)'
./libAttract.a(export.o): In function `printAttractors':
export.cpp:(.text+0x1260): undefined reference to `operator new(unsigned long)'
 *
 * */
