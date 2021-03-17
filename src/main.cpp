#include "PBCController.hpp"
#include "defines.hpp"


#include <random>


void foo(int a){
    std::cout << "\tconsumer get " << a << "\n";
}

class A{
public:
    int operator()(int c){
        std::random_device rd;
        int a = c+rd() % 100;
        std::cout << "producer put " << a << "\n";
        return a;
    }
};


int main(){
    using namespace Diploma;
    A a;
    PBCController<int> pci(std::make_shared<PriorityBuffer<int, std::greater<int> > >(BUFFER_SIZE));
    pci.addProducers<InfiniteProducer<int, A, int > >(PRODUCERS_COUNT, a, -100); 
    pci.addConsumers<Consumer<int, void(*)(int)> >(CONSUMERS_COUNT, foo);
    pci.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(WORK_TIME_MS));
    pci.stop();
    return 0;
}