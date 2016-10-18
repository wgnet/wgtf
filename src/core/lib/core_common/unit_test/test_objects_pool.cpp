#include "third_party/CppUnitLite2/src/CppUnitLite2.h"
#include "third_party/CppUnitLite2/src/Test.h"
#include "core_common/objects_pool.hpp"

#include <memory>
#include <set>
#include <vector>


namespace TestObjectsPoolDetails
{
struct TestObject
{
    size_t value;
};
}

class ObjectsPoolTest
{
public:
    ObjectsPoolTest()
        : m_name("ObjectsPoolTest")
        , m_requestCount(30)
    {
    }

    void run(TestResult& result_)
    {
        using namespace TestObjectsPoolDetails;
        typedef ObjectsPool<TestObject, SingleThreadStrategy> TPool;

        const size_t iterationStep = 4;
        const size_t requestCount = m_requestCount + m_requestCount % iterationStep;
        const size_t poolSize = 100;

        ObjectsPool<TestObject, SingleThreadStrategy> pool(poolSize, 1);

        std::vector<std::shared_ptr<TestObject>> requested;
        
        for (size_t i = 0; i < requestCount; ++i)
        {
            requested.push_back(pool.requestObject());
            requested.back()->value = i;
        }

        for (size_t i = 0; i < requested.size(); i += iterationStep)
        {
            requested[i].reset();
            requested[i + 3].reset();
            requested[i + 2].reset();
            requested[i + 1].reset();
        }

        TPool::PoolNode& node = pool.objectBatches[0];
        TPool::ObjectNode* start = node.batchStart;
        TPool::ObjectNode* head = node.batchHead;

        for (size_t i = 0; i < requestCount; i += iterationStep)
        {
            size_t baseValue = requestCount - i;
            //requested[i + 1].reset();
            size_t headIndex = head - start;
            CHECK_EQUAL(head->nextIndex, headIndex - 1);
            CHECK_EQUAL(head->object.value, baseValue - 3);

            //requested[i + 2].reset();
            headIndex = head->nextIndex;
            head = start + headIndex;
            CHECK_EQUAL(head->nextIndex, headIndex - 1);
            CHECK_EQUAL(head->object.value, baseValue - 2);

            //requested[i + 3].reset();
            headIndex = head->nextIndex;
            head = start + headIndex;
            CHECK_EQUAL(head->nextIndex, headIndex + 3);
            CHECK_EQUAL(head->object.value, baseValue - 1);

            //requested[i].reset();
            headIndex = head->nextIndex;
            head = start + headIndex;
            if (i + iterationStep < requestCount)
            {
                CHECK_EQUAL(head->nextIndex, headIndex + 3);
                CHECK_EQUAL(head->object.value, baseValue - 4);
            }
            else
            {
                CHECK_EQUAL(head->nextIndex, poolSize - requestCount - 1);
            }

            headIndex = head->nextIndex;
            head = start + headIndex;
        }

        head = node.batchHead;
        size_t sum = 0;
        do
        {
            sum += (head - start);
            head = start + head->nextIndex;
        } while (head->nextIndex != TPool::INVALID_INDEX);

        size_t expectedSum = (poolSize - 1) * poolSize / 2;
        CHECK_EQUAL(sum, expectedSum);
    }

private:
     const char * m_name;
     const size_t m_requestCount;
};

TEST(objects_pool)
{
    ObjectsPoolTest().run(result_);
}