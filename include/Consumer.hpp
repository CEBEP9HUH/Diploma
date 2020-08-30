#pragma once


#include "Buffer.hpp"
#include <iostream>


namespace Diploma{
    template<typename T>
    class Consumer {
        private:
            Buffer<T>& _buffer;
            synchronization& _sync;
        public:
            Consumer() = delete;
            Consumer(const Consumer&) = delete;
            Consumer(Consumer&&) = delete;
            Consumer& operator=(const Consumer&) = delete;
            Consumer& operator=(Consumer&&) = delete;
            ~Consumer() = default;

            Consumer(Buffer<T>& buffer, synchronization& sync) : _buffer{buffer}, _sync{sync} {}

            void run(){
                while(!_sync._exitThread){
                    std::unique_lock<std::mutex> lockGuard(_sync._buffer_mutex);
                    auto bufferNotEmpty = _sync._conditionVar.wait_for(lockGuard,
                                                                        std::chrono::milliseconds(10),
                                                                        [this](){return !_buffer.isEmpty();});
                    if(bufferNotEmpty){
                        std::cout << "get: " << _buffer.get() <<'\n';
                    }
                    lockGuard.unlock();
                    _sync._conditionVar.notify_one();
                }
            }
    };
}