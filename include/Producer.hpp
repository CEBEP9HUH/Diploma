#pragma once


#include <iostream>
#include <type_traits>


#include "Buffer.hpp"


namespace Diploma{
    template<typename T, typename function_t, typename...Args>
    class Producer {
        private:
            static_assert(std::is_invocable<function_t, Args...>::value);
            Buffer<T>& _buffer;
            std::queue<T> _localBuffer;
            synchronization& _sync;
            function_t& _producer;
            std::tuple<Args...> _args;
            T produceData(){
                return std::apply(_producer, _args);
            }
        public:
            Producer() = delete;
            Producer(const Producer&) = delete;
            Producer(Producer&&) = delete;
            Producer& operator=(const Producer&) = delete;
            Producer& operator=(Producer&&) = delete;
            ~Producer() = default;

            Producer(Buffer<T>& buffer, synchronization& sync, function_t& funct, Args...args) :    _buffer{buffer}, 
                                                                                                    _sync{sync},
                                                                                                    _producer{funct},
                                                                                                    _args{args...} {}

            void run(){
                while(!_sync._exitThread){
                    std::unique_lock<std::mutex> lockGuard(_sync._buffer_mutex);
                    _sync._conditionVar.wait(lockGuard, [this](){return !_buffer.isFull();});
                    T data = produceData();
                    _buffer.insert(data);
                    lockGuard.unlock();
                    _sync._conditionVar.notify_one();
                    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
                }
            }
    };
}