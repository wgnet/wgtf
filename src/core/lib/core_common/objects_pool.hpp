#ifndef OBJECTS_POOL_HPP
#define OBJECTS_POOL_HPP

#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <cassert>

class SingleThreadStrategy
{
public:
    SingleThreadStrategy();

    void lock();
    void unlock();

private:
    std::thread::id threadAffinity;
};

class MultiThreadStrategy
{
public:
    void lock();
    void unlock();

private:
    std::mutex mutex;
};

template <typename T, typename TLockStrategy>
class ObjectsPool
{
public:
    ObjectsPool(size_t batchSize, size_t initialBatchCount = 1);

    std::shared_ptr<T> requestObject();

private:
    friend class ObjectsPoolTest;
    static const size_t INVALID_INDEX;
    struct ObjectNode
    {
        size_t nodeGeneration;
        size_t nextIndex;
        T object;
    };

    struct PoolNode
    {
        PoolNode()
            : batchStart(nullptr)
            , batchHead(nullptr)
        {
        }

        ~PoolNode()
        {
            delete[] batchStart;
        }

        ObjectNode* batchStart;
        ObjectNode* batchHead;
    };

    struct LockGuard
    {
        LockGuard(TLockStrategy& lockStrategy_)
            : lockStrategy(lockStrategy_)
        {
            lockStrategy.lock();
        }
        ~LockGuard()
        {
            lockStrategy.unlock();
        }

        TLockStrategy& lockStrategy;
    };

    void releaseObject(T* object);
    bool isInPool(T * object); 
    PoolNode* allocateNewBatch();
    
    const size_t batchSize;
    std::vector<PoolNode> objectBatches;
    TLockStrategy lockStrategy;
};

SingleThreadStrategy::SingleThreadStrategy()
    : threadAffinity(std::this_thread::get_id())
{
}

void SingleThreadStrategy::lock()
{
    assert(threadAffinity == std::this_thread::get_id());
}

void SingleThreadStrategy::unlock()
{
    assert(threadAffinity == std::this_thread::get_id());
}

void MultiThreadStrategy::lock()
{
    mutex.lock();
}

void MultiThreadStrategy::unlock()
{
    mutex.unlock();
}

template <typename T, typename TLockStrategy>
const size_t ObjectsPool<T, TLockStrategy>::INVALID_INDEX = static_cast<size_t>(-1);

template <typename T, typename TLockStrategy>
ObjectsPool<T, TLockStrategy>::ObjectsPool(size_t batchSize_, size_t initialBatchCount)
    : batchSize(batchSize_)
{
    assert(initialBatchCount > 0);
    objectBatches.reserve(initialBatchCount);
}

template <typename T, typename TLockStrategy>
std::shared_ptr<T> ObjectsPool<T, TLockStrategy>::requestObject()
{
    LockGuard guard(lockStrategy);
    PoolNode* poolNode = nullptr;
    for (PoolNode& node : objectBatches)
    {
        if (node.batchHead != nullptr)
        {
            poolNode = &node;
            break;
        }
    }

    if (poolNode == nullptr)
    {
        poolNode = allocateNewBatch();
    }

    ObjectNode* result = poolNode->batchHead;
    ObjectNode* nextObject = nullptr;
    if (result->nextIndex != INVALID_INDEX)
    {
        assert(result->nextIndex < batchSize);
        nextObject = poolNode->batchStart + result->nextIndex;
    }
    poolNode->batchHead = nextObject;

    return std::shared_ptr<T>(&result->object, std::bind(&ObjectsPool<T, TLockStrategy>::releaseObject, this, std::placeholders::_1));
}

template <typename T, typename TLockStrategy>
void ObjectsPool<T, TLockStrategy>::releaseObject(T * object)
{
    LockGuard guard(lockStrategy);
    assert(isInPool(object));
    ObjectNode* objectNode = reinterpret_cast<ObjectNode*>(reinterpret_cast<uint8_t*>(object) - 2 * sizeof(size_t));
    size_t generation = objectNode->nodeGeneration;
    assert(generation < objectBatches.size());

    PoolNode& poolNode = objectBatches[generation];
    if (poolNode.batchHead == nullptr)
    {
        objectNode->nextIndex = INVALID_INDEX;
        poolNode.batchHead = objectNode;
    }
    else
    {
        size_t headIndex = poolNode.batchHead - poolNode.batchStart;
        objectNode->nextIndex = headIndex;
        poolNode.batchHead = objectNode;
    }
}

template <typename T, typename TLockStrategy>
typename ObjectsPool<T, TLockStrategy>::PoolNode* ObjectsPool<T, TLockStrategy>::allocateNewBatch()
{
    size_t generation = objectBatches.size();

    objectBatches.push_back(PoolNode());
    PoolNode& poolNode = objectBatches.back();
    poolNode.batchStart = new ObjectNode[batchSize];
    poolNode.batchHead = poolNode.batchStart + (batchSize - 1);
    for (size_t i = 0; i < batchSize; ++i)
    {
        poolNode.batchStart[i].nodeGeneration = generation;
        poolNode.batchStart[i].nextIndex = i - 1;
    }

    return &poolNode;
}

template <typename T, typename TLockStrategy>
bool ObjectsPool<T, TLockStrategy>::isInPool(T * object)
{
    uint8_t* rawObjectPointer = reinterpret_cast<uint8_t*>(object);
    for (const PoolNode& node : objectBatches)
    {
        uint8_t* endNode = reinterpret_cast<uint8_t*>(node.batchStart + batchSize);
        std::ptrdiff_t diffFromStart = rawObjectPointer - reinterpret_cast<uint8_t*>(node.batchStart);
        std::ptrdiff_t diffFromEnd = endNode - rawObjectPointer;

        if (diffFromStart > 0 && diffFromEnd > 0)
        {
            return true;
        }
    }

    return false;
}

#endif