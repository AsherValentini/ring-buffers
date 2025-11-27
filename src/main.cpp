#include <iostream>
#include "ring_buffer.hpp"

int main() {
    asher::SPSCQ<int> ringBuffer(10);
	return 0;
}
