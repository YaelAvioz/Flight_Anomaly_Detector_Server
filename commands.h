#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <iostream>
#include <string.h>
#include <map>
#include <fstream>
#include <sstream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include "HybridAnomalyDetector.h"

using namespace std;

class DefaultIO
{
public:
	virtual string read() = 0;
	virtual void write(string text) = 0;
	virtual void write(float f) = 0;
	virtual void read(float *f) = 0;
	virtual ~DefaultIO() {}
};

// Defines class of SocketIO
class SocketIO : public DefaultIO
{
protected:
	int client_fd;

public:
	SocketIO(int client_fd) : client_fd(client_fd){};

	string read()
	{
		string input = "";
		char c = 0;
		recv(client_fd, &c, sizeof(char), 0);
		while (c != '\n')
		{
			input += c;
			recv(client_fd, &c, sizeof(char), 0);
		}
		return input;
	}

	void write(string text)
	{
		send(client_fd, text.c_str(), text.length(), 0);
	}

	void write(float f)
	{
		string s = to_string(f);
		s.erase(s.find_last_not_of('0') + 1, std::string::npos);
		send(client_fd, s.c_str(), s.length(), 0);
	}

	void read(float *f)
	{
		recv(client_fd, f, sizeof(float *), 0);
	}
};

// Define the type of range
class Range
{
public:
	int begin, end;
	Range(int begin, int end) : begin(begin), end(end) {}
};

class Command
{
public:
	Command(DefaultIO *dio, vector<AnomalyReport> &anomaly_report, HybridAnomalyDetector &anomaly_detector) : dio(dio), anomaly_report(anomaly_report), anomaly_detector(anomaly_detector) {}
	virtual void execute() = 0;
	virtual string command_description() = 0;
	virtual ~Command() {}

protected:
	DefaultIO *dio;
	vector<AnomalyReport> &anomaly_report;
	HybridAnomalyDetector &anomaly_detector;
};

// Option 1 - Upload CSV File
class upload_command : public Command
{
public:
	upload_command(DefaultIO *dio, vector<AnomalyReport> &anomaly_report, HybridAnomalyDetector &anomaly_detector)
		: Command(dio, anomaly_report, anomaly_detector) {}

	// Set the command description
	string description = "upload a time series csv file";

	// Execute the command
	void execute()
	{
		dio->write("Please upload your local train CSV file.\n");
		upload("anomalyTrain.csv");
		dio->write("Upload complete.\nPlease upload your local test CSV file.\n");
		upload("anomalyTest.csv");
		dio->write("Upload complete.\n");
	}

	// Returns the command description
	string command_description()
	{
		return description;
	}

	// Open the given file and read it until the word "done" and save it to a new file
	void upload(string file_name)
	{
		ofstream file;
		file.open(file_name);
		string line;
		while ((line = dio->read()) != "done")
		{
			file << line << "\n";
		}
	}
};

// Option 2 - Change the threshold
class settings_command : public Command
{
public:
	settings_command(DefaultIO *dio, vector<AnomalyReport> &anomaly_report, HybridAnomalyDetector &anomaly_detector)
		: Command(dio, anomaly_report, anomaly_detector) {}

	// Set the command description
	string description = "algorithm settings";

	// Execute the command
	void execute()
	{
		auto cur_threshold = anomaly_detector.m_threshold;
		dio->write("The current correlation threshold is ");
		dio->write(cur_threshold);
		dio->write("\n");
		bool valid_input = false;

		// This loop gets a value from the user and run until the user type a valid input
		do
		{
			dio->write("Type a new threshold\n");
			string threshold_s = dio->read();
			float new_threshold = std::atof(threshold_s.c_str());

			// In case the input is valid update the threshold
			if (new_threshold <= 1 && new_threshold >= 0)
			{
				anomaly_detector.m_threshold = new_threshold;
				valid_input = true;
			}

			// Eles print an error message
			else
			{
				dio->write("please choose a value between 0 and 1.");
			}
		} while (!valid_input);
	}

	// Returns the command description
	string command_description()
	{
		return description;
	}
};

// Option 3 - Detect anomalies
class detect_command : public Command
{
public:
	detect_command(DefaultIO *dio, vector<AnomalyReport> &anomaly_report, HybridAnomalyDetector &anomaly_detector)
		: Command(dio, anomaly_report, anomaly_detector) {}

