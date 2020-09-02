#pragma once


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
    template<typename T>
    class Buffer {
        private:
            size_t _maxSize;
            std::queue<T> _buffer;
        public:
        Buffer(const Buffer&) = delete;
        Buffer(Buffer&&) = delete;
        Buffer& operator=(const Buffer&) = delete;
        Buffer& operator=(Buffer&&) = delete;
        Buffer() : _maxSize{1} {}
        Buffer(size_t size) : _maxSize{size} {}
        ~Buffer() = default;

        inline bool isFull() const noexcept {
            return _buffer.size() == _maxSize;
        }

        inline bool isEmpty() const noexcept {
            return _buffer.empty();
        }

        inline bool insert(const T& val){
            if(!isFull()){
                _buffer.push(val);
                return true;
            }
            return false;
        }

        T get(){
            auto val = _buffer.front();
            _buffer.pop();
            return val;
        }
    };
}