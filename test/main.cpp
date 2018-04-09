#include <chrono>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include "json/json.hpp"

using json = nlohmann::json;

struct TestA {
    int a1;
    int a2;
    int a3;
    std::string a4;
    std::string a5;
    std::string a6;
    std::list<int> a7;
    std::vector<int> a8;
    std::list<std::string> a9;
    std::vector<std::string> a10;
};

void to_json(json &j, const TestA &ta) {
    j = json{{"a1", ta.a1}, {"a2", ta.a2},  {"a3", ta.a3}, {"a4", ta.a4},
             {"a5", ta.a5}, {"a6", ta.a6},  {"a7", ta.a7}, {"a8", ta.a8},
             {"a9", ta.a9}, {"a10", ta.a10}};
}

void from_json(const json &j, TestA &ta) {
    ta.a1 = j.at("a1").get<int>();
    ta.a2 = j.at("a2").get<int>();
    ta.a3 = j.at("a3").get<int>();
    ta.a4 = j.at("a4").get<std::string>();
    ta.a5 = j.at("a5").get<std::string>();
    ta.a6 = j.at("a6").get<std::string>();
    ta.a7 = j.at("a7").get<std::list<int>>();
    ta.a8 = j.at("a8").get<std::vector<int>>();
    ta.a9 = j.at("a9").get<std::list<std::string>>();
    ta.a10 = j.at("a10").get<std::vector<std::string>>();
}

struct TestB {
    TestA b1;
    std::list<TestA> b2;
    std::vector<TestA> b3;
};

void to_json(json &j, const TestB &tb) {
    j = json{{"b1", tb.b1}, {"b2", tb.b2}, {"b3", tb.b3}};
}

void from_json(const json &j, TestB &tb) {
    tb.b1 = j.at("b1").get<TestA>();
    tb.b2 = j.at("b2").get<std::list<TestA>>();
    tb.b3 = j.at("b3").get<std::vector<TestA>>();
}

void test_json_msgpack() {
    TestA ta;
    ta.a1 = 1;
    ta.a2 = 2;
    ta.a3 = 3;
    ta.a4 = "4444";
    ta.a5 = "55555";
    ta.a6 = "666666";
    ta.a7 = {7, 7, 7, 7, 7, 7, 7};
    ta.a8 = {8, 8, 8, 8, 8, 8, 8, 8};
    ta.a9 = {"9", "9", "9", "9", "9", "9", "9", "9", "9"};
    ta.a10 = {"10", "10", "10", "10", "10", "10", "10", "10", "10", "10"};

    TestB tb;
    tb.b1 = ta;
    tb.b2 = {ta, ta};
    tb.b3 = {ta, ta, ta};

    // json j = tb;
    // auto pack = json::to_msgpack(j);

    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100000; ++i) {
        // json j = ta;
        json j = tb;
        // TestA ta1 = j;
        // TestB tb1 = j;
        // auto pack = json::to_msgpack(j);
        // auto conv = json::from_msgpack(pack);
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto time_span =
        std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "cost: " << time_span.count() << std::endl;
}

int main(int argc, char *argv[]) {
    test_json_msgpack();
    return 0;
}