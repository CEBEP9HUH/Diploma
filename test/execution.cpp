#include <gtest/gtest.h>
#include <memory>
#include <mutex>


#include "PBCController.hpp"
#include "test_helpers.hpp"


TEST(EXECUTION, INFINITE_PRODUCER){
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(5);
    Diploma::PBCController<int> pbc(buffer);
    pbc.addProducers<Diploma::InfiniteProducer<int, int(*)()> >(4, producerFuncNoParam);
    pbc.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    pbc.stop();
    while(!buffer->isEmpty()){
        ASSERT_EQ(buffer->get(), producerFuncNoParam());
    }
}


TEST(EXECUTION, INFINITE_PRODUCER_CONSUMER){
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(5000);
    Diploma::PBCController<int> pbc(buffer);
    pbc.addProducers<Diploma::InfiniteProducer<int, int(*)()> >(4, producerFuncNoParam);
    ConsumerFuncNoParam a;
    pbc.addConsumers<Diploma::Consumer<int, ConsumerFuncNoParam> >(4, a);
    pbc.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    pbc.stop();
}


TEST(EXECUTION, PREDICATE_PRODUCER_CONSUMER_1){
    auto buffer = std::make_shared<Diploma::Buffer<int> >(5);
    Diploma::PBCController<int> pbc(buffer);
    ProducerPredicatedFunc pp;
    ConsumerFuncNoParam a;
    pbc.addProducers<Diploma::PredicatedProducer<int, ProducerPredicatedFunc> >(10, pp);
    pbc.addConsumers<Diploma::Consumer<int, ConsumerFuncNoParam> >(1, a);
    pbc.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    pbc.stop();
}