
#define CPP2_INCLUDE_STD         Yes

//=== Cpp2 type declarations ====================================================


#include "cpp2util.h"

#line 1 "CompletionSourceTests.cpp2"


//=== Cpp2 type definitions and function declarations ===========================

#line 1 "CompletionSourceTests.cpp2"
#include "Framework.h"

#line 3 "CompletionSourceTests.cpp2"
auto SetVoid() -> void;
#line 28 "CompletionSourceTests.cpp2"
TEST(CompletionSourceTests, SetVoid)

auto SetValue() -> void;
#line 56 "CompletionSourceTests.cpp2"
TEST(CompletionSourceTests, SetValue)

auto SourceThrows() -> void;
#line 74 "CompletionSourceTests.cpp2"
TEST(CompletionSourceTests, SourceThrows)

auto SetMoveValue() -> void;
#line 96 "CompletionSourceTests.cpp2"
TEST(CompletionSourceTests, SetMoveValue)


//=== Cpp2 function definitions =================================================

#line 1 "CompletionSourceTests.cpp2"

#line 3 "CompletionSourceTests.cpp2"
auto SetVoid() -> void{
 TESTPOOL(2);

 string order {""}; 

 TaskCompletionSource<void> source {}; 

 auto sourceTask {CPP2_UFCS_0(GetTask, source)}; 

 auto task {[_0 = std::move(sourceTask), _1 = (&order)]() mutable -> Task<void>{
  auto task {_0}; 
  *cpp2::assert_not_null(_1) += "1";
  AWAIT(std::move(task));
  *cpp2::assert_not_null(_1) += "3";
 }()}; 

 order += "2";

 EXPECT_TRUE(CPP2_UFCS_0(SetResult, source));
 EXPECT_FALSE(CPP2_UFCS_0(SetResult, std::move(source)));

 CPP2_UFCS_0(Join, std::move(task));

 EXPECT_EQ(string("123"), std::move(order));
}

#line 30 "CompletionSourceTests.cpp2"
auto SetValue() -> void{
 TESTPOOL(2);

 string order {""}; 

 TaskCompletionSource<int> source {}; 

 auto sourceTask {CPP2_UFCS_0(GetTask, source)}; 

 auto task {[_0 = std::move(sourceTask), _1 = (&order)]() mutable -> Task<int>{
  auto task {_0}; 
  *cpp2::assert_not_null(_1) += "1";
  auto result {AWAIT(std::move(task))}; 
  *cpp2::assert_not_null(_1) += "3";
  RETURN(std::move(result));
 }()}; 

 order += "2";

 EXPECT_TRUE(CPP2_UFCS(SetResult, source, 42));
 EXPECT_FALSE(CPP2_UFCS(SetResult, std::move(source), 43));

 EXPECT_EQ(42, CPP2_UFCS_0(Join, std::move(task)));

 EXPECT_EQ(string("123"), std::move(order));
}

#line 58 "CompletionSourceTests.cpp2"
auto SourceThrows() -> void{
 TESTPOOL(2);

 auto source {cpp2_new<TaskCompletionSource<void>>()}; 

 auto sourceTask {CPP2_UFCS_0(GetTask, (*cpp2::assert_not_null(source)))}; 

 auto task {[_0 = std::move(sourceTask)]() mutable -> Task<void>{
  auto task {_0}; 
  AWAIT(std::move(task));
 }()}; 

 source = nullptr;

 EXPECT_THROW(CPP2_UFCS_0(Join, std::move(task)), AsyncTask::UnfinishedTaskException);
}

#line 76 "CompletionSourceTests.cpp2"
auto SetMoveValue() -> void{
 TESTPOOL(2);

 TaskCompletionSource<unique_ptr<int>> source {}; 

 auto sourceTask {CPP2_UFCS_0(GetTask, source)}; 

 auto task {[_0 = std::move(sourceTask)]() mutable -> Task<int>{
  auto task {_0}; 
  auto result {*cpp2::assert_not_null(AWAIT(std::move(task)))}; 
  RETURN(std::move(result));
 }()}; 

 auto result {cpp2_new<int>(42)}; 

 EXPECT_TRUE(CPP2_UFCS(SetResult, source, move(std::move(result))));
 EXPECT_FALSE(CPP2_UFCS(SetResult, std::move(source), nullptr));

 EXPECT_EQ(42, CPP2_UFCS_0(Join, std::move(task)));
}

