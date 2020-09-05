#include "PBCController.hpp"


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

class A1{
    std::string _i;
public:
    A1() = delete;
    A1(const std::string& i): _i{i}{}
    A1(const A1& other){
        _i = other._i;
    }
    A1& operator=(const A1& other){
        _i = other._i;
        return *this;
    }
    std::pair<int, bool> operator()(int c){
        std::random_device rd;
        int a = rd() % 10;
        std::cout << _i << " add " << c << " to " << a << " and get " << c+a << "\n";
        return std::make_pair(c+a, true);
    }
};


/* 
    TODO
    add tests
    add performance tests
 */


int main(){
    using namespace Diploma;
    A a("Pupa"), b("Lupa");
    PBCController<int> pci(std::make_shared<PriorityBuffer<int, std::greater<int> > >(100));
    pci.addProducers<InfiniteProducer<int, A, std::tuple<int> > >(4, b, std::tuple<int>(-100)); 
    pci.addConsumers<Consumer<int, void(*)(int), std::tuple<> > >(4, bar, std::tuple<>());
    pci.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    pci.stop();
    
/*     A c("Maushi");
    pci.addProducers<LoopedProducer<int, A, std::tuple<int> > >(2, c, std::tuple<int>(-0), 2);
    pci.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    pci.stop(); */
    return 0;
}