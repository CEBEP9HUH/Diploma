#include <gtest/gtest.h>
#include <memory>
#include <mutex>


#include "PBCController.hpp"


int producer1(){
    return 1;
}

void consumer1(int a){
    (void)(a);
}

class FunctorProdNoParam{
    public:
        int operator()(){
            return 1;
        }
};

class FunctorConsNoParam{
    public:
        void operator()(int a){
            (void)(a);
        }
};

int producer2(int a){
    return a;
}

void consumer2(int a, int b){
    (void)(a);
    (void)(b);
}

class FunctorProdParam{
    public:
        int operator()(int a){
            return a;
        }
};

class FunctorConsParam{
    public:
        void operator()(int a, int b){
            (void)(a);
            (void)(b);
        }
};

TEST(INFINITE_PRODUCER_CREATION, FUNCTION_NO_PARAMETERS){
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::InfiniteProducer<int, int(*)()> p(buffer, producer1);
}

TEST(INFINITE_PRODUCER_CREATION, LAMBDA_NO_PARAMETERS){
    auto lambda = [](){return 1;};
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::InfiniteProducer<int, decltype(lambda)> p(buffer,lambda);
}

TEST(INFINITE_PRODUCER_CREATION, FUNCTOR_NO_PARAMETERS){
    FunctorProdNoParam functor;
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::InfiniteProducer<int, FunctorProdNoParam> p(buffer, functor);
}

TEST(INFINITE_PRODUCER_CREATION, FUNCTION_PARAMETRIZED){
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::InfiniteProducer<int, int(*)(int), int> p(buffer, producer2, 1);
}

TEST(INFINITE_PRODUCER_CREATION, LAMBDA_PARAMETRIZED){
    auto lambda = [](int a){return a;};
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::InfiniteProducer<int, decltype(lambda), int> p(buffer, lambda, 1);
}

TEST(INFINITE_PRODUCER_CREATION, FUNCTOR_PARAMETRIZED){
    FunctorProdParam functor;
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::InfiniteProducer<int, FunctorProdParam, int> p(buffer, functor, 1);
}

TEST(LOOPED_PRODUCER_CREATION, FUNCTION_NO_PARAMETERS){
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::LoopedProducer<int, int(*)()> p(buffer, producer1, 100);
}

TEST(LOOPED_PRODUCER_CREATION, LAMBDA_NO_PARAMETERS){
    auto lambda = [](){return 1;};
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::LoopedProducer<int, decltype(lambda)> p(buffer, lambda, 100);
}

TEST(LOOPED_PRODUCER_CREATION, FUNCTOR_NO_PARAMETERS){
    FunctorProdNoParam functor;
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::LoopedProducer<int, FunctorProdNoParam> p(buffer, functor, 100);
}

TEST(LOOPED_PRODUCER_CREATION, FUNCTION_PARAMETRIZED){
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::LoopedProducer<int, int(*)(int), int> p(buffer,  producer2, 1, 100);
}

TEST(LOOPED_PRODUCER_CREATION, LAMBDA_PARAMETRIZED){
    auto lambda = [](int a){return a;};
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::LoopedProducer<int, decltype(lambda), int> p(buffer, lambda, 1, 100);
}

TEST(LOOPED_PRODUCER_CREATION, FUNCTOR_PARAMETRIZED){
    FunctorProdParam functor;
    auto buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::LoopedProducer<int, FunctorProdParam, int> p(buffer, functor, 1, 100);
}

TEST(CONSUMER_CREATION, FUNCTION_NO_PARAMETERS){
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::Consumer<int, void(*)(int)> c(buffer, consumer1);
}

TEST(CONSUMER_CREATION, LAMBDA_NO_PARAMETERS){
    auto lambda = [](int a){(void)(a);};
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::Consumer<int, decltype(lambda)> c(buffer, lambda);
}

TEST(CONSUMER_CREATION, FUNCTOR_NO_PARAMETERS){
    FunctorConsNoParam functor;
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::Consumer<int, FunctorConsNoParam> c(buffer, functor);
}

TEST(CONSUMER_CREATION, FUNCTION_PARAMETRIZED){
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::Consumer<int, void(*)(int, int), int> c(buffer, consumer2, 1);
}

TEST(CONSUMER_CREATION, LAMBDA_PARAMETRIZED){
    auto lambda = [](int a, int b){(void)(a); (void)(b);};
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::Consumer<int, decltype(lambda), int> c(buffer, lambda, 1);
}

TEST(CONSUMER_CREATION, FUNCTOR_PARAMETRIZED){
    FunctorConsParam functor;
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::Consumer<int, FunctorConsParam, int> c(buffer, functor, 1);
}

TEST(PBC_CREATION, SIMPLE){
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::PBCController<int> pbc(buffer);
}