#pragma once
/* 
    ConsumerBase<buffer_t, function_t, std::tuple<Args...> > is a base for 
    all consumers which is possible to use with PBCController class
        buffer_t - is a type of storing data in buffer which will be used
        function_t - consumer's function signature. It have to has first 
    parameter of type buffer_t. Function of this type will be passed in 
    constructor and will be used to process data from buffer. First parameter 
    is using to pass data from buffer
        Args... - is an enumeration of types of variables  which must be passed 
    into consumer's function. It have to not include first parameter's type.
    First parameter's type will be evaluated and added automatically. It's a buffer
    value's type
    
    Consumer is an implementation of ConsumerBase 
*/

#include <type_traits>


#include "IFunctionCaller.hpp"
#include "Buffer.hpp"
#include "defines.hpp"


namespace Diploma{
    template<typename buffer_t, typename function_t, typename...Args>
    class ConsumerBase : public IConsumerBase {
    public:
        static_assert(std::is_invocable<function_t, buffer_t, Args...>::value);
    protected:
        std::shared_ptr<BufferBase<buffer_t> > _buffer;
        accessSync& _sync;
        function_t _consumer;
        std::tuple<Args...> _args;
        struct {
            buffer_t value;
            bool processed = true;
        } _localBuffer;
        virtual void consume(){
            std::unique_lock<std::mutex> lockGuard(_sync._buffer_mutex);
            auto bufferNotEmpty = _sync._conditionVar.wait_for(lockGuard,
                                                                std::chrono::nanoseconds(CONSUMERS_CONDITION_VAR_AWAITING_NS),
                                                                [this](){return !_buffer->isEmpty();});
            if(bufferNotEmpty){
                _localBuffer.value = _buffer->get();
                _localBuffer.processed = false;
            }
            if(!_localBuffer.processed){
                auto params = std::tuple_cat(std::make_tuple(_localBuffer.value), _args);
                std::apply(_consumer, params);
                _localBuffer.processed = true;
            }
            lockGuard.unlock();
            _sync._conditionVar.notify_all();
        }
    public:
        ConsumerBase(const std::shared_ptr<BufferBase<buffer_t> >& buffer, 
                    const function_t& funct, 
                    const Args&...args) :   _buffer{buffer}, 
                                            _sync{buffer->getSync()},
                                            _consumer{funct},
                                            _args{args...} {}
        virtual ~ConsumerBase() = default;
    };

    template<typename buffer_t, typename function_t, typename...Args>
    class Consumer : public ConsumerBase<buffer_t, function_t, Args...>{
    private:
        using consumerBase_t = ConsumerBase<buffer_t, function_t, Args...>;
    public:
        Consumer() = delete;
        Consumer(const Consumer&) = delete;
        Consumer(Consumer&&) = delete;
        Consumer& operator=(const Consumer&) = delete;
        Consumer& operator=(Consumer&&) = delete;
        virtual ~Consumer() = default;

        Consumer(const std::shared_ptr<BufferBase<buffer_t> >& buffer, 
                const function_t& funct, 
                const Args&...args) : consumerBase_t{buffer, funct, args...} {}

        virtual void run() override {
            while(!this->_sync._exitThread){
                this->consume();
            }
        }
    };
}