#pragma once
/* 
    ProducerBase<buffer_t, function_t, std::tuple<Args...> > is a base for 
    all producers which is possible to use with PBCController class
        buffer_t - is a type of storing data in buffer which will be used
        function_t - producer's function signature. It have to has return 
    type equals to buffer_t. Function of this type will be passed in 
    constructor and will be used to generate data for buffer
        Args... - is an enumeration of types of variables  which must be passed 
    into producer's function
    
    InfiniteProducer is an implementation of ProducerBase which repeats 
    producer function call until program end
    LoopedProducer is an implementation of ProducerBase which repeats 
    producer function call a finite number of times
    PredicatedProducer is an implementation of ProducerBase which repeats 
    producer function call until predicate returns false. In this case function_t
    have to be inherited from Predicate class
*/

#include <type_traits>


#include "IFunctionCaller.hpp"
#include "Buffer.hpp"


namespace Diploma{
    template<typename buffer_t, typename function_t, typename...Args>
    class ProducerBase : public IProducerBase {
    public:
        static_assert(std::is_invocable<function_t, Args...>::value);
    protected:
        std::shared_ptr<BufferBase<buffer_t> > _buffer;
        accessSync& _sync;
        function_t _producer;
        std::tuple<Args...> _args;
    protected:
        virtual void produce(){
            std::unique_lock<std::mutex> lockGuard(_sync._buffer_mutex);
            auto bufferNotFull = _sync._conditionVar.wait_for(lockGuard, 
                                                            std::chrono::nanoseconds(1), 
                                                            [this](){return !_buffer->isFull();}); 
            if(bufferNotFull){
                _buffer->insert(std::apply(_producer, _args));
            }
            lockGuard.unlock();
            _sync._conditionVar.notify_all(); 
        }
    public:
        ProducerBase(const std::shared_ptr<BufferBase<buffer_t> >& buffer, 
                    function_t funct, 
                    const Args&...args) :   _buffer{buffer}, 
                                            _sync{buffer->getSync()},
                                            _producer{funct},
                                            _args{args...} {}
        virtual ~ProducerBase() = default;
    };

    template<typename buffer_t, typename function_t, typename...Args>
    class InfiniteProducer : public ProducerBase<buffer_t, function_t, Args...>{
    private:
        using produserBase_t = ProducerBase<buffer_t, function_t, Args... >;
        static_assert(std::is_same_v<typename std::invoke_result<function_t, Args...>::type, buffer_t>);
    public:
        InfiniteProducer() = delete;
        InfiniteProducer(const InfiniteProducer&) = delete;
        InfiniteProducer(InfiniteProducer&&) = delete;
        InfiniteProducer& operator=(const InfiniteProducer&) = delete;
        InfiniteProducer& operator=(InfiniteProducer&&) = delete;
        virtual ~InfiniteProducer() = default;

        InfiniteProducer(const std::shared_ptr<BufferBase<buffer_t> >& buffer, 
                        function_t funct, 
                        const Args&...args) : produserBase_t{buffer, funct, args...} {}

        virtual void run() override {
            while(!this->_sync._exitThread){
                this->produce();
            }
        }
    };


    template<typename buffer_t, typename function_t, typename...Args>
    class LoopedProducer : public ProducerBase<buffer_t, function_t, Args...>{
    private:
        using produserBase_t = ProducerBase<buffer_t, function_t, Args...>;
        size_t _repeatsCount;
    public:
        LoopedProducer() = delete;
        LoopedProducer(const LoopedProducer&) = delete;
        LoopedProducer(LoopedProducer&&) = delete;
        LoopedProducer& operator=(const LoopedProducer&) = delete;
        LoopedProducer& operator=(LoopedProducer&&) = delete;
        virtual ~LoopedProducer() = default;

        LoopedProducer(const std::shared_ptr<BufferBase<buffer_t> >& buffer, 
                        function_t funct, 
                        const size_t repeatsCount,
                        const Args&...args) :   produserBase_t{buffer, funct, args...}, 
                                                _repeatsCount{repeatsCount} {}

        virtual void run() override {
            for(size_t i=0; !this->_sync._exitThread &&  i<_repeatsCount; ++i){
                this->produce();
            }
        }

    };

    class Predicate{
        public:
        virtual bool stopProducer() = 0;
    };

    template<typename buffer_t, typename function_t, typename...Args>
    class PredicatedProducer : public ProducerBase<buffer_t, function_t, Args...> {
    protected:
        using produserBase_t = ProducerBase<buffer_t, function_t, Args...>;
        static_assert(std::is_base_of<Predicate, function_t>::value);
    public:
        PredicatedProducer() = delete;
        PredicatedProducer(const PredicatedProducer&) = delete;
        PredicatedProducer(PredicatedProducer&&) = delete;
        PredicatedProducer& operator=(const PredicatedProducer&) = delete;
        PredicatedProducer& operator=(PredicatedProducer&&) = delete;
        virtual ~PredicatedProducer() = default;

        PredicatedProducer(const std::shared_ptr<BufferBase<buffer_t> >& buffer, 
                            function_t funct, 
                            const Args&...args) : produserBase_t{buffer, funct, args...} {}

        virtual void run() override {
            while(!(this->_sync._exitThread || this->_producer.stopProducer())){
                this->produce();
            }
        }
    }; 
}