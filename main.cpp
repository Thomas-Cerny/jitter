#include <fstream>
#include <vector>
#include <sstream>
#include <regex>
#include <cstdint>

int main()
{
    std::regex wanted_rgx(R"(time=(\d+(\.\d+)?))");

    //nacteni dat z input.txt do wanted_vtr
    std::vector<float> wanted_vtr;

    std::ifstream input("input.txt");

    std::string data((std::ifstreambuf_iterator<char>(input)), std::ifstreambuf_iterator<char>());

    auto wanted_begin = std:sregex_iterator(data.begin(), data.end(), wanted_rgx);
    auto wanted_end = std::sregex_iterator();

    for ( ; wanted_begin != wanted_end; wanted_begin++)
    {
        std::string match_time = (*wanted_begin) [1].str();

        wanted_vtr.push_back(stod(match_time));
    }

    input.close();

    if (wanted_vtr.empty())
    {
        std::cout << "Nepodarilo se nacist data ze souboru" << std::endl;

        return 1;
    }

    sortVector(wanted_vtr);

    int64_t wanted_size = wanted_vtr.size();    //urcuje pocet dat v wanted_vtr
    float each_interval = wanted_vtr.back() - wanted_vtr.front() / 100;

    while (wanted_size > 1)
    {

        for (size_t i = 0; i < wanted_size; )
    }
}

float sortVector(std::vector<float> wanted_vtr)
{
    std::sort(wanted_vtr.begin(), wanted_vtr.end());

    return wanted_vtr;
}

std::vector<float> delBounds(std::vector<float> wanted_vtr, int64_t wanted_size, float each_interval)
{
    size_t idx[3];

    for (size_t i = 0; i < 4; i++)
        idx[i] = wanted_size - (i + 1);

    for (size_t i = 0; i < wanted_size; i++)
    {
        if (wanted_vtr)
    }
}
