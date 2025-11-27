#include <iostream>
#include "ring_buffer.hpp"

int main() {
    try{
        asher::SPSCQ<int> ringBuffer(10);
    }catch (const std::runtime_error& e){
        std::cout << e.what() << "\n";
    }
    try {
        asher::SPSCQ<int> ringBuffer(8);
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << "\n";
    }

	return 0;
}
