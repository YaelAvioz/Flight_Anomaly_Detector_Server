#include "SimpleAnomalyDetector.h"
#include "AnomalyDetector.h"
#include <iostream>
#include <math.h>

SimpleAnomalyDetector::SimpleAnomalyDetector()
{
	// TODO Auto-generated constructor stub
}

SimpleAnomalyDetector::~SimpleAnomalyDetector()
{
	// TODO Auto-generated destructor stub
}

void SimpleAnomalyDetector::learnNormal(const TimeSeries &ts)
{
	//check corelation between columns.
	for (size_t i = 0; i < (ts.m_table.size()-1); i++)
	{
		correlatedFeatures cor;
		float max = 0;
		int flag = 0;
		for (size_t j = i+1; j < ts.m_table.size(); j++)
		{
			auto v1 = ts.m_table[i];
			auto v2 = ts.m_table[j];
			float *feature1 = &v1[0];
			float *feature2 = &v2[0];
			auto result = fabs(pearson(feature1, feature2, v1.size()));
			if (result > m_threshold && result > max)
			{
				flag = 1;
				max = result;

				cor.corrlation = result;
				cor.feature1 = ts.m_names[i];
				cor.feature2 = ts.m_names[j];

				float x = cov(feature1, feature2, v1.size()) / var(feature1, v1.size());
				float y = avg(feature2, v2.size()) - (x * avg(feature1, v1.size()));
				Line line(x, y);
				cor.lin_reg = line;

				cor.threshold = 0;
				for (size_t i = 0; i < v1.size(); ++i)
				{
					Point point(feature1[i], feature2[i]);
					auto dev_result = dev(point, cor.lin_reg);
					if (dev_result > cor.threshold)
					{
						cor.threshold = dev_result;
					}
				}
			}
		}
		if(flag == 1){
			cf.push_back(cor);
		}
	}
}

vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries &ts)
{
	std::vector<AnomalyReport> result;
	for (auto cor : cf)
	{
		auto index1 = find(ts.m_names.begin(), ts.m_names.end(), cor.feature1) - ts.m_names.begin();
		auto index2 = find(ts.m_names.begin(), ts.m_names.end(), cor.feature2) - ts.m_names.begin();

		auto v1 = ts.m_table[index1];
		auto v2 = ts.m_table[index2];
		float *feature1 = &v1[0];
		float *feature2 = &v2[0];

		for (size_t i = 0; i < v1.size(); ++i)
		{
			Point point(feature1[i], feature2[i]);
			auto dev_result = dev(point, cor.lin_reg);
			if (dev_result > (cor.threshold)*1.1)
			{
				AnomalyReport report(cor.feature1 + "-" + cor.feature2, i + 1);
				result.push_back(report);
			}
		}
	}
	return result;
}
