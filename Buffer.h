#pragma once
#include <stdint.h>

template<typename T>
class Buffer
{

protected:
	T* data;
	uint32_t length;

public:
	Buffer();
	Buffer(uint32_t length);
	Buffer(const Buffer<T>&);  // ���
	Buffer<T>& operator=(const Buffer<T>&);  // ���
	virtual ~Buffer();

	const uint32_t GetLength() const noexcept;
	const T* Get() const noexcept;
	T* Get() noexcept;
};

#include "Buffer.inl"
