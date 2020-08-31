#include "ProducerConsumer.hpp"
#include "Consumer.hpp"
#include "Producer.hpp"


#include <random>


int main(){
    auto f = [](){
        std::random_device rd;
        int a = rd() % 500;
        std::cout << "Generated: " << a << "\n";
        return a;
    };
    Diploma::ProducerConsumer<Diploma::Producer<decltype(f), std::tuple<> >, Diploma::Consumer<decltype(f)>, 50> pci(4, 2, f);
    pci.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    pci.stop();
    return 0;
}