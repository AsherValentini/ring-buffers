#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <atomic>

namespace asher {
	template <typename T>
	class SPSCQ {
		using traits = std::allocator_traits<std::allocator<T>>;
		public:
			explicit SPSCQ (std::size_t capacity) : head_(0), tail_(0) {
				if(!isPow2(capacity)) throw std::runtime_error("[SPSCQ] invalid capacity");
				std::cout << "[SPSCQ] constructed\n";
			}
			~SPSCQ () {
				std::cout << "[SPSCQ] destructed\n";
			}
			// House keeping
			SPSCQ (const SPSCQ&) = delete;
			SPSCQ& operator=(SPSCQ&) = delete;
			SPSCQ (SPSCQ&&) = delete;
			SPSCQ operator=(SPSCQ&&) = delete;

			bool push() {
				return false;
			}

			bool pop () {
				return false;
			}



		private:
			// we will pad the atomics to avoid false sharing and then later force false sharing
			alignas(64) std::atomic<std::size_t> head_{0};
			char pad1[64];
			alignas(64) std::atomic<std::size_t> tail_{0};
			char pad2[64];
			T* data_ = nullptr;

			bool isPow2 (const std::size_t& capacity) {
				if(capacity > 1 && ( (capacity & (capacity -1)) == 0) ) return true;
				return false;
			}

	};
} 