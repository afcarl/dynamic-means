#ifndef __EXPKERNEL_HPP
#include <Eigen/Dense>

namespace dmeans{

class ExpKernelData{
	public:
		Eigen::VectorXd v;
		double distTo(const ExpKernelData& rhs){
			return (this->v - rhs.v).squaredNorm();
		}
};

typedef std::map<int, ExpKernelData>::iterator expkernel_dmap_iterator;
class ExpKernelParameter{
	public:
		Eigen::VectorXd v, vOld;
		void update(expkernel_dmap_iterator be, expkernel_dmap_iterator en, double gamma){
			v = gamma*vOld;
			double wt = gamma;
			for(auto it = be; it != en; ++it){
				v += it->second.d.v;
				wt += 1.0;
			}
			v /= wt;
		}
		double cost(expkernel_dmap_iterator be, expkernel_dmap_iterator en, double gamma){
			double c = gamma*(v-vOld).squaredNorm();
			for(auto it = be; it != en; ++it){
				c += (v-it->second.d.v).squaredNorm();
			}
			return c;
		}
		std::vector<uint64_t> updateOld(expkernel_dmap_iterator be, expkernel_dmap_iterator en, double gamma){
			Eigen::VectorXd tmpv = gamma*vOld;
			double wt = gamma;
			for(auto it = be; it != en; ++it){
				tmpv += it->second.d.v;
				wt += 1.0;
			}
			vOld = tmpv / wt;
			return std::vector<uint64_t>();
		}

		double distTo(const ExpKernelData& vec, bool isActive){
			return (isActive ? (vec.v-this->v).squaredNorm() : (vec.v-this->vOld).squaredNorm());
		}
};
}

#define __EXPKERNEL_HPP
#endif /* __EXPKERNEL_HPP */
