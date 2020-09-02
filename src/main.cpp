#include "ProducerConsumer.hpp"
#include "Consumer.hpp"
#include "Producer.hpp"


#include <random>

void foo(int a){
    std::cout << "1st group's consumer get " << a << "\n";
}

void bar(int a){
    std::cout << "2nd group's consumer get " << a << "\n";
}

class A{
    std::string _i;
public:
    A() = delete;
    A(const std::string& i): _i{i}{}
    A(const A& other){
        _i = other._i;
    }
    A& operator=(const A& other){
        _i = other._i;
        return *this;
    }
    int operator()(int c){
        std::random_device rd;
        int a = rd() % 10;
        std::cout << _i << " add " << c << " to " << a << " and get " << c+a << "\n";
        return c+a;
    }
};


int main(){
    A a("Pupa"), b("Lupa");
    Diploma::ProducerConsumer<Diploma::Producer<decltype(a), std::tuple<int> >, 
                                Diploma::Consumer<void(*)(int), int, std::tuple<> >, 
                                100> pci(2, a, std::make_tuple<int>(-10), 2, foo, std::make_tuple<>());
    pci.addProducers(4, b, std::tuple<int>(-100));
    pci.addConsumers(4, bar, std::tuple<>());
    pci.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    pci.stop();
    return 0;
}