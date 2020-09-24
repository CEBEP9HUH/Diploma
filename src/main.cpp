#include "PBCController.hpp"


#include <random>


void foo(int a){
    std::cout << "consumer get " << a << "\n";
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
    PBCController<int> pci(std::make_shared<PriorityBuffer<int, std::greater<int> > >(10));
    pci.addProducers<InfiniteProducer<int, A, int > >(1, a, -100); 
    pci.addConsumers<Consumer<int, void(*)(int)> >(1, foo);
    pci.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(6));
    pci.stop();
    return 0;
}