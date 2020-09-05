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
    
    InfiniteProducer is an implementation of ProducerBase which repeats 
    producer function call until program end
    LoopedProducer is an implementation of ProducerBase which repeats 
    producer function call a finite number of times
*/

#include <type_traits>


#include "IFunctionCaller.hpp"
#include "Buffer.hpp"


namespace Diploma{
    template<typename buffer_t, typename function_t, typename...Args>
    class ProducerBase : public IProducerBase {};

    template<typename buffer_t, typename function_t, typename...Args>
    class ProducerBase<buffer_t, function_t, std::tuple<Args...> > : 
    public IProducerBase{
    public:
        static_assert(std::is_invocable<function_t, Args...>::value);
        using signature_t = function_t;
        using args_tuple_t = std::tuple<Args...>;
    protected:
        std::shared_ptr<BufferBase<buffer_t> > _buffer;
        synchronization& _sync;
        function_t _producer;
        args_tuple_t _args;
        virtual void produce(){
            std::unique_lock<std::mutex> lockGuard(_sync._buffer_mutex);
            auto bufferNotFull = _sync._conditionVar.wait_for(lockGuard, 
                                                            std::chrono::milliseconds(1), 
                                                            [this](){return !_buffer->isFull();});
            if(bufferNotFull){
                _buffer->insert(std::apply(_producer, _args));
            }
            lockGuard.unlock();
            _sync._conditionVar.notify_all();
            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
        }
    public:
        ProducerBase(const std::shared_ptr<BufferBase<buffer_t> >& buffer, synchronization& sync, 
                    function_t funct, const args_tuple_t& args) : _buffer{buffer}, 
                                                            _sync{sync},
                                                            _producer{funct},
                                                            _args{args} {}
        virtual ~ProducerBase() = default;
    };


    template<typename buffer_t, typename function_t, typename...Args>
    class InfiniteProducer : public ProducerBase<buffer_t, function_t, Args...>{};

    template<typename buffer_t, typename function_t, typename...Args>
    class InfiniteProducer <buffer_t, function_t, std::tuple<Args...> >  : 
    public ProducerBase<buffer_t, function_t, std::tuple<Args...> > {
    private:
        using produserBase_t = ProducerBase<buffer_t, function_t, std::tuple<Args...> >;
    public:
        InfiniteProducer() = delete;
        InfiniteProducer(const InfiniteProducer&) = delete;
        InfiniteProducer(InfiniteProducer&&) = delete;
        InfiniteProducer& operator=(const InfiniteProducer&) = delete;
        InfiniteProducer& operator=(InfiniteProducer&&) = delete;
        virtual ~InfiniteProducer() = default;

        InfiniteProducer(const std::shared_ptr<BufferBase<buffer_t> >& buffer, synchronization& sync, 
                function_t funct, const typename produserBase_t::args_tuple_t& args) : produserBase_t{buffer, sync, funct, args} {}

        virtual void run() override {
            while(!this->_sync._exitThread){
                this->produce();
            }
        }
    };


    template<typename buffer_t, typename function_t, typename...Args>
    class LoopedProducer : public ProducerBase<buffer_t, function_t, Args...>{};

    template<typename buffer_t, typename function_t, typename...Args>
    class LoopedProducer <buffer_t, function_t, std::tuple<Args...> >  : public ProducerBase<buffer_t, function_t, std::tuple<Args...> > {
    private:
        using produserBase_t = ProducerBase<buffer_t, function_t, std::tuple<Args...> >;
        size_t _repeatsCount;
    public:
        LoopedProducer() = delete;
        LoopedProducer(const LoopedProducer&) = delete;
        LoopedProducer(LoopedProducer&&) = delete;
        LoopedProducer& operator=(const LoopedProducer&) = delete;
        LoopedProducer& operator=(LoopedProducer&&) = delete;
        virtual ~LoopedProducer() = default;

        LoopedProducer(const std::shared_ptr<BufferBase<buffer_t> >& buffer, synchronization& sync, 
                function_t funct, const typename produserBase_t::args_tuple_t& args, 
                const size_t repeatsCount) : produserBase_t{buffer, sync, funct, args}, 
                                            _repeatsCount{repeatsCount} {}

        virtual void run() override {
            for(size_t i=0; !this->_sync._exitThread &&  i<_repeatsCount; ++i){
                this->produce();
            }
        }
    };


/*     template<typename buffer_t, typename function_t, typename...Args>
    class PredicatedProducer : public ProducerBase<buffer_t, function_t, Args...>{};

    template<typename buffer_t, typename function_t, typename...Args>
    class PredicatedProducer <buffer_t, function_t, std::tuple<Args...> >  : 
    public ProducerBase<buffer_t, function_t, std::tuple<Args...> > {
    protected:
        using produserBase_t = ProducerBase<buffer_t, function_t, std::tuple<Args...> >;
        static_assert(std::is_same_v<std::pair<buffer_t, bool>, typename std::invoke_result<function_t, Args...>::type >);
        bool _finish = false;
        virtual void produce() override {
            std::unique_lock<std::mutex> lockGuard(this->_sync._buffer_mutex);
            auto bufferNotFull = this->_sync._conditionVar.wait_for(lockGuard, 
                                                            std::chrono::milliseconds(1), 
                                                            [this](){return !this->_buffer->isFull();});
            if(bufferNotFull){
                auto result = std::apply(this->_producer, this->_args);
                _finish = result.second;
                if(!_finish){
                    this->_buffer->insert(result.first);
                }
            }
            lockGuard.unlock();
            this->_sync._conditionVar.notify_all();
            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
        }
    public:
        PredicatedProducer() = delete;
        PredicatedProducer(const PredicatedProducer&) = delete;
        PredicatedProducer(PredicatedProducer&&) = delete;
        PredicatedProducer& operator=(const PredicatedProducer&) = delete;
        PredicatedProducer& operator=(PredicatedProducer&&) = delete;
        virtual ~PredicatedProducer() = default;

        PredicatedProducer(std::shared_ptr<BufferBase<buffer_t> >& buffer, synchronization& sync, 
                function_t funct, typename produserBase_t::args_tuple_t& args) : produserBase_t{buffer, sync, funct, args} {}

        virtual void run() override {
            while(!this->_sync._exitThread && !_finish){
                PredicatedProducer::produce();
            }
        }
    }; */
}