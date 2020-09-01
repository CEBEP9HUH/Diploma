#include "ProducerConsumer.hpp"
#include "Consumer.hpp"
#include "Producer.hpp"


#include <random>

void foo(int a){
    std::cout << "10 + " << a << " = " << 10+a << "\n";
}

class A{
public:
    int operator()(int c){
        std::random_device rd;
        int a = c+rd() % 10;
        std::cout << "Random: " << a << "\n";
        return a;
    }
};


int main(){
    A a;
    Diploma::ProducerConsumer<Diploma::Producer<decltype(a), std::tuple<int> >, 
                                Diploma::Consumer<void(*)(int), int, std::tuple<> >, 
                                50> pci(4, 2, a, std::make_tuple<int>(-10), foo, std::make_tuple<>());
    pci.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    pci.stop();
    return 0;
}