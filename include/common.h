#pragma once

#include <algorithm>
#include <vector>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <cassert>
#include <cmath>
#include <string>
#include <queue>
#include <set>
#include <map>
#include <cstdlib>
#include <unordered_map>

using namespace std;

const int MAX_N = 100005;
const int MAX_S = 105;
const int INF = 2000000000;

struct Context {
    using TSolution = vector<pair<int, int>>;
    TSolution Solution;

    vector<string> _deps[MAX_N];

    size_t C, T, S;
    vector<int> Deps[MAX_N];
    unordered_map<string, int> NameToID;
    string Name[MAX_N];
    int CT[MAX_N];
    int RT[MAX_N];

    string TargetName[MAX_N];
    int Target[MAX_N];
    int Deadline[MAX_N];
    int Points[MAX_N];

    void Input() {
        cin >> C >> T >> S;
        for (size_t i = 0; i < C; ++i) {
            cin >> Name[i] >> CT[i] >> RT[i];
            NameToID[Name[i]] = i;

            size_t n_deps;
            cin >> n_deps;
            for (size_t i = 0; i < n_deps; ++i) {
                string dep_name;
                cin >> dep_name;
                _deps[i].emplace_back(std::move(dep_name));
            }
        }
        for (size_t i = 0; i < C; ++i) {
            for (auto j : _deps[i]) {
                Deps[i].push_back(NameToID[j]);
            }
        }

        for (size_t i = 0; i < T; ++i) {
            cin >> TargetName[i] >> Deadline[i] >> Points[i];
            Target[i] = NameToID[TargetName[i]];
        }
    }

    void Output() {
        cout << Solution.size() << endl;
        for (auto& p : Solution) {
            cout << Name[p.first] << " " << p.second << endl;
        }
    }

    uint64_t Verify() {
        assert(Solution.size() <= C * S && "Too many steps.");

        map<pair<int, int>, int> compiledOnServer;
        int earliestAvailableEverywhere[MAX_N];
        int earliestAvailable[MAX_N];
        memset(earliestAvailable, -1, sizeof(earliestAvailable));
        memset(earliestAvailableEverywhere, -1, sizeof(earliestAvailableEverywhere));
        int timeOnServer[MAX_S] = {};

        auto getEarliestAvailable = [&](int file, int server) {
            if (earliestAvailableEverywhere[file] < 0) {
                return -1;
            }
            auto it = compiledOnServer.find(make_pair(file, server));
            if (it != compiledOnServer.end()) {
                return it->second;
            }
            return earliestAvailableEverywhere[file];
        };

        for (auto& [file, server] : Solution) {
            auto earliestStart = timeOnServer[server];
            for (auto dep : Deps[file]) {
                auto tDep = getEarliestAvailable(dep, server);
                assert(tDep >= 0 && "Dependency not available");
                earliestStart = max(earliestStart, tDep);
            }
            auto compileFinish = earliestStart + CT[file];
            if (earliestAvailable[file] < 0) {
                earliestAvailable[file] = compileFinish;
            } else {
                earliestAvailable[file] = min(earliestAvailable[file], compileFinish);
            }
            auto replicateFinish = compileFinish + RT[file];
            if (earliestAvailableEverywhere[file] < 0) {
                earliestAvailableEverywhere[file] = replicateFinish;
            } else {
                earliestAvailableEverywhere[file] = min(earliestAvailableEverywhere[file], replicateFinish);
            }
            assert(compiledOnServer.find(make_pair(file, server)) == compiledOnServer.end() && "Compiling same file on same server");
            compiledOnServer[make_pair(file, server)] = earliestStart + CT[file];
            timeOnServer[server] = compileFinish;
        }

        uint64_t score = 0;
        for (size_t i = 0; i < T; ++i) {
            if (earliestAvailable[i] < 0) {
                continue;
            }
            if (earliestAvailable[i] <= Deadline[i]) {
                score += Points[i] + (Deadline[i] - earliestAvailable[i]);
            }
        }

        return score;
    }

    uint64_t GetScore() {
        return Verify();
    }

    void Better(const int iterations = 1000, const int generation = 100) {

    }

};
