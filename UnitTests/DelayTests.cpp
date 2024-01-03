
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


#line 21 "DelayTests.cpp2"
};

auto DelayJoin() -> void;
#line 36 "DelayTests.cpp2"
TEST(DelayTests, DelayJoin)

auto AwaitDelay() -> void;
#line 53 "DelayTests.cpp2"
TEST(DelayTests, AwaitDelay)

auto AfterAnother() -> void;
#line 71 "DelayTests.cpp2"
TEST(DelayTests, AfterAnother)

auto InParallel() -> void;
#line 90 "DelayTests.cpp2"
TEST(DelayTests, InParallel)

auto ContinueInPool() -> void;
#line 112 "DelayTests.cpp2"
TEST(DelayTests, ContinueInPool)


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
  EXPECT_TRUE(([_0 = -_epsilon, _1 = std::move(deviation), _2 = _epsilon]{ return cpp2::cmp_less_eq(_0,_1) && cpp2::cmp_less_eq(_1,_2); }()));
 }

#line 23 "DelayTests.cpp2"
auto DelayJoin() -> void{
 TESTPOOL(2);

 cpp2::deferred_init<Task<void>> task; 

 {
  ExpectingTimer auto_29_3 {0ms, 5ms}; 
  task.construct(Task<void>::Delay(100ms));
 }

 ExpectingTimer auto_33_2 {100ms, 20ms}; 
 CPP2_UFCS_0(Join, std::move(task.value()));
}

#line 38 "DelayTests.cpp2"
auto AwaitDelay() -> void{
 TESTPOOL(2);

 cpp2::deferred_init<Task<void>> task; 

 {
  ExpectingTimer auto_44_3 {0ms, 5ms}; 
  task.construct([]() mutable -> Task<void>{
   AWAIT(Task<void>::Delay(100ms));
  }());
 }

 ExpectingTimer auto_50_2 {100ms, 20ms}; 
 CPP2_UFCS_0(Join, std::move(task.value()));
}

#line 55 "DelayTests.cpp2"
auto AfterAnother() -> void{
 TESTPOOL(2);

 cpp2::deferred_init<Task<void>> task; 

 {
  ExpectingTimer auto_61_3 {0ms, 5ms}; 
  task.construct([]() mutable -> Task<void>{
   AWAIT(Task<void>::Delay(100ms));
   AWAIT(Task<void>::Delay(200ms));
  }());
 }

 ExpectingTimer auto_68_2 {300ms, 20ms}; 
 CPP2_UFCS_0(Join, std::move(task.value()));
}

#line 73 "DelayTests.cpp2"
auto InParallel() -> void{
 TESTPOOL(2);

 cpp2::deferred_init<Task<void>> task; 

 {
  ExpectingTimer auto_79_3 {0ms, 5ms}; 
  task.construct([]() mutable -> Task<void>{
   auto t100 {Task<void>::Delay(100ms)}; 
   AWAIT(Task<void>::Delay(200ms));
   AWAIT(std::move(t100));
  }());
 }

 ExpectingTimer auto_87_2 {200ms, 20ms}; 
 CPP2_UFCS_0(Join, std::move(task.value()));
}

#line 92 "DelayTests.cpp2"
auto ContinueInPool() -> void{
 TESTPOOL(2);

 cpp2::deferred_init<Task<void>> task; 

 vector<thread::id> ids {}; 

 {
  task.construct([_0 = (&ids)]() mutable -> Task<void>{
   CPP2_UFCS(push_back, (*cpp2::assert_not_null(_0)), this_thread::get_id());
   AWAIT(Task<void>::ContinueInPool());
   CPP2_UFCS(push_back, (*cpp2::assert_not_null(_0)), this_thread::get_id());
  }());
 }

 CPP2_UFCS_0(Join, std::move(task.value()));

 EXPECT_EQ(2, CPP2_UFCS_0(size, ids));
 EXPECT_NE(CPP2_ASSERT_IN_BOUNDS(ids, 0), CPP2_ASSERT_IN_BOUNDS(std::move(ids), 1));
}

