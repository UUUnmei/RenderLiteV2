#include "DepthBuffer.h"

DepthBuffer::DepthBuffer(uint32_t w, uint32_t h)
    : Buffer(w * h), width(w), height(h)
{
    std::cout << "[DEPTHBUFFER] buffer allocate length: " << length << '\n';
}

DepthBuffer::~DepthBuffer()
{
    std::cout << "[DEPTHBUFFER] buffer allocate length: " << length << '\n';
    // data交给父类析构
}

const int DepthBuffer::GetWidth() const noexcept
{
    return width;
}

const int DepthBuffer::GetHeight() const noexcept
{
    return height;
}

bool DepthBuffer::TryUpdate(uint32_t x, uint32_t y, float newZ)
{
    if (x >= width || y >= height) return false;
    if (newZ < data[y * width + x]) {
        data[y * width + x] = newZ;
        return true;
    }
    return false;
}

void DepthBuffer::Clear()
{
    std::fill(data, data + length, std::numeric_limits<float>::max());

}
