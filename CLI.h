#ifndef CLI_H_
#define CLI_H_

#include <string.h>
#include "commands.h"

using namespace std;

class CLI {
	DefaultIO* dio;
	vector<AnomalyReport> anomaly_report;
    HybridAnomalyDetector anomaly_detector;
	// you can add data members
public:
	CLI(DefaultIO* dio);
	void start();
	virtual ~CLI();

private:
	std::vector<Command*> commands;
};

#endif /* CLI_H_ */
