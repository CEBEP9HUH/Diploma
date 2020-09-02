#pragma once


#include <type_traits>
#include <cassert>


#include "Buffer.hpp"


namespace Diploma{

    template<typename function_t, typename...Args>
    class Producer;

    template<typename function_t, typename...Args>
    class Producer <function_t, std::tuple<Args...> > {
        public:
            static_assert(std::is_invocable<function_t, Args...>::value);
            using return_t = typename std::invoke_result<function_t, Args...>::type;
            using signature_t = function_t;
            using args_tuple_t = std::tuple<Args...>;
        private:
            Buffer<return_t>& _buffer;
            synchronization& _sync;
            function_t _producer;
            args_tuple_t _args;
        public:
            Producer() = delete;
            Producer(const Producer&) = delete;
            Producer(Producer&&) = delete;
            Producer& operator=(const Producer&) = delete;
            Producer& operator=(Producer&&) = delete;
            ~Producer() = default;

            Producer(Buffer<return_t>& buffer, synchronization& sync, function_t funct, args_tuple_t& args) :  _buffer{buffer}, 
                                                                                                                _sync{sync},
                                                                                                                _producer{funct},
                                                                                                                _args{args} {}

            void run(){
                while(!_sync._exitThread){
                    std::unique_lock<std::mutex> lockGuard(_sync._buffer_mutex);
                    auto bufferNotFull = _sync._conditionVar.wait_for(lockGuard, std::chrono::milliseconds(1), [this](){return !_buffer.isFull();});
                    if(bufferNotFull){
                        _buffer.insert(std::apply(_producer, _args));
                    }
                    lockGuard.unlock();
                    _sync._conditionVar.notify_all();
                    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
                }
            }
    };
}