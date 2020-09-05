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
    //template<typename producer_t, typename consumer_t, size_t bufferSize>
    template<typename buffer_t>
    class PBCController {
        private:
            std::vector<std::unique_ptr<IProducerBase> > _producers;
            std::vector<std::unique_ptr<IConsumerBase> > _consumers;
            std::vector<std::thread> _producerThreads;
            std::vector<std::thread> _consumerThreads;
            std::shared_ptr<BufferBase<buffer_t> > _buffer;
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

            explicit PBCController(const std::shared_ptr<BufferBase<buffer_t> >& buffer) : _buffer{buffer} {
            }

            // add count of producers. both the function and args can be different 
            // from producer's function and args passed in constructor (but type have to be the same).
            template<typename producer_t, typename...Args>
            bool addProducers(const size_t count, Args...args) {        
                if(!running){        
                    _producers.reserve(_producers.size() + count);
                    for(size_t i = 0; i < count; ++i){
                        _producers.emplace_back(std::make_unique<producer_t>(_buffer, _sync, args...));
                    }
                    return true;
                }
                return false;
            }


            // add count of consumers. both the function and args can be different 
            // from consumer's function and args passed in constructor (but type have to be the same).
            template<typename consumer_t, typename...Args>
            bool addConsumers(const size_t count, Args...args) {
                if(!running){
                    _consumers.reserve(_consumers.size() + count);
                    for(size_t i = 0; i < count; ++i){
                        _consumers.emplace_back(std::make_unique<consumer_t>(_buffer, _sync, args...));
                    }
                    return true;
                }
                return false;
            }

            // starts the produce-consume process
            void run() {
                if(!running){
                    _sync._exitThread = false;
                    mainLoop();
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
            bool clearProducers(){
                if(!running){
                    _producers.clear();
                    _producers.shrink_to_fit();
                    return true;
                }
                return false;
            }


            // clears consumers list
            bool clearConsumers(){
                if(!running){
                    _consumers.clear();
                    _consumers.shrink_to_fit();
                    return true;
                }
                return false;
            }
    };
}