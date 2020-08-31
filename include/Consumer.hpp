#pragma once


#include <type_traits>


#include "Buffer.hpp"


#include <iostream>


namespace Diploma{
    template<typename function_t, typename...Args>
    class Consumer {
        public:
            static_assert(std::is_invocable<function_t, Args...>::value);
            using return_t = typename std::invoke_result<function_t, Args...>::type;
            using signature_t = function_t;
        private:
            Buffer<return_t>& _buffer;
            synchronization& _sync;
            function_t& _producer;
            std::tuple<Args...> _args;
        public:
            Consumer() = delete;
            Consumer(const Consumer&) = delete;
            Consumer(Consumer&&) = delete;
            Consumer& operator=(const Consumer&) = delete;
            Consumer& operator=(Consumer&&) = delete;
            ~Consumer() = default;

            Consumer(Buffer<return_t>& buffer, synchronization& sync, function_t& funct, Args...args) : _buffer{buffer}, 
                                                                                                        _sync{sync},
                                                                                                        _producer{funct},
                                                                                                        _args{args...} {}

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