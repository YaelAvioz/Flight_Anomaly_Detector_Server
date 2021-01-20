#ifndef TIMESERIES_H_
#define TIMESERIES_H_

#include <vector>
#include <string>

using namespace std;

class TimeSeries
{

public:
	TimeSeries(const char *CSVfileName);
	// Todo: return table function
	std::vector<std::vector<float>> m_table;
	std::vector<std::string> m_names;
};

#endif /* TIMESERIES_H_ */
