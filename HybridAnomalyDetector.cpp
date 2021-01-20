#include "HybridAnomalyDetector.h"
#include "anomaly_detection_util.h"

HybridAnomalyDetector::HybridAnomalyDetector()
{
	// TODO Auto-generated constructor stub
}

HybridAnomalyDetector::~HybridAnomalyDetector()
{
	// TODO Auto-generated destructor stub
}

void HybridAnomalyDetector::learnNormal(const TimeSeries &ts)
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
			// In case there is corrlation between the vectors set cor
			if (((result >= m_threshold) && (result > max)) || ((result < m_threshold) && (result > 0.5)))
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
				if (result > m_threshold)
				{
					cor.is_circle = 0;
					for (size_t k = 0; k < v1.size(); k++)
					{
						Point point(feature1[k], feature2[k]);
						auto dev_result = dev(point, cor.lin_reg);
						if (dev_result > cor.threshold)
						{
							cor.threshold = dev_result;
						}
					}
				}
				else
				{
					// Creat an array of points
					auto **points = new Point *[v1.size()];
					for (int n = 0; n < v1.size(); n++)
					{
						points[n] = new Point(feature1[n], feature2[n]);
					}
					Circle min_circle = findMinCircle(points, v1.size());
					cor.threshold = min_circle.radius;
					cor.circle = min_circle;
					cor.is_circle = 1;
					// Free the memory
					for (int b = 0; b < v1.size(); b++)
					{
						delete points[b];
					}
					delete[] points;
				}
			}
		}
		// just if you find corrlation push this to the vector
		if (flag == 1)
		{
			cf.push_back(cor);
		}
	}
}

vector<AnomalyReport> HybridAnomalyDetector::detect(const TimeSeries &ts)
{
	std::vector<AnomalyReport> result;
	for (auto cor : cf)
	{
		int index1 = find(ts.m_names.begin(), ts.m_names.end(), cor.feature1) - ts.m_names.begin();
		int index2 = find(ts.m_names.begin(), ts.m_names.end(), cor.feature2) - ts.m_names.begin();

		//check the index
		if ((index1 < 0) || (index1 >= ts.m_table.size())) {
			continue;
		}
		if ((index2 < 0) || (index2 >= ts.m_table.size())) {
			continue;
		}

		auto v1 = ts.m_table[index1];
		auto v2 = ts.m_table[index2];
		float *feature1 = &v1[0];
		float *feature2 = &v2[0];

		m_row_number = v1.size();

		for (size_t i = 0; i < v1.size(); ++i)
		{
			Point point(feature1[i], feature2[i]);
			if (cor.is_circle == 0)
			{
				auto dev_result = dev(point, cor.lin_reg);
				if (dev_result > (cor.threshold) * 1.1)
				{
					AnomalyReport report(cor.feature1 + "-" + cor.feature2, i + 1);
					result.push_back(report);
				}
			}
			else
			{
				auto dist_result = distance(point, cor.circle.center);
				if (dist_result > ((cor.threshold) * 1.1))
				{
					AnomalyReport report(cor.feature1 + "-" + cor.feature2, i + 1);
					result.push_back(report);
				}
			}
		}
	}
	return result;
}

int HybridAnomalyDetector::get_row_num()
{
	return m_row_number;
}
