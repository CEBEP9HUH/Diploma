#pragma once


#include <vector>
#include <memory>
#include <thread>
#include <type_traits>


#include "Consumer.hpp"
#include "Producer.hpp"
#include "Buffer.hpp"


namespace Diploma{

    template<typename producer_t, typename consumer_t, size_t bufferSize>
    class ProducerConsumer {
        public:
            using buffer_t = typename producer_t::return_t;
            static_assert(!std::is_same<buffer_t, void>::value, "return type of producer_t::function_t cannot be void");
        private:
            static_assert(std::is_base_of<Producer<typename producer_t::signature_t, typename producer_t::args_tuple_t>, producer_t>::value, "producer_t must be inherited from Producer<function_t, ...Args>");
            static_assert(std::is_base_of<Consumer<typename consumer_t::signature_t, buffer_t, typename consumer_t::args_tuple_t>, consumer_t>::value, "consumer_t must be inherited from Consumer<function_t, ...Args>");
            std::vector<std::unique_ptr<producer_t> > _producers;
            std::vector<std::unique_ptr<consumer_t> > _consumers;
            std::vector<std::thread> _producerThreads;
            std::vector<std::thread> _consumerThreads;
            Buffer<buffer_t> _buffer;
            std::thread _mainLoop;
            synchronization _sync;
            bool running = false;
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
            ProducerConsumer() = delete;
            ProducerConsumer(const ProducerConsumer&) = delete;
            ProducerConsumer(ProducerConsumer&&) = delete;
            ProducerConsumer& operator=(const ProducerConsumer&) = delete;
            ProducerConsumer& operator=(ProducerConsumer&&) = delete;
            ~ProducerConsumer() {
                stop();
            }

            explicit ProducerConsumer(  const size_t producersCount, 
                                        typename producer_t::signature_t producerFunct, 
                                        typename producer_t::args_tuple_t producerArgs, 
                                        const size_t consumersCount, 
                                        typename consumer_t::signature_t consumerFunct, 
                                        typename consumer_t::args_tuple_t consumerArgs) : _buffer{bufferSize} {
                addProducers(producersCount, producerFunct, producerArgs);
                addConsumers(consumersCount, consumerFunct, consumerArgs);
            }

            void addProducers(const size_t count,
                            typename producer_t::signature_t producerFunct, 
                            typename producer_t::args_tuple_t producerArgs) {                
                _producers.reserve(_producers.size() + count);
                for(size_t i = 0; i < count; ++i){
                    _producers.emplace_back(std::make_unique<producer_t>(_buffer, _sync, producerFunct, producerArgs));
                }
            }

            void addConsumers(const size_t count,
                            typename consumer_t::signature_t consumerFunct, 
                            typename consumer_t::args_tuple_t consumerArgs) {
                _consumers.reserve(_consumers.size() + count);
                for(size_t i = 0; i < count; ++i){
                    _consumers.emplace_back(std::make_unique<consumer_t>(_buffer, _sync, consumerFunct, consumerArgs));
                }
            }

            void run() {
                if(!running){
                    _sync._exitThread = false;
                    std::thread tmp(&ProducerConsumer::mainLoop, &*this);
                    _mainLoop.swap(tmp);
                    _mainLoop.join();
                    running = true;
                }
            }

            void stop(){
                if(running){
                    _sync._buffer_mutex.lock();
                    _sync._exitThread = true;
                    _sync._buffer_mutex.unlock();
                    for(auto& p : _producerThreads) {
                        p.join();
                    }
                    for(auto& c : _consumerThreads) {
                        c.join();
                    }
                    running = false;
                }
            }

            inline bool isRunning() const noexcept {
                return running;
            }
    };
}