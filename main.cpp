#include <fstream>
#include <vector>
#include <regex>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <cstdint>

struct Data {
    std::vector<float> wanted_vtr;
    uint16_t wanted_size;
}

struct Interval {
    uint16_t interval_count[12];
    float intervals[11];
}

float calculateMean(const Data& data);
float calculateStdDev(const Data& data, float mean);
void removeOutliers(const Data& data, float mean, float std_dev, const Interval& interval);
void calculateIntervals(const std::vector<float>& result, const Interval& interval);
float calculateDispersion(const Data& data);
float calculateJitter(const std::vector<float>& wanted_vtr, const Data& data);
void sortIntervals(const std::vector<float>& result, const Data& data, const Interval& interval);
void printToFile(const Interval& interval);

int main()
{
    //read data from input.txt and store the wanted data in wanted_vtr
    Data data;

    std::regex wanted_rgx(R"(time=(\d+(\.\d+)?))");

    std::ifstream input("input.txt");

        //check if the data was loaded
    if (data.wanted_vtr.empty())
    {
        std::cout << "Couldn't read data from file" << std::endl;

        return 1;
    }

    std::string file_data((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

    auto wanted_begin = std::sregex_iterator(file_data.begin(), file_data.end(), wanted_rgx);
    auto wanted_end = std::sregex_iterator();

    for ( ; wanted_begin != wanted_end; wanted_begin++)
    {
        std::string match_time = (*wanted_begin) [1].str();

        data.wanted_vtr.push_back(stod(match_time));     //stores the wanted data in wanted_vtr as type float
    }

    input.close();

    data.wanted_size = data.wanted_vtr.size();    //calculates the size of wanted_vtr, max value is +65535, this is the maximum amount of ping values

    float mean = calculateMean(data);    //calculates the mean of wanted_vtr
    float std_dev = calculateStdDev(data, mean); //calculates the standard deviation of wanted_vtr
    float dispersion = calculateDispersion(data.wanted_vtr);   //calculates the dispersion of wanted_vtr
    float jitter = calculateJitter(data);   //calculates the packet-to-packet jitter of wanted_vtr

    std::cout << "mean: " << mean << std::endl;
    std::cout << "std_dev: " << std_dev << std::endl;
    std::cout << "dispersion: " << dispersion << std::endl;
    std::cout << "jitter: " << jitter << std::endl;

    Interval interval;

    interval.intervals[11];
    interval.interval_count[12] = {0};  //stores the amount of numbers in each interval

    //debug
    for (const auto& i : wanted_vtr)
        std::cout << i << " ";

    std::cout << std::endl;

    std::cout << "pocet hodnot:" << data.wanted_vtr.size() << std::endl;
    //

    for (size_t i = 0; i < 11; i++)
        interval.intervals[i] = i * 10;

    //debug
    for (size_t i = 0; i < 11; i++)
        std::cout << interval.intervals[i] << " ";
    //

    removeOutliers(data, mean, std_dev, interval_count, intervals);

    //print results to file output.txt
    printToFile(interval_count, intervals);

    //print results to console
    for (size_t i = 0; i < 11; i++)
        std::cout << data.intervals[i] << " - " << data.intervals[i + 1] << ": " << data.interval_count[i] << std::endl << std::endl;

    std::cout << "Mean: " << mean << std::endl << std::endl;

    std::cout << "Packet-to-packet jitter: " << jitter << std::endl << std::endl;

    std::cout << "Dispersion: " << dispersion << std::endl << std::endl;
}

//calculate mean of wanted_vtr - CHECK FOR LOGICAL CALCULATION ERROR
float calculateMean(const Data& data)
{
    return std::accumulate(data.wanted_vtr.begin(), data.wanted_vtr.end(), 0) / data.wanted_size;
}

//calculate standard deviation of wanted_vtr - CHECK FOR LOGICAL CALCULATION ERROR
float calculateStdDev(const Data& data, float mean)
{
    float sum = 0;

    for (auto& i : data.wanted_vtr)
        sum += std::pow(i - mean, 2);

    return std::sqrt(sum / data.wanted_size);
}

//remove outliers from wanted_vtr, calculate intervals and sort wanted_vtr into the intervals
void removeOutliers(const Data& data, float mean, float std_dev, const Interval& interval)
{
    std::vector<float> result;
    std::vector<float> outliers;

    for (auto& i : data.wanted_vtr)
        if (i < mean + 1.5 * std_dev && i > mean - 1.5 * std_dev)   //if the number is within 1.5 standard deviations from the mean, it is not an outlier
            result.push_back(i);
        else
        {
            outliers.push_back(i);

            std::cout << "Deleted number: " << i << std::endl;  
        }

    std::cout << "============================================================" << std::endl;

    calculateIntervals(result, interval);

    //debug
    for (size_t i = 0; i < 11; i++)
        interval.intervals[i] = 0;

    for (size_t i = 0; i < 12; i++)
        interval.interval_count[i] = 0;
    //

    for (const auto& i : outliers)
        result.push_back(i);

    sortIntervals(result, interval, data);

    //debug
    for (size_t i = 0; i < 13; i++)
        std::cout << interval.interval_count[i] << " ";
    //
}

//calculates the outliers for 10 intervals - CHECK FOR LOGICAL CALCULATION ERROR
void calculateIntervals(const std::vector<float>& result, const Interval& interval)
{
    float min_outlier = *std::min_element(result.begin(), result.end());
    float max_outlier = *std::max_element(result.begin(), result.end());

    interval.intervals[0] = min_outlier;
    interval.intervals[10] = max_outlier;

    for (size_t i = 1; i < 11; i++)
        interval.intervals[i] = interval.intervals[0] + i * (interval.intervals[10] - interval.intervals[0]) / 10;
}

//calculate dispersion of wanted_vtr
float calculateDispersion(const Data& data)
{
    return *std::max_element(data.wanted_vtr.begin(), wanted_vtr.end()) - *std::min_element(wanted_vtr.begin(), wanted_vtr.end());
}

//calculate packet-to-packet jitter of wanted_vtr
float calculateJitter(const std::vector<float>& wanted_vtr, const Data& data)
{
    float sum = 0;

    for (size_t i = 1; i < data.wanted_vtr.size(); i++)
        sum += std::abs(data.wanted_vtr[i] - data.wanted_vtr[i - 1]);

    return sum / (data.wanted_size - 1);
}

//sorting wanted_vtr into the already defined intervals - CHECK FOR LOGICAL ERROR
void sortIntervals(const std::vector<float>& result, const Data& data, const Interval& interval)
{
    for (size_t i = 0; i < data.wanted_size; i++)
        for (size_t j = 0; j < 12; j++)
        {
            if (result[i] < data.intervals[0])   //if the number is smaller than the first interval, it is stored in the first interval
            {
                interval.interval_count[0]++;
                break;
            }
            else if (result[i] >= interval.intervals[j] && result[i] < interval.intervals[j + 1])  //if the number is within the interval, it is stored in the interval
            {
                interval.interval_count[j]++;
                break;
            }
            else if (result[i] >= interval.intervals[11])    //if the number is bigger than the last interval, it is stored in the last interval
            {
                interval.interval_count[11]++;
                break;
            }
        }
}

void printToFile(const Interval& interval)
{
    std::ofstream output("output.txt");

    for (size_t i = 0; i < 11; i++)
        output << interval.intervals[i] << " - " << interval.intervals[i + 1] << ": " << interval.interval_count[i] << std::endl;

    output.close();
}