#include "Buffer.h"
#include <cstring>
#include <iostream>

template<typename T>
inline Buffer<T>::Buffer()
	:data(nullptr), length(0)
{
	std::cout << "empty Buffer construct\n";
}

template<typename T>
inline Buffer<T>::Buffer(uint32_t length)
	:data(new T[length]), length(length)
{ 
	std::cout << "Buffer construct, length: " << length << '\n';
}


template<typename T>
inline Buffer<T>::Buffer(const Buffer<T>& rhs)
{
	data = new T[rhs.length];
	std::memcpy(data, rhs.data, rhs.length * sizeof(T));
	length = rhs.length;
	std::cout << "Buffer copy construct, length: " << length << '\n';
}

template<typename T>
inline Buffer<T>& Buffer<T>::operator=(const Buffer<T>& rhs)
{
	if (data == rhs.data) return *this;
	T* temp = data;
	data = new T[rhs.length];
	std::memcpy(data, rhs.data, rhs.length * sizeof(T));
	delete[] temp;
	length = rhs.length;
	std::cout << "Buffer assign, length: " << length << '\n';
	return *this;
}

template<typename T>
inline Buffer<T>::~Buffer()
{
	delete[] data;
	data = nullptr;
	std::cout << "Buffer deconstruct\n";
}


template<typename T>
inline const uint32_t Buffer<T>::GetLength() const noexcept
{
	return length;
}

template<typename T>
inline const T* Buffer<T>::Get() const noexcept
{
	return data;
}

template<typename T>
inline T* Buffer<T>::Get() noexcept
{
	return data;
}
