#include <fstream>
#include <vector>
#include <regex>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <cstdint>

float calculateMean(const std::vector<float>& wanted_vtr, uint16_t wanted_size);
float calculateStdDev(const std::vector<float>& wanted_vtr, float mean, uint16_t wanted_size);
void removeOutliers(const std::vector<float>& wanted_vtr, float mean, float std_dev, uint16_t interval_count[12], float intervals[11], uint16_t wanted_size);
void calculateIntervals(const std::vector<float>& result, float intervals[11]);
float calculateDispersion(const std::vector<float>& wanted_vtr);
float calculateJitter(const std::vector<float>& wanted_vtr, uint16_t wanted_size);
void sortIntervals(const std::vector<float>& result, float intervals[11], uint16_t wanted_size, uint16_t interval_count[12]);
void printToFile(uint16_t interval_count[12], float intervals[11]);

int main()
{
    //read data from input.txt and store the wanted data in wanted_vtr
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

    uint16_t wanted_size = wanted_vtr.size();    //calculates the size of wanted_vtr, max value is +65535, this is the maximum amount of ping values

    float mean = calculateMean(wanted_vtr, wanted_size);    //calculates the mean of wanted_vtr
    float std_dev = calculateStdDev(wanted_vtr, mean, wanted_size); //calculates the standard deviation of wanted_vtr
    float dispersion = calculateDispersion(wanted_vtr);   //calculates the dispersion of wanted_vtr
    float jitter = calculateJitter(wanted_vtr, wanted_size);   //calculates the packet-to-packet jitter of wanted_vtr

    float intervals[11];

    uint16_t interval_count[12] = {0};  //stores the amount of numbers in each interval

    removeOutliers(wanted_vtr, mean, std_dev, interval_count, intervals, wanted_size);

    //print results to file output.txt
    printToFile(interval_count, intervals);

    //print results to console
    for (size_t i = 0; i < 11; i++)
        std::cout << intervals[i] << " - " << intervals[i + 1] << ": " << interval_count[i] << std::endl << std::endl;

    std::cout << "Mean: " << mean << std::endl << std::endl;

    std::cout << "Packet-to-packet jitter: " << jitter << std::endl << std::endl;

    std::cout << "Dispersion: " << dispersion << std::endl << std::endl;
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

//remove outliers from wanted_vtr, calculate intervals and sort wanted_vtr into the intervals
void removeOutliers(const std::vector<float>& wanted_vtr, float mean, float std_dev, uint16_t interval_count[12], float intervals[11], uint16_t wanted_size)
{
    std::vector<float> result;
    std::vector<float> outliers;

    for (auto& i : wanted_vtr)
        if (i < mean + 1.5 * std_dev && i > mean - 1.5 * std_dev)   //if the number is within 1.5 standard deviations from the mean, it is not an outlier
            result.push_back(i);
        else
        {
            outliers.push_back(i);

            std::cout << "Deleted number: " << i << std::endl;  
        }

    std::cout << "============================================================" << std::endl;

    calculateIntervals(result, intervals);

    for (const auto& i : outliers)
        result.push_back(i);

    sortIntervals(result, intervals, wanted_size, interval_count);
}

//calculates the outliers for 10 intervals
void calculateIntervals(const std::vector<float>& result, float intervals[11])
{
    float min_outlier = *std::min_element(result.begin(), result.end());
    float max_outlier = *std::max_element(result.begin(), result.end());

    intervals[0] = min_outlier;
    intervals[10] = max_outlier;

    for (size_t i = 1; i < 11; i++)
        intervals[i] = intervals[0] + i * (intervals[10] - intervals[0]) / 10;
}

//calculate dispersion of wanted_vtr
float calculateDispersion(const std::vector<float>& wanted_vtr)
{
    return *std::max_element(wanted_vtr.begin(), wanted_vtr.end()) - *std::min_element(wanted_vtr.begin(), wanted_vtr.end());
}

//calculate packet-to-packet jitter of wanted_vtr
float calculateJitter(const std::vector<float>& wanted_vtr, uint16_t wanted_size)
{
    float sum = 0;

    for (size_t i = 1; i < wanted_vtr.size(); i++)
        sum += std::abs(wanted_vtr[i] - wanted_vtr[i - 1]);

    return sum / (wanted_size - 1);
}

//sorting wanted_vtr into the already defined intervals
void sortIntervals(const std::vector<float>& result, float intervals[11], uint16_t wanted_size, uint16_t interval_count[12])
{
    for (size_t i = 0; i < wanted_size; i++)
        for (size_t j = 0; j < 12; j++)
        {
            if (result[i] < intervals[0])   //if the number is smaller than the first interval, it is stored in the first interval
            {
                interval_count[0]++;
                break;
            }
            else if (result[i] >= intervals[j] && result[i] < intervals[j + 1])  //if the number is within the interval, it is stored in the interval
            {
                interval_count[j]++;
                break;
            }
            else if (result[i] >= intervals[11])    //if the number is bigger than the last interval, it is stored in the last interval
            {
                interval_count[11]++;
                break;
            }
        }
}

void printToFile(uint16_t interval_count[12], float intervals[11])
{
    std::ofstream output("output.txt");

    for (size_t i = 0; i < 11; i++)
        output << intervals[i] << " - " << intervals[i + 1] << ": " << interval_count[i] << std::endl;

    output.close();
}