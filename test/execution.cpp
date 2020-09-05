#include <gtest/gtest.h>
#include <memory>
#include <mutex>


#include "PBCController.hpp"


int producer(){
    return 1;
}

void consumer(int a){
    (void)(a);
}


TEST(EXECUTION, INFINITE_PRODUCER){
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(50);
    Diploma::PBCController<int> pbc(buffer);
    pbc.addProducers<Diploma::InfiniteProducer<int, int(*)()> >(4, producer);
    pbc.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    pbc.stop();
    while(!buffer->isEmpty()){
        ASSERT_EQ(buffer->get(), producer());
    }
}