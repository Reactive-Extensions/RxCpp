#include "rxcpp/rx.hpp"
namespace rx=rxcpp;
namespace rxu=rxcpp::util;
namespace rxs=rxcpp::sources;
namespace rxsc=rxcpp::schedulers;
namespace rxsub=rxcpp::subjects;

#include "rxcpp/rx-test.hpp"
#include "catch.hpp"

const int static_onnextcalls = 100000;
static int aliased = 0;

SCENARIO("range observed on current_thread", "[hide][range][observe_on][long][perf]"){
    const int& onnextcalls = static_onnextcalls;
    GIVEN("a range"){
        WHEN("multicasting a million ints"){
            using namespace std::chrono;
            typedef steady_clock clock;

            auto el = rx::observe_on_new_thread();

            for (int n = 0; n < 10; n++)
            {
                std::atomic_bool done(false);
                auto c = std::make_shared<int>(0);

                auto start = clock::now();
                rxs::range<int>(1)
                    .take(onnextcalls)
                    .observe_on(el)
                    .subscribe(
                        [c](int){
                           ++(*c);
                        },
                        [&](){
                            done = true;
                        });
                while(!done) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                auto expected = onnextcalls;
                REQUIRE(*c == expected);
                auto finish = clock::now();
                auto msElapsed = duration_cast<milliseconds>(finish-start);
                std::cout << "range -> observe_on current_thread : " << (*c) << " on_next calls, " << msElapsed.count() << "ms elapsed, int-per-second " << *c / (msElapsed.count() / 1000.0) << std::endl;
            }
        }
    }
}
