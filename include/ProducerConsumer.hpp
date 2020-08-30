#pragma once


#include <vector>
#include <memory>
#include <thread>


#include "Consumer.hpp"
#include "Producer.hpp"
#include "Buffer.hpp"


namespace Diploma{
    template<typename T, typename producer_t, typename consumer_t, size_t bufferSize, typename function_t, typename...Args>
    class ProducerConsumer {
        private:
            static_assert(std::is_base_of<Producer<T, function_t, Args...>, producer_t>::value, "Producer must be inherited from Producer<T>");
            static_assert(std::is_base_of<Consumer<T>, consumer_t>::value, "Consumer must be inherited from Consumer<T>");
            std::vector<std::unique_ptr<Producer<T, function_t, Args...> > > _producers;
            std::vector<std::unique_ptr<Consumer<T> > > _consumers;
            std::vector<std::thread> _producerThreads;
            std::vector<std::thread> _consumerThreads;
            Buffer<T> _buffer;
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
            explicit ProducerConsumer(const size_t producersCount, const size_t consumersCount, function_t& producerFunct, Args...args) : _buffer{_bufferSize} {
                _producers.reserve(producersCount);
                _consumers.reserve(consumersCount);
                for(size_t i = 0; i < producersCount; ++i){
                    _producers.emplace_back(std::make_unique<Producer<T, function_t, Args...> >(_buffer, _sync, producerFunct, args...));
                }
                for(size_t i = 0; i < consumersCount; ++i){
                    _consumers.emplace_back(std::make_unique<Consumer<T> >(_buffer, _sync));
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