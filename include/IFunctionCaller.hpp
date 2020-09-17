#pragma once


namespace Diploma{
    class IFunctionCaller{
    public:
        virtual void run() = 0;
        virtual ~IFunctionCaller() = default;
    };

    class IProducerBase : public IFunctionCaller {};
    class IConsumerBase : public IFunctionCaller {};
}