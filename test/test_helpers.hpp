#pragma once


#include <string_view>
#include <fstream>


#include "PBCController.hpp"


int producerFuncNoParam();
int producerParametrisedFunc(int a);
void consumerFuncNoParam(int a);
void consumerParametrisedFunc(int a, int b);


class ProducerFuncNoParam{
    public:
        int operator()(){
            return 1;
        }
};


class ProduserParametrisedFunc{
    public:
        int operator()(int a){
            return a;
        }
};


class ProducerPredicatedFunc : public Diploma::Predicate {
    public:
        int operator()(){
            return 1;
        }
        virtual bool stopProducer() override {
            return true;
        }
};


class ConsumerFuncNoParam{
    public:
        void operator()(int a){
            (void)(a);
        }
};


class ConsumerParametrisedFunc{
    public:
        void operator()(int a, int b){
            (void)(a);
            (void)(b);
        }
};


class FileReader : public Diploma::Predicate{
private:
    std::vector<std::string> _text;
    size_t _line = 0;
public:
    FileReader() = delete;

    FileReader(std::string name){
        std::ifstream _in;
        _in.open(name, std::ios::in);
        std::string tmp;
        while(std::getline(_in, tmp)){
            _text.emplace_back(std::move(tmp));
        }
        _in.close();
    }

    ~FileReader() {
    }
                                                
    virtual bool stopProducer() override {
        return _line == _text.size();
    }

    std::string operator()(){
        return _text[_line++];
    }
};


class StringHandler{
public:    
    void operator()(std::string_view text){
        std::cout << text << '\n';
    }
};