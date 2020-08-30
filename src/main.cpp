#include "ProducerConsumer.hpp"
#include "Consumer.hpp"
#include "Producer.hpp"


#include <random>


int main(){
    auto f = [](){
        std::random_device rd;
        int a = rd() % 100;
        std::cout << "Generated: " << a << "\n";
        return a;
    };
    Diploma::ProducerConsumer<int, Diploma::Producer<int, decltype(f)>, Diploma::Consumer<int>, 50, decltype(f)> pci(4, 2, f);
    pci.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    pci.stop();
    return 0;
}