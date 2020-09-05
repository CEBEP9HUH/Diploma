#include <gtest/gtest.h>
#include <memory>
#include <mutex>


#include "PBCController.hpp"


int producer1(){
    return 1;
}

class Functor1{
    public:
        int operator()(){
            return 1;
        }
};

int producer2(int a){
    return a;
}

class Functor2{
    public:
        int operator()(int a){
            return a;
        }
};

TEST(INFINITE_PRODUCER_CREATION, FUNCTION_NO_PARAMETERS){
    Diploma::synchronization s;
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::InfiniteProducer<int, int(*)(), std::tuple<> > p(buffer, s, producer1, std::tuple<>());
}

TEST(INFINITE_PRODUCER_CREATION, LAMBDA_NO_PARAMETERS){
    auto lambda = [](){return 1;};
    Diploma::synchronization s;
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::InfiniteProducer<int, decltype(lambda), std::tuple<> > p(buffer, s, lambda, std::tuple<>());
}

TEST(INFINITE_PRODUCER_CREATION, FUNCTOR_NO_PARAMETERS){
    Functor1 functor;
    Diploma::synchronization s;
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::InfiniteProducer<int, Functor1, std::tuple<> > p(buffer, s, functor, std::tuple<>());
}

TEST(INFINITE_PRODUCER_CREATION, FUNCTION_PARAMETRIZED){
    Diploma::synchronization s;
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::InfiniteProducer<int, int(*)(int), std::tuple<int> > p(buffer, s, producer2, std::tuple<int>(1));
}

TEST(INFINITE_PRODUCER_CREATION, LAMBDA_PARAMETRIZED){
    auto lambda = [](int a){return a;};
    Diploma::synchronization s;
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::InfiniteProducer<int, decltype(lambda), std::tuple<int> > p(buffer, s, lambda, std::tuple<int>(1));
}

TEST(INFINITE_PRODUCER_CREATION, FUNCTOR_PARAMETRIZED){
    Functor2 functor;
    Diploma::synchronization s;
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::InfiniteProducer<int, Functor2, std::tuple<int> > p(buffer, s, functor, std::tuple<int>(1));
}




/* TEST(LOOPED_PRODUCER_CREATION, FUNCTION_NO_PARAMETERS){
    Diploma::synchronization s;
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::LoopedProducer<int, int(*)(), std::tuple<> > p(buffer, s, producer, std::tuple<>(), 50);
}


TEST(CONSUMER_CREATION, FUNCTION_NO_PARAMETERS){
    Diploma::synchronization s;
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::Consumer<int, void(*)(int), std::tuple<> > c(buffer, s, consumer, std::tuple<>());
}


TEST(CREATION, LOOPED_PRODUCER_LAMBDA){
    auto lambda = [](){return 1;};
    Diploma::synchronization s;
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::LoopedProducer<int, int(*)(), std::tuple<> > p(buffer, s, producer, std::tuple<>(), 50);
}


TEST(CREATION, CONSUMER_LAMBDA){
    Diploma::synchronization s;
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::Consumer<int, void(*)(int), std::tuple<> > c(buffer, s, consumer, std::tuple<>());
} */