
#define CPP2_INCLUDE_STD         Yes

//=== Cpp2 type declarations ====================================================


#include "cpp2util.h"

#line 1 "DelayTests.cpp2"

#line 3 "DelayTests.cpp2"
class ExpectingTimer;


//=== Cpp2 type definitions and function declarations ===========================

#line 1 "DelayTests.cpp2"
#include "Framework.h"

#line 3 "DelayTests.cpp2"
class ExpectingTimer {

 private: chrono::milliseconds _expected; 
 private: chrono::milliseconds _epsilon; 

 private: chrono::system_clock::time_point _start; 

 public: explicit ExpectingTimer(cpp2::in<chrono::milliseconds> expected, cpp2::in<chrono::milliseconds> epsilon);

#line 15 "DelayTests.cpp2"
 public: ~ExpectingTimer() noexcept;
 public: ExpectingTimer(ExpectingTimer const&) = delete; /* No 'that' constructor, suppress copy */
 public: auto operator=(ExpectingTimer const&) -> void = delete;


#line 22 "DelayTests.cpp2"
};

auto DelayJoin() -> void;
#line 37 "DelayTests.cpp2"
TEST(DelayTests, DelayJoin)

auto AwaitDelay() -> void;
#line 54 "DelayTests.cpp2"
TEST(DelayTests, AwaitDelay)

auto AfterAnother() -> void;
#line 72 "DelayTests.cpp2"
TEST(DelayTests, AfterAnother)

auto InParallel() -> void;
#line 91 "DelayTests.cpp2"
TEST(DelayTests, InParallel)


//=== Cpp2 function definitions =================================================

#line 1 "DelayTests.cpp2"

#line 10 "DelayTests.cpp2"
 ExpectingTimer::ExpectingTimer(cpp2::in<chrono::milliseconds> expected, cpp2::in<chrono::milliseconds> epsilon)
  : _expected{ expected }
  , _epsilon{ epsilon }
  , _start{ chrono::system_clock::now() }{

#line 14 "DelayTests.cpp2"
 }
 ExpectingTimer::~ExpectingTimer() noexcept{
  auto delta {chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - _start)}; 
  auto deviation {std::move(delta) - _expected}; 
  EXPECT_LE(deviation, _epsilon);
  EXPECT_GE(std::move(deviation), -_epsilon);
 }

#line 24 "DelayTests.cpp2"
auto DelayJoin() -> void{
 TESTPOOL(2);

 cpp2::deferred_init<Task<void>> task; 

 {
  ExpectingTimer auto_30_3 {0ms, 5ms}; 
  task.construct(Task<void>::Delay(100ms));
 }

 ExpectingTimer auto_34_2 {100ms, 20ms}; 
 CPP2_UFCS_0(Join, std::move(task.value()));
}

#line 39 "DelayTests.cpp2"
auto AwaitDelay() -> void{
 TESTPOOL(2);

 cpp2::deferred_init<Task<void>> task; 

 {
  ExpectingTimer auto_45_3 {0ms, 5ms}; 
  task.construct([]() mutable -> Task<void>{
   AWAIT(Task<void>::Delay(100ms));
  }());
 }

 ExpectingTimer auto_51_2 {100ms, 20ms}; 
 CPP2_UFCS_0(Join, std::move(task.value()));
}

#line 56 "DelayTests.cpp2"
auto AfterAnother() -> void{
 TESTPOOL(2);

 cpp2::deferred_init<Task<void>> task; 

 {
  ExpectingTimer auto_62_3 {0ms, 5ms}; 
  task.construct([]() mutable -> Task<void>{
   AWAIT(Task<void>::Delay(100ms));
   AWAIT(Task<void>::Delay(200ms));
  }());
 }

 ExpectingTimer auto_69_2 {300ms, 20ms}; 
 CPP2_UFCS_0(Join, std::move(task.value()));
}

#line 74 "DelayTests.cpp2"
auto InParallel() -> void{
 TESTPOOL(2);

 cpp2::deferred_init<Task<void>> task; 

 {
  ExpectingTimer auto_80_3 {0ms, 5ms}; 
  task.construct([]() mutable -> Task<void>{
   auto t100 {Task<void>::Delay(100ms)}; 
   AWAIT(Task<void>::Delay(200ms));
   AWAIT(std::move(t100));
  }());
 }

 ExpectingTimer auto_88_2 {200ms, 20ms}; 
 CPP2_UFCS_0(Join, std::move(task.value()));
}

