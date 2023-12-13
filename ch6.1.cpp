#include <unordered_map>
#include <algorithm>
#include <iostream>

template<typename T>
concept Integer = std::is_integral<T>::value;

template<Integer T, size_t Length>
T mode(const T (&values)[Length]) { // TODO: Why (&values)[Length]
    std::unordered_map<T, int> counts;

    for (size_t i = 0; i < Length; i++) {
        counts[values[i]]++;
    }

    T result{};
    int max_count = 0;

    for (const auto& pair : counts) {
        if (pair.second > max_count) {
            result = pair.first;
            max_count = pair.second;
        }
    }

    if (
        std::any_of(
            counts.begin(),
            counts.end(),
            [max_count, result](std::pair<const T, int>& pair) {
                return pair.second == max_count && pair.first != result;
            }
        )
    ) {
        return T{};
    }

    return result;
}

int main() {
    int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 9};
    std::cout << mode(values) << std::endl;

    long values2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 9};
    std::cout << mode(values2) << std::endl;

    // This will not compile because double is not an Integer
    // double values3[] = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9, 9.9};
    // std::cout << mode(values3) << std::endl;
}