	// Set the command description
	string description = "detect anomalies";

	// Execute the command
	void execute()
	{
		TimeSeries train("anomalyTrain.csv");
		anomaly_detector.learnNormal(train);
		TimeSeries test("anomalyTest.csv");
		anomaly_report = anomaly_detector.detect(test);
		dio->write("anomaly detection complete.\n");
	}

	// Returns the command description
	string command_description()
	{
		return description;
	}
};

// Option 4 - Display the results
class display_command : public Command
{
public:
	display_command(DefaultIO *dio, vector<AnomalyReport> &anomaly_report, HybridAnomalyDetector &anomaly_detector)
		: Command(dio, anomaly_report, anomaly_detector) {}

	// Set the command description
	string description = "display results";

	// Execute the command
	void execute()
	{
		for (auto item : anomaly_report)
		{
			dio->write(item.timeStep);
			dio->write("\t ");
			dio->write(item.description);
			dio->write("\n");
		}
		dio->write("Done.\n");
	}

	// Returns the command description
	string command_description()
	{
		return description;
	}
};

// Option 5 - Upload anomalies and analyze results
class upload_and_analyze_command : public Command
{
public:
	upload_and_analyze_command(DefaultIO *dio, vector<AnomalyReport> &anomaly_report, HybridAnomalyDetector &anomaly_detector)
		: Command(dio, anomaly_report, anomaly_detector) {}

	// Set the command description
	string description = "upload anomalies and analyze results";

	// Execute the command
	void execute()
	{
		// Set the variabls
		float P = 0, N = 0, TP = 0, FP = 0;
		int sum = 0, counter = 0;
		std::map<string, std::vector<Range>> range_map;

		// For each description connect the intersection time and put this is vector of Range
		for (auto report : anomaly_report)
		{
			auto curr = range_map.find(report.description);
			// In case description not in map
			if (curr == range_map.end())
			{
				Range range(report.timeStep, report.timeStep);
				std::vector<Range> time_step_v = {range};
				range_map.insert({report.description, time_step_v});
			}
			else
			{
				// In case there is intersection - update the range of the last range in the vetor
				if (report.timeStep - curr->second.back().end == 1)
				{
					curr->second.back().end = report.timeStep;
				}
				// else, add new range to the vector
				else
				{
					Range new_range(report.timeStep, report.timeStep);
					curr->second.push_back(new_range);
				}
			}
		}

		std::vector<Range> v_range;

		// In this part the user upload the file
		dio->write("Please upload your local anomalies file.\n");
		string line;
		std::string begin;
		std::string end;
		while ((line = dio->read()) != "done")
		{
			std::istringstream stream(line);
			std::getline(stream, begin, ',');
			std::getline(stream, end, ',');
			Range new_range(std::atoi(begin.c_str()), std::atoi(end.c_str()));
			v_range.push_back(new_range);
		}
		dio->write("Upload complete.\n");

		// Checks inersectin between the given report and the user report
		for (auto it = range_map.begin(); it != range_map.end(); it++)
		{
			for (auto range : it->second)
			{
				for (auto report : v_range)
				{
					// In case there is no intersection
					if (checkIntersection(report, range))
					{
						TP++;
					}
				}
			}
			//count the nuber of ranges in the report
			counter += it->second.size();
		}

		FP = counter - TP;

		P = v_range.size();

		// Sum the number of the lines with detection of anomalies
		for (auto range : v_range)
		{
			sum += (range.end - range.begin + 1);
		}

		N = anomaly_detector.get_row_num() - sum;

		// This part prints the results
		TP = floor((TP * 1000) / P) / 1000;
		FP = floor((FP * 1000) / N) / 1000;
		dio->write("True Positive Rate: ");
		dio->write(TP);
		dio->write("\n");
		dio->write("False Positive Rate: ");
		dio->write(FP);
		dio->write("\n");
	}

	// Checks intersection between the ranges
	bool checkIntersection(Range first, Range second)
	{
		if (((first.begin <= second.begin) && (first.end >= second.end)) ||
			((first.end >= second.begin) && (second.end >= first.end)) ||
			((first.begin >= second.begin) && (second.end >= first.begin)))
		{
			return true;
		}
		return false;
	}

	// Returns the command description
	string command_description()
	{
		return description;
	}
};

#endif /* COMMANDS_H_ */
