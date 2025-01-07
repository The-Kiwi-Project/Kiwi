#include <unordered_map>
#include <vector>
#include <memory>

int main() {
    auto m = std::unordered_map<int, std::vector<std::unique_ptr<int>>> {};
}