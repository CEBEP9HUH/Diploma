#pragma once
/* 
    PBCController is a Producer-Buffer-Consumer Controller class. It 
    provides tools to adding, deleting, running and stopping producers 
    and consumers. Also it creates buffer and syncronization tool.
        producer_t is a producer type. It have to be derived from 
    ProducerBase type.
        consumer_t is a consumer type. It have to be derived from 
    ConsumerBase type.
        bufferSize is a maximum size of buffer.
*/

#include <vector>
#include <memory>
#include <thread>
#include <type_traits>


#include "Consumer.hpp"
#include "Producer.hpp"
#include "Buffer.hpp"


namespace Diploma{
    template<typename producer_t, typename consumer_t, size_t bufferSize>
    class PBCController {
        public:
            using buffer_t = typename producer_t::return_t;
            static_assert(!std::is_same<buffer_t, void>::value, "return type of buffer cannot be void");
        private:
            using producerBase_t = ProducerBase<buffer_t, typename producer_t::signature_t, typename producer_t::args_tuple_t>;
            using consumerBase_t = ConsumerBase<buffer_t, typename consumer_t::signature_t, typename consumer_t::args_tuple_t>;
            static_assert(std::is_base_of<producerBase_t, producer_t>::value, "producer_t must be inherited from ProducerBase<buffer_t, function_t, std::tuple<Args...> >");
            static_assert(std::is_base_of<consumerBase_t, consumer_t>::value, "consumer_t must be inherited from ConsumerBase<buffer_t, function_t, std::tuple<Args...> >");
            std::vector<std::unique_ptr<IFunctionCaller> > _producers;
            std::vector<std::unique_ptr<IFunctionCaller> > _consumers;
            std::vector<std::thread> _producerThreads;
            std::vector<std::thread> _consumerThreads;
            Buffer<buffer_t> _buffer;
            std::thread _mainLoop;
            synchronization _sync;
            bool running = false;

            template<typename T>
            void runThreads(std::vector<std::thread>& threads, 
                            const std::vector<std::unique_ptr<T> >& source){
                threads.clear();
                threads.shrink_to_fit();
                threads.reserve(source.size());
                for(auto& s : source){
                    threads.emplace_back(&T::run, &*s);
                }
            }

            void mainLoop(){
                runThreads(_producerThreads, _producers);
                runThreads(_consumerThreads, _consumers);
            }
        public:
            PBCController() = delete;
            PBCController(const PBCController&) = delete;
            PBCController(PBCController&&) = delete;
            PBCController& operator=(const PBCController&) = delete;
            PBCController& operator=(PBCController&&) = delete;
            ~PBCController() {
                stop();
            }

            explicit PBCController( const size_t producersCount, 
                                    typename producer_t::signature_t producerFunct, 
                                    typename producer_t::args_tuple_t producerArgs, 
                                    const size_t consumersCount, 
                                    typename consumer_t::signature_t consumerFunct, 
                                    typename consumer_t::args_tuple_t consumerArgs) : _buffer{bufferSize} {
                addProducers(producersCount, producerFunct, producerArgs);
                addConsumers(consumersCount, consumerFunct, consumerArgs);
            }

            // add count of producers. both the function and args can be different 
            // from producer's function and args passed in constructor (but type have to be the same).
            void addProducers(const size_t count,
                            typename producer_t::signature_t producerFunct, 
                            typename producer_t::args_tuple_t producerArgs) {                
                _producers.reserve(_producers.size() + count);
                for(size_t i = 0; i < count; ++i){
                    _producers.emplace_back(std::make_unique<producer_t>(_buffer, _sync, producerFunct, producerArgs));
                }
            }


            // add count of consumers. both the function and args can be different 
            // from consumer's function and args passed in constructor (but type have to be the same).
            void addConsumers(const size_t count,
                            typename consumer_t::signature_t consumerFunct, 
                            typename consumer_t::args_tuple_t consumerArgs) {
                _consumers.reserve(_consumers.size() + count);
                for(size_t i = 0; i < count; ++i){
                    _consumers.emplace_back(std::make_unique<consumer_t>(_buffer, _sync, consumerFunct, consumerArgs));
                }
            }

            // starts the produce-consume process
            void run() {
                if(!running){
                    _sync._exitThread = false;
                    std::thread tmp(&PBCController::mainLoop, &*this);
                    _mainLoop.swap(tmp);
                    _mainLoop.join();
                    running = true;
                }
            }

            //stops the produce-consume process
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

            // checks if the produce-consume process is running
            inline bool isRunning() const noexcept {
                return running;
            }

            // clears producers list
            void clearProducers(){
                _producers.clear();
                _producers.shrink_to_fit();
            }


            // clears consumers list
            void clearConsumers(){
                _consumers.clear();
                _consumers.shrink_to_fit();
            }
    };
}