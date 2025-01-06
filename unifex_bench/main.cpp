#include <unifex/sync_wait.hpp>
#include <unifex/task.hpp>
#include <unifex/just.hpp>
#include <unifex/then.hpp>
#include <unifex/when_all.hpp>
#include <unifex/when_all_range.hpp>
#include <unifex/async_scope.hpp>
#include <unifex/async_manual_reset_event.hpp>

#include <chrono>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <ranges>
#include <vector>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,
                "Please specify the number of tasks\n"
                "example: %s 10000\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    std::vector<unifex::task<void>> tasks;
    unifex::async_manual_reset_event evt;
    int counter = 0;

    for (auto i : std::views::iota(0, std::stoi(argv[1]))) {
        tasks.push_back(
                [](int& counter, auto& evt) -> unifex::task<void> { ++counter; co_await evt.async_wait(); --counter; }(counter, evt));
    }

    unifex::sync_wait(unifex::when_all(
        unifex::when_all_range(std::move(tasks)),
        unifex::just() | unifex::then([&] { std::cout << "counter=" << counter << "\n"; evt.set(); })
    ));
    return 0;
}
