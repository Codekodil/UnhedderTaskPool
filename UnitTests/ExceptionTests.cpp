
#define CPP2_INCLUDE_STD         Yes

//=== Cpp2 type declarations ====================================================


#include "cpp2util.h"

#line 1 "ExceptionTests.cpp2"

#line 3 "ExceptionTests.cpp2"
class TestException;
 

//=== Cpp2 type definitions and function declarations ===========================

#line 1 "ExceptionTests.cpp2"
#include "Framework.h"

#line 3 "ExceptionTests.cpp2"
class TestException: public exception {
 public: TestException() = default;
 public: TestException(TestException const&) = delete; /* No 'that' constructor, suppress copy */
 public: auto operator=(TestException const&) -> void = delete;


#line 5 "ExceptionTests.cpp2"
};

auto BeforeAwait() -> void;
#line 17 "ExceptionTests.cpp2"
TEST(ExceptionTests, BeforeAwait)

auto AfterAwait() -> void;
#line 29 "ExceptionTests.cpp2"
TEST(ExceptionTests, AfterAwait)

auto AwaitRethrows() -> void;
#line 47 "ExceptionTests.cpp2"
TEST(ExceptionTests, AwaitRethrows)

auto RunException() -> void;
#line 56 "ExceptionTests.cpp2"
TEST(ExceptionTests, RunException)


//=== Cpp2 function definitions =================================================

#line 1 "ExceptionTests.cpp2"

#line 7 "ExceptionTests.cpp2"
auto BeforeAwait() -> void{
 TESTPOOL(2);

 auto task {[]() mutable -> Task<void>{
  THROW(TestException());
  AWAIT(Task<void>::Delay(1ms));
 }()}; 

 EXPECT_THROW(CPP2_UFCS_0(Join, std::move(task)), TestException);
}

#line 19 "ExceptionTests.cpp2"
auto AfterAwait() -> void{
 TESTPOOL(2);

 auto task {[]() mutable -> Task<void>{
  AWAIT(Task<void>::Delay(1ms));
  THROW(TestException());
 }()}; 

 EXPECT_THROW(CPP2_UFCS_0(Join, std::move(task)), TestException);
}

#line 31 "ExceptionTests.cpp2"
auto AwaitRethrows() -> void{
 TESTPOOL(2);

 auto throwingTask {[]() mutable -> Task<void>{
  AWAIT(Task<void>::Delay(1ms));
  THROW(TestException());
 }()}; 

 auto task {[_0 = std::move(throwingTask)]() mutable -> Task<void>{
  auto scopeTask {_0}; 
  AWAIT(Task<void>::Delay(1ms));
  AWAIT(std::move(scopeTask));
 }()}; 

 EXPECT_THROW(CPP2_UFCS_0(Join, std::move(task)), TestException);
}

#line 49 "ExceptionTests.cpp2"
auto RunException() -> void{
 TESTPOOL(2);

 auto task {Task<void>::Run([]() mutable -> void{THROW(TestException()); })}; 

 EXPECT_THROW(CPP2_UFCS_0(Join, std::move(task)), TestException);
}

