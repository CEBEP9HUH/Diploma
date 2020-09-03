#pragma once
/* 
    ProducerBase<buffer_t, function_t, std::tuple<Args...> > is a base for 
    all producers which is possible to use with PBCController class
        buffer_t - is a type of storing data in buffer which will be used
        function_t - producer's function signature. It have to has return 
    type equals to buffer_t. Function of this type will be passed in 
    constructor and will be used to generate data for buffer
        std::tuple<Args...> - is a tuple of types which must be passed 
    into producer's function
    
    Producer is an implementation of ProducerBase 
*/

#include <type_traits>


#include "Buffer.hpp"


namespace Diploma{
    template<typename buffer_t, typename function_t, typename...Args>
    class ProducerBase;

    template<typename buffer_t, typename function_t, typename...Args>
    class Producer : public ProducerBase<buffer_t, function_t, Args...>{};

    template<typename buffer_t, typename function_t, typename...Args>
    class ProducerBase<buffer_t, function_t, std::tuple<Args...> >{
    public:
        static_assert(std::is_invocable<function_t, Args...>::value);
        using return_t = typename std::invoke_result<function_t, Args...>::type;
        static_assert(std::is_same_v<buffer_t, return_t>);
        using signature_t = function_t;
        using args_tuple_t = std::tuple<Args...>;
    protected:
        Buffer<buffer_t>& _buffer;
        synchronization& _sync;
        function_t _producer;
        args_tuple_t _args;
    public:
        ProducerBase(Buffer<buffer_t>& buffer, synchronization& sync, 
                    function_t funct, args_tuple_t& args) : _buffer{buffer}, 
                                                            _sync{sync},
                                                            _producer{funct},
                                                            _args{args} {}
        virtual void run() = 0;
        virtual ~ProducerBase() = default;
    };

    template<typename buffer_t, typename function_t, typename...Args>
    class Producer <buffer_t, function_t, std::tuple<Args...> >  : public ProducerBase<buffer_t, function_t, std::tuple<Args...> > {
    private:
        using produserBase_t = ProducerBase<buffer_t, function_t, std::tuple<Args...> >;
    public:
        Producer() = delete;
        Producer(const Producer&) = delete;
        Producer(Producer&&) = delete;
        Producer& operator=(const Producer&) = delete;
        Producer& operator=(Producer&&) = delete;
        virtual ~Producer() = default;

        Producer(Buffer<buffer_t>& buffer, synchronization& sync, 
                function_t funct, typename produserBase_t::args_tuple_t& args) : produserBase_t{buffer, sync, funct, args} {}

        virtual void run() override {
            while(!this->_sync._exitThread){
                std::unique_lock<std::mutex> lockGuard(this->_sync._buffer_mutex);
                auto bufferNotFull = this->_sync._conditionVar.wait_for(lockGuard, 
                                                                        std::chrono::milliseconds(1), 
                                                                        [this](){return !this->_buffer.isFull();});
                if(bufferNotFull){
                    this->_buffer.insert(std::apply(this->_producer, this->_args));
                }
                lockGuard.unlock();
                this->_sync._conditionVar.notify_all();
                std::this_thread::sleep_for(std::chrono::nanoseconds(1));
            }
        }
    };
}