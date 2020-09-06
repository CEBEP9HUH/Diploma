#include <benchmark/benchmark.h>
#include <random>


#include "PBCController.hpp"


void foo(int a){
    std::cout << a << "\n";
}

class A{
private:
    int _i = 0;
public:
    int operator()(){
        std::cout << _i << "\n";
        return ++_i;
    }
};


static void BM_ProducerConsumerTesting(benchmark::State& state){
    A a;
    std::shared_ptr<Diploma::Buffer<int> > buffer = std::make_shared<Diploma::Buffer<int> >(1000);
    Diploma::PBCController<int> pbc(buffer);
    pbc.addProducers<Diploma::LoopedProducer<int, A> >(20, a, 1000);
    pbc.addConsumers<Diploma::Consumer<int, void(*)(int)> >(20, foo);
    for (auto _ : state){
        pbc.run();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        pbc.stop();
    }
}
BENCHMARK(BM_ProducerConsumerTesting)->Unit(benchmark::kMillisecond)
                                    ->Iterations(1);


/* static void BM_ClearTesting(benchmark::State& state){
    A a;
    for (auto _ : state){
        for(size_t i =0; i<1000; ++i){
            auto q = a();
            foo(q);
        }
    }
}
BENCHMARK(BM_ClearTesting)->Unit(benchmark::kMillisecond)
                                ->Iterations(1); */

BENCHMARK_MAIN();