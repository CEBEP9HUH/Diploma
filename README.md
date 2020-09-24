# Diploma project for OTUS C++ Developer course
## Producer-Consumer
### Review
It is a C++ realisation of producer-consumer pattern. 
### Features
- Provides several types of producers:
-- Infinite producer - produce data until program end or user's command to stop producer-consumer process
-- Looped producer - produce data finite number of times
-- Predicated producer - produce data until condition will become true
Every producer type takes a type of data to produce, a function-producer signature and types function parameters as a template parameters. Predicated producer's function have to be inherited from Predicate class.
 - Consumers class. It takes a type of data to consume, a function-consumer signature and types function parameters as a template parameters. Consumer be in progress until program end or user's command to stop producer-consumer process.
- Provides several types of buffers:
 -- Buffer is a shared queue with data between all producers and consumers. Producers puts data in it and consumers gets data from it.
 -- Priority buffer is a shared priority queue with data between all producers and consumers. Producers puts data in it. Consumers gets data from it by order defined by user.
 - Producer-Buffer-Consumer controller. Provides tools to add or remove producers and consumers. Different producers and consumers can be runned with mutual buffer at the same time (also user defined).
### Example
In example below we create PBCController. We pass a shared pointer at priority buffer as a constructor parameter. Buffer's constructors takes integral value - size of buffer.
On next line we add producer at PBCController. There is several parameters: count of producers to create, function-producer, function-producer's parameters.
On next line we add consumer at PBCController. There is also several parameters: count of consumers to create, function-consumer, function-consumer's parameters (we no need one in this example).
After it our producer-consumer is ready to start. We run it for a 50 ms and than we stop it.
```
void bar(int a){
    std::cout << "consumer get " << a << "\n";
}

class Foo{
public:
    int operator()(int c){
        std::random_device rd;
        int a = c+rd() % 10;
        std::cout << "producer generate " << a << "\n";
        return a;
    }
};

int main(){
    using namespace Diploma;
    Foo a;
    PBCController<int> pci(std::make_shared<PriorityBuffer<int, std::greater<int> > >(10));
    pci.addProducers<InfiniteProducer<int, Foo, int > >(1, a, -100); 
    pci.addConsumers<Consumer<int, void(*)(int)> >(1, bar);
    pci.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    pci.stop();
    return 0;
}
```
There is program output:
```
 producer generate -43
 producer generate -11
 producer generate -12
 producer generate -84
 consumer get -43
 consumer get -12
 consumer get -11
```

As you can see there is -84 was generated but consumer's first value is -43. This is happens because producers generate data before they lock shared buffer. So, producer was generate value but it wasn't be placed into buffer.

### TODOs
 - To add possibility to save buffer on disk and load it
 - To add possibility to interract with producers and consimers via network

### License
MIT
