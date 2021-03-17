#include <benchmark/benchmark.h>
#include <random>
#include <filesystem>


#include "PBCController.hpp"
#include "../test/test_helpers.hpp"


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




static void BM_StringHandling(benchmark::State& state){
    auto buffer = std::make_shared<Diploma::Buffer<std::string> >(3);
    Diploma::PBCController<std::string> pbc(buffer);

    using namespace std;
    filesystem::path p1 = filesystem::current_path().parent_path();
    p1 /= "performance";
    filesystem::path p2 = p1;
    p1 /= "test_text_1.txt";
    p2 /= "test_text_2.txt";

    FileReader reader_1(p1.u8string());
    FileReader reader_2(p2.u8string());
    StringHandler handler;
    pbc.addProducers<Diploma::PredicatedProducer<std::string, FileReader> >(5, reader_1);
    pbc.addProducers<Diploma::PredicatedProducer<std::string, FileReader> >(5, reader_2);
    pbc.addConsumers<Diploma::Consumer<std::string, StringHandler> >(10, handler);
    for (auto _ : state){
        pbc.run();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        pbc.stop();
    }
}
BENCHMARK(BM_StringHandling)->Unit(benchmark::kMillisecond)
                                ->Iterations(5);

BENCHMARK_MAIN();