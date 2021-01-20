/*
 * HybridAnomalyDetector.h
 *
 * Author: Yael Avioz,207237421
 */

#ifndef HYBRIDANOMALYDETECTOR_H_
#define HYBRIDANOMALYDETECTOR_H_

#include "SimpleAnomalyDetector.h"
#include "minCircle.h"

class HybridAnomalyDetector:public SimpleAnomalyDetector {
public:
	HybridAnomalyDetector();
	virtual ~HybridAnomalyDetector();
	void learnNormal(const TimeSeries &ts) override;
	vector<AnomalyReport> detect(const TimeSeries &ts) override;
	int get_row_num();
};

#endif /* HYBRIDANOMALYDETECTOR_H_ */
