#pragma once
/* 
    Buffer<buffer_t> is a shared memory place which can get values 
    from producer and provides them to consumer.
    buffer_t - is a type of storing data
*/

#include <queue>
#include <mutex>
#include <condition_variable>

#include <iostream>
namespace Diploma{
    struct synchronization{
        std::mutex _buffer_mutex;
        std::condition_variable _conditionVar;
        bool _exitThread = false;
    };
    template<typename buffer_t>
    class Buffer {
        private:
            size_t _maxSize;
            std::queue<buffer_t> _buffer;
        public:
        Buffer(const Buffer&) = delete;
        Buffer(Buffer&&) = delete;
        Buffer& operator=(const Buffer&) = delete;
        Buffer& operator=(Buffer&&) = delete;
        Buffer() : _maxSize{1} {}
        Buffer(size_t size) : _maxSize{size} {}
        ~Buffer() = default;

        // checks if buffer is full
        inline bool isFull() const noexcept {
            return _buffer.size() == _maxSize;
        }

        // checks if buffer is empty
        inline bool isEmpty() const noexcept {
            return _buffer.empty();
        }

        // insert new element into buffer if it possible
        // return true if insertion was succeed
        inline bool insert(const buffer_t& val){
            if(!isFull()){
                _buffer.push(val);
                return true;
            }
            return false;
        }

        // get next value of buffer and remove it from buffer
        // doesn't check if buffer is empty
        buffer_t get(){
            auto val = _buffer.front();
            _buffer.pop();
            return val;
        }
    };
}