#include <iostream>
#include <vector>

int main()
{
    size_t order = 4;
    std::vector<int>
        data{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    
    size_t index = 0;
    for (size_t i = 0; i < 10; i++)
    {
        for (size_t lie = 0; lie < order; lie++)
        {
            data.insert(data.begin() + index, i);
            index++;
        }
        index++;
    }

    std::cout << data.size() << '\n';
    for (size_t i = 0; i < data.size(); i++)
        std::cout << data.at(i) << ' ';
}
