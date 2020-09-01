#pragma once


#include <type_traits>


#include "Buffer.hpp"


#include <iostream>


namespace Diploma{
    template<typename function_t, typename T, typename...Args>
    class Consumer;

    template<typename function_t, typename T, typename...Args>
    class Consumer <function_t, T, std::tuple<Args...> > {
        public:
            static_assert(std::is_invocable<function_t, T, Args...>::value);
            using buffer_t = T;
            using signature_t = function_t;
            using args_tuple_t = std::tuple<Args...>;
        private:
            Buffer<buffer_t>& _buffer;
            synchronization& _sync;
            function_t _consumer;
            std::tuple<Args...> _args;
        public:
            Consumer() = delete;
            Consumer(const Consumer&) = delete;
            Consumer(Consumer&&) = delete;
            Consumer& operator=(const Consumer&) = delete;
            Consumer& operator=(Consumer&&) = delete;
            ~Consumer() = default;

            Consumer(Buffer<buffer_t>& buffer, synchronization& sync, function_t funct, args_tuple_t& args) :  _buffer{buffer}, 
                                                                                                                _sync{sync},
                                                                                                                _consumer{funct},
                                                                                                                _args{args} {}

            void run(){
                while(!_sync._exitThread){
                    std::unique_lock<std::mutex> lockGuard(_sync._buffer_mutex);
                    auto bufferNotEmpty = _sync._conditionVar.wait_for(lockGuard,
                                                                        std::chrono::milliseconds(10),
                                                                        [this](){return !_buffer.isEmpty();});
                    if(bufferNotEmpty){
                        auto params = std::tuple_cat(std::make_tuple(_buffer.get()), _args);
                        std::apply(_consumer, params);
                    }
                    lockGuard.unlock();
                    _sync._conditionVar.notify_one();
                }
            }
    };
}