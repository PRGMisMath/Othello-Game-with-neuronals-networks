#include "NLBuffer.hpp"
#include<memory>
#include "NLDebug.hpp"

nl::Buffer::Buffer(size_t capacity) :
	m_capacity(capacity)
{
	m_buffer = (float*)malloc(m_capacity * sizeof(float));
	if (m_buffer == NULL)
		throw std::bad_alloc();
	memset(m_buffer, 0, m_capacity * sizeof(float));
}

nl::Buffer::~Buffer()
{
	free(m_buffer);
}

size_t nl::Buffer::get_capacity() const
{
	return m_capacity;
}

float& nl::Buffer::operator[](size_t index)
{

#ifdef NL_DEBUG
	if (index >= m_capacity)
		debug_break(IndexOutOfRange, "Index out of range !");
#endif

	return m_buffer[index];
}

float nl::Buffer::operator[](size_t index) const
{

#ifdef NL_DEBUG
	if (index >= m_capacity)
		debug_break(IndexOutOfRange, "Index out of range !");
#endif

	return m_buffer[index];
}

float* nl::Buffer::get_buffer()
{
	return m_buffer;
}

const float* nl::Buffer::get_buffer() const
{
	return m_buffer;
}

void nl::Buffer::set_to_zero()
{
	memset(m_buffer, 0, m_capacity * sizeof(float));
}

void nl::Buffer::set_to_zero(size_t reset_size)
{
#ifdef NL_DEBUG
	if (reset_size > m_capacity)
		debug_break(BadSizeArgument, "Too big reset size !");
#endif // NL_DEBUG

	memset(m_buffer, 0, reset_size * sizeof(float));
}
