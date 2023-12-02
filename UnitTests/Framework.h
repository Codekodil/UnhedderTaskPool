
#define GTEST_DONT_DEFINE_TEST 1
#include "gtest/gtest.h"
#define TEST(TestCaseName,TestName)GTEST_TEST(TestCaseName,TestName){TestName();}
#include "..\include\TaskPool.h"
using namespace AsyncTask;
using namespace std;
#define TESTPOOL(workers)auto pool=make_shared<TaskPool>(workers);const ThreadLink link(pool)
#define AWAIT(expr) co_await expr
#define RETURN(expr) co_return expr
