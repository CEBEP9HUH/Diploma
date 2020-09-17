#include "test_helpers.hpp"


int producerFuncNoParam(){
    return 1;
}


int producerParametrisedFunc(int a){
    return a;
}


void consumerFuncNoParam(int a){
    (void)(a);
}


void consumerParametrisedFunc(int a, int b){
    (void)(a);
    (void)(b);
}