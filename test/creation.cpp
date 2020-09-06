#include <gtest/gtest.h>
#include <memory>
#include <mutex>


#include "helpers.hpp"
#include "PBCController.hpp"


TEST(INFINITE_PRODUCER_CREATION, FUNCTION_NO_PARAMETERS){
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::InfiniteProducer<int, int(*)()> p(buffer, producerFuncNoParam);
}

TEST(INFINITE_PRODUCER_CREATION, LAMBDA_NO_PARAMETERS){
    auto lambda = [](){return 1;};
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::InfiniteProducer<int, decltype(lambda)> p(buffer,lambda);
}

TEST(INFINITE_PRODUCER_CREATION, FUNCTOR_NO_PARAMETERS){
    ProducerFuncNoParam functor;
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::InfiniteProducer<int, ProducerFuncNoParam> p(buffer, functor);
}

TEST(INFINITE_PRODUCER_CREATION, FUNCTION_PARAMETRIZED){
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::InfiniteProducer<int, int(*)(int), int> p(buffer, producerParametrisedFunc, 1);
}

TEST(INFINITE_PRODUCER_CREATION, LAMBDA_PARAMETRIZED){
    auto lambda = [](int a){return a;};
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::InfiniteProducer<int, decltype(lambda), int> p(buffer, lambda, 1);
}

TEST(INFINITE_PRODUCER_CREATION, FUNCTOR_PARAMETRIZED){
    ProduserParametrisedFunc functor;
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::InfiniteProducer<int, ProduserParametrisedFunc, int> p(buffer, functor, 1);
}

TEST(LOOPED_PRODUCER_CREATION, FUNCTION_NO_PARAMETERS){
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::LoopedProducer<int, int(*)()> p(buffer, producerFuncNoParam, 100);
}

TEST(LOOPED_PRODUCER_CREATION, LAMBDA_NO_PARAMETERS){
    auto lambda = [](){return 1;};
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::LoopedProducer<int, decltype(lambda)> p(buffer, lambda, 100);
}

TEST(LOOPED_PRODUCER_CREATION, FUNCTOR_NO_PARAMETERS){
    ProducerFuncNoParam functor;
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::LoopedProducer<int, ProducerFuncNoParam> p(buffer, functor, 100);
}

TEST(LOOPED_PRODUCER_CREATION, FUNCTION_PARAMETRIZED){
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::LoopedProducer<int, int(*)(int), int> p(buffer,  producerParametrisedFunc, 1, 100);
}

TEST(LOOPED_PRODUCER_CREATION, LAMBDA_PARAMETRIZED){
    auto lambda = [](int a){return a;};
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::LoopedProducer<int, decltype(lambda), int> p(buffer, lambda, 1, 100);
}

TEST(LOOPED_PRODUCER_CREATION, FUNCTOR_PARAMETRIZED){
    ProduserParametrisedFunc functor;
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::LoopedProducer<int, ProduserParametrisedFunc, int> p(buffer, functor, 1, 100);
}

TEST(PREDICATE_PRODUCER_CREATION, FUNCTION_NO_PARAMETERS){
    ProducerPredicatedFunc pp;
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::PredicatedProducer<int, ProducerPredicatedFunc> p(buffer, pp);
}

TEST(CONSUMER_CREATION, FUNCTION_NO_PARAMETERS){
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::Consumer<int, void(*)(int)> c(buffer, consumerFuncNoParam);
}

TEST(CONSUMER_CREATION, LAMBDA_NO_PARAMETERS){
    auto lambda = [](int a){(void)(a);};
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::Consumer<int, decltype(lambda)> c(buffer, lambda);
}

TEST(CONSUMER_CREATION, FUNCTOR_NO_PARAMETERS){
    ConsumerFuncNoParam functor;
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::Consumer<int, ConsumerFuncNoParam> c(buffer, functor);
}

TEST(CONSUMER_CREATION, FUNCTION_PARAMETRIZED){
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::Consumer<int, void(*)(int, int), int> c(buffer, consumerParametrisedFunc, 1);
}

TEST(CONSUMER_CREATION, LAMBDA_PARAMETRIZED){
    auto lambda = [](int a, int b){(void)(a); (void)(b);};
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::Consumer<int, decltype(lambda), int> c(buffer, lambda, 1);
}

TEST(CONSUMER_CREATION, FUNCTOR_PARAMETRIZED){
    ConsumerParametrisedFunc functor;
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::Consumer<int, ConsumerParametrisedFunc, int> c(buffer, functor, 1);
}

TEST(PBC_CREATION, SIMPLE){
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::PBCController<int> pbc(buffer);
}