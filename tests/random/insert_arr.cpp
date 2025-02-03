#include <iostream>
#include <vector>

struct word
{
    int v0 = 0, v1 = 0;
};

// success
int old_main()
{

    size_t order = 2;
    std::vector<int>
        data{1, 2, 3, 4, 5, 6};
    size_t my_size = data.size();

    size_t index = 0;
    for (size_t i = 0; i < my_size; i++)
    {
        // computation
        for (size_t lie = 0; lie < order; lie++)
        {
            int route = data.at(i);
            data.insert(data.begin() + index, route);
            index++;
        }
        index++;
    }

    std::cout << data.size() << '\n';
    for (size_t i = 0; i < data.size(); i++)
        std::cout << data.at(i) << ' ';

    return 0;
}

/*
1 2
1 2
3 4
3 4
5 6
5 6
*/
