#include <fstream>
#include <vector>
#include <regex>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <cstdint>

float calculateMean(const std::vector<float>& wanted_vtr, int32_t wanted_size);
float calculateStdDev(const std::vector<float>& wanted_vtr, float mean, int32_t wanted_size);
std::vector<float> removeOutliers(const std::vector<float>& wanted_vtr, float mean, float std_dev);
void calculateIntervals(const std::vector<float>& wanted_vtr, float intervals[11]);

int main()
{
    //read data and store the wanted data in wanted_vtr
    std::regex wanted_rgx(R"(time=(\d+(\.\d+)?))");

    std::vector<float> wanted_vtr;

    std::ifstream input("input.txt");

    std::string data((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

    auto wanted_begin = std::sregex_iterator(data.begin(), data.end(), wanted_rgx);
    auto wanted_end = std::sregex_iterator();

    for ( ; wanted_begin != wanted_end; wanted_begin++)
    {
        std::string match_time = (*wanted_begin) [1].str();

        wanted_vtr.push_back(stod(match_time));     //stores the wanted data in wanted_vtr as type float
    }

    input.close();

    //check if the data was loaded
    if (wanted_vtr.empty())
    {
        std::cout << "Couldn't read data from file" << std::endl;

        return 1;
    }

    uint16_t wanted_size = wanted_vtr.size();    //calculates the size of wanted_vtr, max value is +2147483647

    float mean = calculateMean(wanted_vtr, wanted_size);
    float std_dev = calculateStdDev(wanted_vtr, mean, wanted_size);

    wanted_vtr = removeOutliers(wanted_vtr, mean, std_dev);

    float intervals[11];

    calculateIntervals(wanted_vtr, intervals);

    for (const auto& i : intervals)
        std::cout << i << std::endl;
}

//calculate mean of wanted_vtr
float calculateMean(const std::vector<float>& wanted_vtr, uint16_t wanted_size)
{
    return std::accumulate(wanted_vtr.begin(), wanted_vtr.end(), 0) / wanted_size;
}

//calculate standard deviation of wanted_vtr
float calculateStdDev(const std::vector<float>& wanted_vtr, float mean, uint16_t wanted_size)
{
    float sum = 0;

    for (auto& i : wanted_vtr)
        sum += std::pow(i - mean, 2);

    return std::sqrt(sum / wanted_size);
}

//remove outliers from wanted_vtr
std::vector<float> removeOutliers(const std::vector<float>& wanted_vtr, float mean, float std_dev)
{
    std::vector<float> result;

    for (auto& i : wanted_vtr)
        if (i < mean + 2 * std_dev && i > mean - 2 * std_dev)
            result.push_back(i);
        else
            std::cout << "Deleted number: " << i << std::endl;

    std::cout << "============================================================" << std::endl;

    return result;
}

//calculates the outliers for 10 intervals
void calculateIntervals(const std::vector<float>& wanted_vtr, float intervals[11])
{
    float min_outlier = *std::min_element(wanted_vtr.begin(), wanted_vtr.end());
    float max_outlier = *std::max_element(wanted_vtr.begin(), wanted_vtr.end());

    intervals[0] = min_outlier;
    intervals[10] = max_outlier;

    for (size_t i = 1; i < 10; i++)
        intervals[i] = intervals[0] + i * (intervals[10] - intervals[0]) / 10;
}