
#define CPP2_INCLUDE_STD         Yes

//=== Cpp2 type declarations ====================================================


#include "cpp2util.h"

#line 1 "ValueTests.cpp2"


//=== Cpp2 type definitions and function declarations ===========================

#line 1 "ValueTests.cpp2"
#include "Framework.h"

#line 3 "ValueTests.cpp2"
auto ValueJoin() -> void;
#line 10 "ValueTests.cpp2"
TEST(ValueTests, ValueJoin)

auto RunResult() -> void;
#line 19 "ValueTests.cpp2"
TEST(ValueTests, RunResult)

auto AwaitValue() -> void;
#line 32 "ValueTests.cpp2"
TEST(ValueTests, AwaitValue)

auto MoveResult() -> void;
#line 46 "ValueTests.cpp2"
TEST(ValueTests, MoveResult)


//=== Cpp2 function definitions =================================================

#line 1 "ValueTests.cpp2"

#line 3 "ValueTests.cpp2"
auto ValueJoin() -> void{
 TESTPOOL(2);

 auto task {Task<int>::FromResult(42)}; 

 EXPECT_EQ(42, CPP2_UFCS_0(Join, std::move(task)));
}

#line 12 "ValueTests.cpp2"
auto RunResult() -> void{
 TESTPOOL(2);

 auto task {Task<int>::Run([]() mutable -> int { return 42;  })}; 

 EXPECT_EQ(42, CPP2_UFCS_0(Join, std::move(task)));
}

#line 21 "ValueTests.cpp2"
auto AwaitValue() -> void{
 TESTPOOL(2);

 auto task {[]() mutable -> Task<int>{
  auto n1 {AWAIT(Task<int>::FromResult(20))}; 
  auto n2 {AWAIT(Task<int>::Run([]() mutable -> int { return 11;  }))}; 
  RETURN(std::move(n1) + std::move(n2) * 2);
 }()}; 

 EXPECT_EQ(42, CPP2_UFCS_0(Join, std::move(task)));
}

#line 34 "ValueTests.cpp2"
auto MoveResult() -> void{
 TESTPOOL(2);

 auto task {[]() mutable -> Task<unique_ptr<int>>{
  auto n1 {AWAIT(Task<unique_ptr<int>>::FromResult(cpp2_new<int>(20)))}; 
  auto n2 {AWAIT(Task<unique_ptr<int>>::Run([]() mutable -> unique_ptr<int> { return cpp2_new<int>(11);  }))}; 
  auto n {cpp2_new<int>(*cpp2::assert_not_null(std::move(n1)) + *cpp2::assert_not_null(std::move(n2)) * 2)}; 
  RETURN(std::move(n));
 }()}; 

 EXPECT_EQ(42, *cpp2::assert_not_null(CPP2_UFCS_0(Join, std::move(task))));
}

