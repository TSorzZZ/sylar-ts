
#ifndef __SYLAR_ENDIANN_H__
#define __SYLAR_ENDIANN_H__

#include <type_traits>
#define SYLAR_LITTLE_ENDIAN 1
#define SYLAR_BIG_ENDIAN 2

#include <byteswap.h>
#include <stdint.h>

namespace sylar {

//8字节类型的字节序转化
template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type
byteswap(T value) {
    return (T)bswap_64((uint64_t)value);
}

//4字节类型的字节序转化
template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type
byteswap(T value) {
    return (T)bswap_32((uint32_t)value);
}

//2字节类型的字节序转化
template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type
byteswap(T value) {
    return (T)bswap_16((uint16_t)value);
}

#if BYTE_ORDER == __LITTLE_ENDIAN
#define SYLAR_BYTE_ORDER SYLAR_LITTLE_ENDIAN
#else
#define SYLAR_BYTE_ORDER SYLAR_BIG_ENDIAN
#endif

#if SYLAR_BYTE_ORDER == SYLAR_BIG_ENDIAN

//只在小端机器上执行byteswap, 在大端机器上什么都不做
template<class T>
T byteswapOnLittleEndian(T t) {
    return t;
}

//只在大端机器上执行byteswap, 在小端机器上什么都不做
template<class T>
T byteswapOnBigEndian(T t) {
    return byteswap(t);
}
#else

//只在小端机器上执行byteswap, 在大端机器上什么都不做
template<class T>
T byteswapOnLittleEndian(T t) {
    return byteswap(t);
}

//只在大端机器上执行byteswap, 在小端机器上什么都不做
template<class T>
T byteswapOnBigEndian(T t) {
    return t;
}
#endif

}

#endif