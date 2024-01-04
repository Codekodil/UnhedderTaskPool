
#define CPP2_INCLUDE_STD         Yes

//=== Cpp2 type declarations ====================================================


#include "cpp2util.h"

#line 1 "PoolManagementTests.cpp2"


//=== Cpp2 type definitions and function declarations ===========================

#line 1 "PoolManagementTests.cpp2"
#include "Framework.h"

#line 3 "PoolManagementTests.cpp2"
auto ContinueInPool() -> void;
#line 20 "PoolManagementTests.cpp2"
TEST(PoolManagementTests, ContinueInPool)

auto ContinueInPoolBreak() -> void;
#line 46 "PoolManagementTests.cpp2"
TEST(PoolManagementTests, ContinueInPoolBreak)

auto LinkedThread() -> void;
#line 65 "PoolManagementTests.cpp2"
TEST(PoolManagementTests, LinkedThread)


//=== Cpp2 function definitions =================================================

#line 1 "PoolManagementTests.cpp2"

#line 3 "PoolManagementTests.cpp2"
auto ContinueInPool() -> void{
 TESTPOOL(2);

 vector<thread::id> ids {}; 

 auto task {[_0 = (&ids)]() mutable -> Task<void>{
  auto idsPtr {_0}; 
  CPP2_UFCS(push_back, (*cpp2::assert_not_null(idsPtr)), this_thread::get_id());
  AWAIT(Task<void>::ContinueInPool());
  CPP2_UFCS(push_back, (*cpp2::assert_not_null(std::move(idsPtr))), this_thread::get_id());
 }()}; 

 CPP2_UFCS_0(Join, std::move(task));

 EXPECT_EQ(2, CPP2_UFCS_0(size, ids));
 EXPECT_NE(CPP2_ASSERT_IN_BOUNDS(ids, 0), CPP2_ASSERT_IN_BOUNDS(std::move(ids), 1));
}

#line 22 "PoolManagementTests.cpp2"
auto ContinueInPoolBreak() -> void{

 auto i {0}; 

 {
  TESTPOOL(2);

  Task<void> auto_29_3 {[_0 = (&i)]() mutable -> Task<void>{
   auto iPtr {_0}; 
   while( true ) {
    AWAIT(Task<void>::ContinueInPool());
    ++*cpp2::assert_not_null(iPtr);
   }
  }()}; 

  this_thread::sleep_for(100ms);
 }

 EXPECT_GT(i, 0);

 auto iAfterJoin {i}; 
 this_thread::sleep_for(100ms);
 EXPECT_EQ(std::move(i), std::move(iAfterJoin));
}

#line 48 "PoolManagementTests.cpp2"
auto LinkedThread() -> void{
 TESTPOOL(2);

 auto task {[]() mutable -> Task<int>{
  AWAIT(Task<void>::Delay(200ms));
  RETURN(1);
 }()}; 

 jthread auto_56_2 {ThreadLink::LinkedThread<jthread>([_0 = std::move(task)]() mutable -> void{
  auto taskCopy {_0}; 
  auto otherTask {[_0 = std::move(taskCopy)]() mutable -> Task<int>{
   auto result {AWAIT(_0)}; 
   RETURN(std::move(result) + 2);
  }()}; 
  EXPECT_EQ(3, CPP2_UFCS_0(Join, std::move(otherTask)));
 })}; 
}

