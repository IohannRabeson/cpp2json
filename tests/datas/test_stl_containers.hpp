#ifndef TEST_STL_CONTAINERS_HPP
#define TEST_STL_CONTAINERS_HPP

#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <array>
#include <unordered_set>

class ClassTestStlContainerTypes
{
    std::string a_string;
    std::vector<std::string> a_vector_string;
    std::list<std::string> a_list_string;
    std::set<std::string> a_set_string;
    std::multiset<std::string> a_multiset_string;
    std::map<std::string, std::string> a_map_string;
    std::multimap<std::string, std::string> a_multimap_string;
    std::array<std::string, 123u> an_array_string;
    std::unordered_set<std::string> an_unordered_set_string;
    std::unordered_multiset<std::string> an_unordered_multiset_string;
};

#endif // TEST_STL_CONTAINERS_HPP
