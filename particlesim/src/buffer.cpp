#include <psim/buffer.h>

namespace psim {

	Buffer::Buffer(std::size_t size)
		: totalSize(size)
		, firstInactiveIdx(0)
		, bufferData(new particle[size])
	{
		for (std::size_t i = 0; i < totalSize; ++i) {
			bufferData[i].ttl = -1;
		}
	}

	Buffer::~Buffer()
	{
		delete[] bufferData;
	}

	void Buffer::sort()
	{
		if (totalSize >= 2) {
			std::size_t l = 0;
			std::size_t r = totalSize-1;

			bool done = false;
			while (l < r) {
				while ( (l < r) && (bufferData[l].ttl > 0) ) {
					++l;
				}

				while ( (r > l) && (bufferData[r].ttl <= 0) ) {
					--r;
				}

				if (l != r) {
					particle tmp = bufferData[r];
					bufferData[r] = bufferData[l];
					bufferData[l] = tmp;
					++l;
					firstInactiveIdx = r;
					--r;
				} 
			}
		}
	}

	bool Buffer::add(const particle & p)
	{
		bool ret;
		if (firstInactiveIdx >= totalSize) {
			ret = false;
		}
		else {
			bufferData[firstInactiveIdx] = p;
			++firstInactiveIdx;
			ret = true;
		}

		return ret;
	}
}