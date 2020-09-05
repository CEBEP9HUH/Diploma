#pragma once
/* 
    ConsumerBase<buffer_t, function_t, std::tuple<Args...> > is a base for 
    all consumers which is possible to use with PBCController class
        buffer_t - is a type of storing data in buffer which will be used
        function_t - consumer's function signature. It have to has first 
    parameter of type buffer_t. Function of this type will be passed in 
    constructor and will be used to process data from buffer. First parameter 
    is using to pass data from buffer
        std::tuple<Args...> - is a tuple of types which must be passed 
    into producer's function. It have to not include first parameter's type.
    First parameter's type will be evaluated and added automatically.
    
    Consumer is an implementation of ConsumerBase 
*/

#include <type_traits>


#include "IFunctionCaller.hpp"
#include "Buffer.hpp"


namespace Diploma{
    template<typename buffer_t, typename function_t, typename...Args>
    class ConsumerBase : public IConsumerBase {};

    template<typename buffer_t, typename function_t, typename...Args>
    class ConsumerBase<buffer_t, function_t, std::tuple<Args...> > : 
    public IConsumerBase { 
    public:
        static_assert(std::is_invocable<function_t, buffer_t, Args...>::value);
        using signature_t = function_t;
        using args_tuple_t = std::tuple<Args...>;
    protected:
        std::shared_ptr<BufferBase<buffer_t> >& _buffer;
        synchronization& _sync;
        function_t _consumer;
        std::tuple<Args...> _args;
        virtual void consume(){
            std::unique_lock<std::mutex> lockGuard(_sync._buffer_mutex);
            auto bufferNotEmpty = _sync._conditionVar.wait_for(lockGuard,
                                                                    std::chrono::milliseconds(1),
                                                                    [this](){return !_buffer->isEmpty();});
            if(bufferNotEmpty){
                auto params = std::tuple_cat(std::make_tuple(_buffer->get()), _args);
                std::apply(_consumer, params);
            }
            lockGuard.unlock();
            _sync._conditionVar.notify_all();
            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
        }
    public:
        ConsumerBase(std::shared_ptr<BufferBase<buffer_t> >& buffer, synchronization& sync, 
                    function_t funct, args_tuple_t& args) : _buffer{buffer}, 
                                                            _sync{sync},
                                                            _consumer{funct},
                                                            _args{args} {}
        virtual ~ConsumerBase() = default;
    };

    template<typename buffer_t, typename function_t, typename...Args>
    class Consumer : public ConsumerBase<buffer_t, function_t, Args...>{
    };

    template<typename buffer_t, typename function_t, typename...Args>
    class Consumer <buffer_t, function_t, std::tuple<Args...> > : 
    public ConsumerBase<buffer_t, function_t, std::tuple<Args...> > {
    private:
        using consumerBase_t = ConsumerBase<buffer_t, function_t, std::tuple<Args...> >;
    public:
        Consumer() = delete;
        Consumer(const Consumer&) = delete;
        Consumer(Consumer&&) = delete;
        Consumer& operator=(const Consumer&) = delete;
        Consumer& operator=(Consumer&&) = delete;
        virtual ~Consumer() = default;

        Consumer(std::shared_ptr<BufferBase<buffer_t> >& buffer, synchronization& sync, 
            function_t funct, typename consumerBase_t::args_tuple_t& args) : consumerBase_t{buffer, sync, funct, args} {}

        virtual void run() override {
            while(!this->_sync._exitThread){
                this->consume();
            }
        }
    };
}