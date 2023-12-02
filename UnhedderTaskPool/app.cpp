
#define CPP2_INCLUDE_STD         Yes

//=== Cpp2 type declarations ====================================================


#include "cpp2util.h"

#line 1 "app.cpp2"


//=== Cpp2 type definitions and function declarations ===========================

#line 1 "app.cpp2"

#include "TaskPool.h"

using namespace AsyncTask;
using namespace std;

#define await(expr) co_await expr
#define coReturn(expr) co_return expr

#line 10 "app.cpp2"
[[nodiscard]] auto RecursiveAsync(cpp2::in<int> i) -> Task<int>;

#line 25 "app.cpp2"
auto main() -> int;

//=== Cpp2 function definitions =================================================

#line 1 "app.cpp2"

#line 10 "app.cpp2"
[[nodiscard]] auto RecursiveAsync(cpp2::in<int> i) -> Task<int>{
 await(Task<void>::Delay(100ms));
 cout << i << "\n";
 if (cpp2::cmp_less(i,5)) {
  auto n {await(RecursiveAsync(i + 1))}; 
  await(Task<void>::CompletedTask());
  auto result {await(Task<int>::FromResult(std::move(n) + i))}; 
  coReturn(std::move(result));
 }
 else {
  auto n {await(Task<int>::Run([]() mutable -> int { return 100;  }))}; 
  coReturn(std::move(n) + i);
 }
}

auto main() -> int{
 auto pool {make_shared<TaskPool>(2)}; 
 ThreadLink auto_27_2 {std::move(pool)}; 

 auto task {RecursiveAsync(0)}; 
 cout << "result: " + cpp2::to_string(CPP2_UFCS_0(Join, std::move(task))) + "\n";
}

