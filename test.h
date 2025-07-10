#ifndef TEST_H
#define TEST_H

#include <unordered_map>
#include <string_view>

std::unordered_map<std::string_view, fp_t> g_mapTestCases = 
{
    {"fnint('x+2',2,3,'x')", static_cast<fp_t>(4.5)},
    {"fnint('x+2',2,3,'x')+fnint('x^2+2',2,3,'x')", static_cast<fp_t>(12.833333)},
    {"prod('x+3',2,5,1,'x')+deriv('mu^2',4,'mu')", 1688},
    {"3epsilon+2*g+2sin(pi*0.5)%3", static_cast<fp_t>(21.61403)},
    {"pi(2+3)", static_cast<fp_t>(15.7079632679489656)},
    {"sin(max(2,3)/3*pi)", 0},
    {"2+5*(3/5+2)", 15},
    {"sin(pi*0.5)", 1},
    {"sin(pi/2)", 1},
    {"2+5%3*2", 6},
    {"2+5%2^2", 3}
};

inline bool ExecuteTestCases()
{
    for(auto& [infix, res] : g_mapTestCases)
        if(std::abs(Evaluate(infix.data()) - res) > g_mapConstantNames.at("epsilon"))
            return false;
    return true;
}

#endif