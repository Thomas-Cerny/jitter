#include <fstream>
#include <vector>
#include <regex>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <numeric>

struct Data {
    std::vector<float> wanted_vtr;  //elements will be altered, used for interval calculation
    std::vector<float> wanted_alt;  //will be sorted in intervals
    unsigned short wanted_size;
};

//store amount of numbers of each interval in intervals_count[] and interval outliers in intervals[]
struct Interval {
    unsigned short interval_count[12] = { 0 };
    float intervals[11];
    float tolerance;
};

float calculateMean(const Data data);
float calculateDispersion(const Data data);
float calculateJitter(const Data data);
void calculateIntervals(const Data data, Interval& interval);
void removeOutliers(Data& data, Interval& interval);
void manRemove(Data& data, Interval& interval);
void sortIntervals(const Data data, Interval& interval);
void consoleOut(const Data data, const Interval interval);
double setPrecision(float val);

int main()
{
    //read data from input.txt
    Data data;

    std::regex wanted_rgx(R"(time=(\d+(\.\d+)?))");

    std::ifstream input("input.txt");

    std::string file_data((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

    //store wanted data matching wanted_rgx in wanted_vtr
    auto wanted_begin = std::sregex_iterator(file_data.begin(), file_data.end(), wanted_rgx);
    auto wanted_end = std::sregex_iterator();

    for (; wanted_begin != wanted_end; wanted_begin++)
    {
        std::string match_time = (*wanted_begin)[1].str();

        data.wanted_vtr.push_back(stof(match_time));     //store wanted data in wanted_vtr as type float
    }

    input.close();

    //check if data was loaded
    if (data.wanted_vtr.empty())
    {
        std::cout << "Couldn't read data from file" << std::endl;

        return 1;
    }

    data.wanted_size = data.wanted_vtr.size();    //calculate size of wanted_vtr, max value is +65535, this is the maximum amount of ping values

    float mean = calculateMean(data);    //calculate mean of wanted_vtr
    float dispersion = calculateDispersion(data);   //calculate dispersion of wanted_vtr
    float jitter = calculateJitter(data);   //calculate packet-to-packet jitter of wanted_vtr

    Interval interval;

    data.wanted_alt = data.wanted_vtr;      //copy elements of wanted_vtr to wanted_alt
    std::sort(data.wanted_alt.begin(), data.wanted_alt.end());

    calculateIntervals(data, interval);     //calculate intervals

    removeOutliers(data, interval);     //remove outliers, recalculate intervals, sort intervals

    consoleOut(data, interval);     //print sorted values to console

    manRemove(data, interval);      //manually remove values, recalculate intervals, sort intervals

    consoleOut(data, interval);     //print final sorted values to console

    //print mean, dispersion and jitter to console
    std::cout << "Mean: " << setPrecision(mean) << std::endl;
    std::cout << "Dispersion: " << setPrecision(dispersion) << std::endl;
    std::cout << "Jitter: " << jitter << std::endl;
}

//calculate mean of wanted_vtr
float calculateMean(const Data data)
{
    return std::accumulate(data.wanted_vtr.begin(), data.wanted_vtr.end(), 0.0f) / data.wanted_size;
}

//calculate dispersion of wanted_vtr
float calculateDispersion(const Data data)
{
    return *std::max_element(data.wanted_vtr.begin(), data.wanted_vtr.end()) - *std::min_element(data.wanted_vtr.begin(), data.wanted_vtr.end());
}

//calculate packet-to-packet jitter of wanted_vtr
float calculateJitter(const Data data)
{
    float sum = 0;

    for (size_t i = 1; i < data.wanted_vtr.size(); i++)
        sum += std::abs(data.wanted_vtr[i] - data.wanted_vtr[i - 1]);

    return sum / (data.wanted_size - 1);
}

//calculates the outliers for 10 intervals
void calculateIntervals(const Data data, Interval& interval)
{
    //calculates the biggest and smallest interval outliers
    float min_outlier = *std::min_element(data.wanted_alt.begin(), data.wanted_alt.end());
    float max_outlier = *std::max_element(data.wanted_alt.begin(), data.wanted_alt.end());

    interval.intervals[0] = min_outlier;
    interval.intervals[10] = max_outlier;

    //calculates the medial interval outliers from min_outlier and max_outlier (a bit inaccurate)
    for (size_t i = 1; i < 10; i++)
        interval.intervals[i] = interval.intervals[0] + i * ((interval.intervals[10] - interval.intervals[0]) / 10);
}

//automaticaly remove wanted_vtr elements larger than tolerance and store in wanted_alt, console output
void removeOutliers(Data& data, Interval& interval)
{
    while (data.wanted_alt.size() >= 4)     //continue cycle if there are 4+ elements in wanted_alt
    {
        size_t size = data.wanted_alt.size();   //ids of last 4 elements in wanted_alt

        //check whether each difference of pairs of last 4 elements of wanted_alt is bigger than set tolerance
        if ((data.wanted_alt[size - 1] - data.wanted_alt[size - 2] > interval.tolerance) || (data.wanted_alt[size - 2] - data.wanted_alt[size - 3] > interval.tolerance) || (data.wanted_alt[size - 3] - data.wanted_alt[size - 4] > interval.tolerance))
        {
            std::cout << "Deleted number: " << data.wanted_alt.back() << std::endl;

            data.wanted_alt.pop_back();     //if condition is met, delete last element of wanted_alt
        }
        else
            break;
    }

    std::cout << std::endl;

    calculateIntervals(data, interval);

    sortIntervals(data, interval);
}

//choice to manually remove elements from front and back of wanted_alt
void manRemove(Data& data, Interval& interval)
{
    std::cout << "Delete more numbers? [y/n]" << std::endl << std::endl;

    char del_char;
    std::cin >> del_char;

    if (del_char == 'y')        //deletion if del_char is 'y'
    {
        std::cout << std::endl << "How many from front?" << std::endl << std::endl;

        short del_front;
        std::cin >> del_front;      //value must be 0 or greater, 0 means no erasion

        std::cout << std::endl << "How many from back?" << std::endl << std::endl;

        short del_back;
        std::cin >> del_back;       //value must be 0 or greater, 0 means no erasion

        if (del_front > 0)
            for (size_t i = 0; i < del_front; i++)
                data.wanted_alt.erase(data.wanted_alt.begin());   //erase elements from front of wanted_alt if condition is met

        if (del_back > 0)
            for (size_t i = 0; i < del_back; i++)
                data.wanted_alt.pop_back();         //erase elements from back of wanted_alt if condition is met
    }

    calculateIntervals(data, interval);

    sortIntervals(data, interval);
}

//sorting wanted_vtr into the already defined intervals
void sortIntervals(const Data data, Interval& interval)
{
    for (size_t i = 0; i < data.wanted_size; i++)   //sorting wanted_vtr, wanted_size = wanted_vtr.size()
        for (size_t j = 0; j < 11; j++)             //loops for 12 intervals
        {
            if (data.wanted_vtr[i] < interval.intervals[0])     //if number is smaller than first interval
            {
                interval.interval_count[0]++;
                break;
            }
            else if (data.wanted_vtr[i] >= interval.intervals[j] && data.wanted_vtr[i] < interval.intervals[j + 1])  //if number is within indexed interval
            {
                interval.interval_count[j]++;
                break;
            }
            else if (data.wanted_vtr[i] >= interval.intervals[10])    //if number is greater than last interval
            {
                interval.interval_count[11]++;
                break;
            }
        }
}

//print sorted results to console, number printed with 2 decimal place precision
void consoleOut(const Data data, const Interval interval)
{
    std::cout << "0 - " << setPrecision(interval.intervals[0]) << ": " << interval.interval_count[0] << std::endl;

    for (size_t i = 1; i < 11; i++)
        std::cout << setPrecision(interval.intervals[i - 1]) << " - " << setPrecision(interval.intervals[i]) << ": " << interval.interval_count[i] << std::endl;

    std::cout << setPrecision(interval.intervals[10]) << "+ : " << interval.interval_count[11] << std::endl << std::endl;
}

//set precision to 2 decimal places
double setPrecision(float val)
{
    return std::round(val * 100.) / 100.;
}