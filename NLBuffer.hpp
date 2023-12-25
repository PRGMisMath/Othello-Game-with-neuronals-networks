#ifndef NL_BUFFER_HPP
#define NL_BUFFER_HPP

namespace nl {

class Buffer {
public:

	Buffer(size_t capacity);

	~Buffer();

	size_t get_capacity() const;

	float& operator[](size_t index);

	float operator[](size_t index) const;

	float* get_buffer();

	const float* get_buffer() const;

	void set_to_zero();
	void set_to_zero(size_t reset_size);

private:

	size_t m_capacity;

	float* m_buffer;
};

}

#endif