#pragma once


#include "PBCController.hpp"


int producerFuncNoParam(){
    return 1;
}

int producerParametrisedFunc(int a){
    return a;
}

class ProducerFuncNoParam{
    public:
        int operator()(){
            return 1;
        }
};

class ProduserParametrisedFunc{
    public:
        int operator()(int a){
            return a;
        }
};

class ProducerPredicatedFunc : public Diploma::Predicate {
    public:
        int operator()(){
            return 1;
        }
        virtual bool stopProducer() override {
            return true;
        }
};

void consumerFuncNoParam(int a){
    (void)(a);
}

void consumerParametrisedFunc(int a, int b){
    (void)(a);
    (void)(b);
}

class ConsumerFuncNoParam{
    public:
        void operator()(int a){
            (void)(a);
        }
};

class ConsumerParametrisedFunc{
    public:
        void operator()(int a, int b){
            (void)(a);
            (void)(b);
        }
};