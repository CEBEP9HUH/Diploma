#pragma once


#include <vector>
#include <memory>
#include <thread>
#include <type_traits>


#include "Consumer.hpp"
#include "Producer.hpp"
#include "Buffer.hpp"


namespace Diploma{
/*     template <typename R, typename ... Types> constexpr size_t getArgumentCount( R(*f)(Types ...))
    {
        return sizeof...(Types);
    } */

    template<typename producer_t, typename consumer_t, size_t bufferSize, typename...Args>
    class ProducerConsumer {
        public:
            using buffer_t = typename producer_t::return_t;
            static_assert(!std::is_same<buffer_t, void>::value, "return type of producer_t::function_t cannot be void");
        private:
            static_assert(std::is_base_of<Producer<typename producer_t::signature_t, typename producer_t::args_tuple_t>, producer_t>::value, "producer_t must be inherited from Producer<function_t, ...Args>");
            static_assert(std::is_base_of<Consumer<typename consumer_t::signature_t, Args...>, consumer_t>::value, "consumer_t must be inherited from Consumer<function_t, ...Args>");
            std::vector<std::unique_ptr<Producer<typename producer_t::signature_t, typename producer_t::args_tuple_t> > > _producers;
            std::vector<std::unique_ptr<Consumer<typename consumer_t::signature_t, Args...> > > _consumers;
            std::vector<std::thread> _producerThreads;
            std::vector<std::thread> _consumerThreads;
            Buffer<buffer_t> _buffer;
            std::thread _mainLoop;
            synchronization _sync;
            size_t _bufferSize = bufferSize;
            void mainLoop(){
                _producerThreads.reserve(_producers.size());
                _consumerThreads.reserve(_consumers.size());
                for(auto& p : _producers){
                    _producerThreads.emplace_back(&producer_t::run, &*p);
                }
                for(auto& c : _consumers){
                    _consumerThreads.emplace_back(&consumer_t::run, &*c);
                }
            }
        public:
            ProducerConsumer(const ProducerConsumer&) = delete;
            ProducerConsumer(ProducerConsumer&&) = delete;
            ProducerConsumer& operator=(const ProducerConsumer&) = delete;
            ProducerConsumer& operator=(ProducerConsumer&&) = delete;
            ~ProducerConsumer() {
                if(!_sync._exitThread){
                    stop();
                }
                for(auto& p : _producerThreads) {
                    p.join();
                }
                for(auto& c : _consumerThreads) {
                    c.join();
                }
            }

            ProducerConsumer() : ProducerConsumer{1, 1} {}
            explicit ProducerConsumer(const size_t producersCount, const size_t consumersCount, typename producer_t::signature_t& producerFunct, Args...args) : _buffer{_bufferSize} {
                _producers.reserve(producersCount);
                _consumers.reserve(consumersCount);
                for(size_t i = 0; i < producersCount; ++i){
                    _producers.emplace_back(std::make_unique<producer_t>(_buffer, _sync));
                }
                for(size_t i = 0; i < consumersCount; ++i){
                    _consumers.emplace_back(std::make_unique<consumer_t>(_buffer, _sync, producerFunct, args...));
                }
            }


            void run() {
                std::thread tmp(&ProducerConsumer::mainLoop, &*this);
                _mainLoop.swap(tmp);
            }

            void stop(){
                _mainLoop.join();
                _sync._buffer_mutex.lock();
                _sync._exitThread = true;
                _sync._buffer_mutex.unlock();
            }
    };
}