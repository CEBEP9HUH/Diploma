#pragma once


#include <type_traits>
#include <cassert>


#include "Buffer.hpp"


/* 
    TODO
    end with producer (rework function parameters into tuple)
    replace Args... in ProducerConsumer with tuples (in constructor etc.)
 */

namespace Diploma{
    template<size_t S>
    auto foo1(){
    return std::tuple<>();
    }

    template<size_t S, typename Head, typename...Tail>
    auto foo1(Head h, Tail...t){
    if constexpr (S >= 0)
        return std::tuple_cat(std::make_tuple(h), foo1<S-1, Tail...>(t...));
    else
        return std::tuple<>();
    }

    template<size_t S, typename...Args>
    auto foo(Args...args){
    return foo1<S, Args...>(args...);
    }

    template<size_t S, typename Head, typename...Tail>
    auto bar1(Head h, Tail...t){
    if constexpr (S > 0)
        return bar1<S-1, Tail...>(t...);
    else 
        return std::make_tuple(t...);
    }

    template<size_t S, typename...Args>
    auto bar(Args...args){
    return bar1<S, Args...>(args...);
    }

    template<size_t S, typename...Args>
    auto separate(Args...args){
    return std::make_pair(foo<S, Args...>(args...), bar<S, Args...>(args...));
    }
}

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
            function_t* _producer;
            args_tuple_t _args;
            //std::queue<return_t> _localBuffer;
            return_t produceData(){
                return std::apply(*_producer, _args);
            }
        public:
            Producer() = delete;
            Producer(const Producer&) = delete;
            Producer(Producer&&) = delete;
            Producer& operator=(const Producer&) = delete;
            Producer& operator=(Producer&&) = delete;
            ~Producer() = default;

            Producer(Buffer<return_t>& buffer, synchronization& sync) : _buffer{buffer}, 
                                                                        _sync{sync},
                                                                        _producer{nullptr} {}

            void setProduceMethod(function_t& funct, args_tuple_t args){
                _producer = &funct;
                _args(args);
            }

            void run(){
                assert(_producer!=nullptr);
                while(!_sync._exitThread){
                    std::unique_lock<std::mutex> lockGuard(_sync._buffer_mutex);
                    _sync._conditionVar.wait(lockGuard, [this](){return !_buffer.isFull();});
                    return_t data = produceData();
                    _buffer.insert(data);
                    lockGuard.unlock();
                    _sync._conditionVar.notify_one();
                    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
                }
            }
    };
}