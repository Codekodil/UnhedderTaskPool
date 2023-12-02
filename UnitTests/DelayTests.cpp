
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
#line 32 "DelayTests.cpp2"
TEST(DelayTests, DelayJoin)


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

 auto delayTask {Task<void>::Delay(100ms)}; 

 ExpectingTimer auto_29_2 {100ms, 10ms}; 
 CPP2_UFCS_0(Join, std::move(delayTask));
}

