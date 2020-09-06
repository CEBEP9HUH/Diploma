#pragma once
/* 
    Buffer<buffer_t> is a shared memory place which can get values 
    from producer and provides them to consumer.
    PriorityBuffer<buffer_t, comparator> is a shared memory place which can 
    get values from producer and provides them to consumer. 
    Return values by priority. Priority is sets by comparator
    buffer_t - is a type of storing data
*/

#include <queue>
#include <mutex>
#include <condition_variable>

#include <iostream>
namespace Diploma{
    struct accessSync{
        std::mutex _buffer_mutex;
        std::condition_variable _conditionVar;
        bool _exitThread = false;
    };
    template<typename buffer_t>
    class BufferBase{
    protected:
        static_assert(!std::is_same<buffer_t, void>::value, "return type of buffer cannot be void");
        size_t _maxSize;
        accessSync _sync;
    public:
        BufferBase(const size_t size) : _maxSize{size} {}
        virtual ~BufferBase() = default;

        // insert new element into buffer if it possible
        // return true if insertion was succeed
        virtual bool insert(const buffer_t& val) = 0;

        // get next value of buffer and remove it from buffer
        // doesn't check if buffer is empty
        virtual buffer_t get() = 0;

        virtual bool isFull() = 0;
        virtual bool isEmpty() = 0;
        accessSync& getSync() {
            return _sync;
        }
    };
    

    template<typename buffer_t>
    class Buffer : public BufferBase<buffer_t> {
    private:
        std::queue<buffer_t> _buffer;
    public:
        Buffer(const Buffer&) = delete;
        Buffer(Buffer&&) = delete;
        Buffer& operator=(const Buffer&) = delete;
        Buffer& operator=(Buffer&&) = delete;
        Buffer() : BufferBase<buffer_t>{1} {}
        Buffer(size_t size) : BufferBase<buffer_t>{size} {}
        virtual ~Buffer() = default;

        virtual bool isFull() override { 
            return _buffer.size() == this->_maxSize;
        }

        virtual bool isEmpty() override {
            return _buffer.empty();
        }
        virtual bool insert(const buffer_t& val) override {
            if(!isFull()){
                _buffer.push(val);
                return true;
            }
            return false;
        }
        virtual buffer_t get() override {
            auto val = _buffer.front();
            _buffer.pop();
            return val;
        }
    };

    template<typename buffer_t, typename comparator = std::less<buffer_t> >
    class PriorityBuffer : public BufferBase<buffer_t> {
    private:
        std::priority_queue<buffer_t, std::vector<buffer_t>, comparator> _buffer;
    public:
        PriorityBuffer(const PriorityBuffer&) = delete;
        PriorityBuffer(PriorityBuffer&&) = delete;
        PriorityBuffer& operator=(const PriorityBuffer&) = delete;
        PriorityBuffer& operator=(PriorityBuffer&&) = delete;
        PriorityBuffer() : BufferBase<buffer_t>{1} {}
        PriorityBuffer(size_t size) : BufferBase<buffer_t>{size} {}
        virtual ~PriorityBuffer() = default;

        virtual bool isFull() override { 
            return _buffer.size() == this->_maxSize;
        }

        virtual bool isEmpty() override {
            return _buffer.empty();
        }
        virtual bool insert(const buffer_t& val) override {
            if(!isFull()){
                _buffer.push(val);
                return true;
            }
            return false;
        }
        virtual buffer_t get() override {
            auto val = _buffer.top();
            _buffer.pop();
            return val;
        }
    };
}