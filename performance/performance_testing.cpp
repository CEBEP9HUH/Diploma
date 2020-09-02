#include <benchmark/benchmark.h>
#include <random>


#include "ProducerConsumer.hpp"

/* 
void foo([[maybe_unused]]int a){
    //std::cout << "10 + " << a << " = " << 10+a << "\n";
}

class A{
public:
    int operator()(int c){
        std::random_device rd;
        int a = c+rd() % 10;
        //std::cout << "Random: " << a << "\n";
        return a;
    }
};


static void BM_ProducerConsumerTesting(benchmark::State& state){
    A a;
    Diploma::ProducerConsumer<Diploma::Producer<decltype(a), std::tuple<int> >, 
                                Diploma::Consumer<void(*)(int), int, std::tuple<> >, 
                                50> pci(40, 2, a, std::make_tuple<int>(-10), foo, std::make_tuple<>()); 
    for (auto _ : state){
        pci.run();
        std::this_thread::sleep_for(std::chrono::minutes(20));
        pci.stop();
    }
}

BENCHMARK(BM_ProducerConsumerTesting)->Unit(benchmark::kMillisecond)
                                    ->Iterations(1); */

BENCHMARK_MAIN();