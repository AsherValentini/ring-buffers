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
			explicit SPSCQ (std::size_t capacity) : head_(0), tail_(0), mask_(capacity -1) {
				if(!isPow2(capacity)) throw std::runtime_error("[SPSCQ] invalid capacity");
				capacity_ = capacity;
				std::allocator<T> alloc;
				data_ = alloc.allocate(capacity_);
				std::cout << "[SPSCQ] constructed\n";
			}
			~SPSCQ () {
				std::size_t head = head_.load(std::memory_order_relaxed);
				std::size_t tail = tail_.load(std::memory_order_relaxed);
				std::allocator<T> alloc;
				while(tail != head) {
					traits::destroy(alloc, &data_[tail & mask_]);
					tail++;
				}
				traits::deallocate(alloc, data_, capacity_);
				data_ = nullptr;
				std::cout << "[SPSCQ] destructed\n";
			}
			// House keeping
			SPSCQ (const SPSCQ&) = delete;
			SPSCQ& operator=(SPSCQ&) = delete;
			SPSCQ (SPSCQ&&) = delete;
			SPSCQ& operator=(SPSCQ&&) = delete;

			bool push(const T& item) {
				std::size_t head = head_.load(std::memory_order_relaxed);
				std::size_t tail = tail_.load(std::memory_order_acquire);

				if( (head - tail) == (capacity_ -1) ) return false;

				std::allocator<T> alloc;
				traits::construct(alloc, &data_[head & mask_], item);

				head_.store(head + 1, std::memory_order_release);

				return true;
			}

			bool pop (T& item) {
				std::size_t tail = tail_.load(std::memory_order_relaxed);
				std::size_t head = head_.load(std::memory_order_acquire);

				if( (head == tail) ) return false;

				std::allocator<T> alloc;
				item = std::move(data_[tail & mask_]);
				traits::destroy(alloc, &data_[tail & mask_]);

				tail_.store(tail + 1, std::memory_order_release);

				return true;
			}
			[[nodiscard]] bool full () {
				std::size_t head = head_.load(std::memory_order_relaxed);
				std::size_t tail = tail_.load(std::memory_order_relaxed);
				if ( (head - tail ) == (capacity_ - 1) ) return true;
				return false;
			}
			[[nodiscard]] bool empty () {
				std::size_t head = head_.load(std::memory_order_relaxed);
				std::size_t tail = tail_.load(std::memory_order_relaxed);
				if ( head == tail ) return true;
				return false;
			}
			[[nodiscard]] std::size_t size() {
				std::size_t head = head_.load(std::memory_order_relaxed);
				std::size_t tail = tail_.load(std::memory_order_relaxed);
				return (head - tail);
			}

		private:
			// we will pad the atomics to avoid false sharing and then later force false sharing
			alignas(64) std::atomic<std::size_t> head_{0};
			//std::atomic<std::size_t> head_{0};
			char pad1[64];
			alignas(64) std::atomic<std::size_t> tail_{0};
			//std::atomic<std::size_t> tail_{0};
			char pad2[64];
			T* data_ = nullptr;
			std::size_t capacity_;
			std::size_t mask_;
			bool isPow2 (const std::size_t& capacity) {
				if(capacity > 1 && ( (capacity & (capacity -1)) == 0) ) return true;
				return false;
			}

	};
} 