#ifndef __EXPKERNELMODEL_HPP

#include <Eigen/Dense>
#include "../core/cluster.hpp"
#include "../util/sparsevectorapprox.hpp"
namespace dmeans{
template <int n>
class ExponentialKernelModel{
	public:
		double lambda, Q, tau, omega, spEps;
		uint64_t spK;
		ExponentialKernelModel(Config cfg){
			this->lambda = cfg.get("lambda", Config::REQUIRED, -1.0);
			this->Q = cfg.get("Q", Config::REQUIRED, -1.0);
			this->tau = cfg.get("tau", Config::REQUIRED, -1.0);
			this->omega = cfg.get("kernelWidth", Config::REQUIRED, -1.0);
			this->spK = cfg.get("sparseApproximationSize", Config::REQUIRED, -1);
			this->spEps = cfg.get("sparseApproximationErrorThreshold", Config::OPTIONAL, 1.0e-4);
		}
		class Data{
			public:
				Data(){v.setZero();}
				Eigen::Matrix<double, n, 1> v;
		};
		class Parameter{
			public:
				Parameter(){vs.clear(); w = 0;}
				std::vector< Eigen::Matrix<double, n, 1> > vs;
				std::vector< double > coeffs;
				double w;
		};

		bool isClusterDead(double age) const{
			return Q*age > lambda;
		}

		typedef typename std::map<uint64_t, Data>::const_iterator dmap_iterator; 

		bool exceedsNewClusterCost(const Data& d, double cost) const{
			return cost > lambda;
		}

		double getEigenvalueLowerThreshold() const{
			return lambda;
		}

		double oldWeight(const Cluster<Data, Parameter>& c) const {
			return 1.0/(1.0/c.getOldPrm().w +tau*c.getAge()); 
		}

		double kernelDD(const Data& d1, const Data& d2) const{
			return exp( -(d1.v-d2.v).squaredNorm()/(2*omega*omega) );
		}

		double kernelDOldP(const Data& d, const Cluster<Data, Parameter>& c) const{
			double kern = 0.0;
			for (int i = 0; i < c.getOldPrm().vs.size(); i++){
				kern += c.getOldPrm().coeffs[i]*exp(-(d.v-c.getOldPrm().vs[i]).squaredNorm()/(2*omega*omega) );
			}
			return kern;
		}

		double kernelDP(const Data& d, const Cluster<Data, Parameter>& c) const{
			double kern = 0.0;
			for (int i = 0; i < c.getPrm().vs.size(); i++){
				kern += c.getPrm().coeffs[i]*exp(-(d.v-c.getPrm().vs[i]).squaredNorm()/(2*omega*omega) );
			}
			return kern;
		}

		double kernelOldPOldP(const Cluster<Data, Parameter>& c) const{
			double kern = 0.0;
			for (int i = 0; i < c.getOldPrm().vs.size(); i++){
				for (int j = 0; j < c.getOldPrm().vs.size(); j++){
					kern += c.getOldPrm().coeffs[i]*c.getOldPrm().coeffs[j]*exp(-(c.getOldPrm().vs[j]-c.getOldPrm().vs[i]).squaredNorm()/(2*omega*omega) );
				}
			}
			return kern;
		}

		double kernelPP(const Cluster<Data, Parameter>& c) const{
			double kern = 0.0;
			for (int i = 0; i < c.getPrm().vs.size(); i++){
				for (int j = 0; j < c.getPrm().vs.size(); j++){
					kern += c.getPrm().coeffs[i]*c.getPrm().coeffs[j]*exp(-(c.getPrm().vs[j]-c.getPrm().vs[i]).squaredNorm()/(2*omega*omega) );
				}
			}
			return kern;
		}

		double kernelPOldP(const Cluster<Data, Parameter>& c) const{
			double kern = 0.0;
			for (int i = 0; i < c.getPrm().vs.size(); i++){
				for (int j = 0; j < c.getOldPrm().vs.size(); j++){
					kern += c.getPrm().coeffs[i]*c.getOldPrm().coeffs[j]*exp(-(c.getPrm().vs[i]-c.getOldPrm().vs[j]).squaredNorm()/(2*omega*omega) );
				}
			}
			return kern;
		}

		double clusterCost(const Cluster<Data, Parameter>& c) const{
			if (c.isEmpty()){
				return 0.0;
			}
			double cost = 0.0;
			if (c.isNew()){
				cost += lambda;
			} else {
				double age = c.getAge();
				double gamma = 1.0/(1.0/c.getOldPrm().w +tau*age); //cluster old penalty
				cost += Q*age+gamma*(kernelPP(c) -2*kernelPOldP(c) +kernelOldPOldP(c));
			}
			for(auto it = c.data_cbegin(); it != c.data_cend(); ++it){
				cost += kernelDD(it->second, it->second) -2*kernelDP(it->second, c) + kernelPP(c);
			}
			return cost;
		}

		void updatePrm(Cluster<Data, Parameter>& c) const{
			if (c.isEmpty()){
				c.getPrmRef() = c.getOldPrmRef();
				return;
			} else {
				double age = c.getAge();
				double gamma = 1.0/(1.0/c.getOldPrm().w +tau*c.getAge()); 
				uint64_t N = c.getAssignedIds().size();

				std::vector< Eigen::Matrix<double, n, 1> > fullvs;
				std::vector< double > fullcoeffs;
				for (int i=0; i < c.getOldPrm().vs.size(); i++) {
					fullvs.push_back( c.getOldPrm().vs[i] );
					fullcoeffs.push_back( c.getOldPrm().coeffs[i]*gamma/(gamma+N) );
				}
				for (auto it = c.data_cbegin(); it != c.data_cend(); ++it){
					fullvs.push_back(it->second.v);
					fullcoeffs.push_back(1.0/(gamma+N));
				}

				SparseVectorApproximation spa(spK, spEps);
				MXd kmat = MXd::Zero(fullvs.size(), fullvs.size());
				VXd coeffvec = VXd::Zero(fullvs.size());
				for (int i = 0; i < fullvs.size(); i++){
					for (int j = 0; j <= i; j++){
						kmat(i, j) = kmat(j, i) = exp( -(fullvs[i]-fullvs[j]).squaredNorm()/(2*omega*omega) );
					}
					coeffvec(i) = fullcoeffs[i];
				}
				spa.fromKernelMatrix(kmat, coeffvec);
				std::vector<uint64_t> approxvecs;
				std::vector<double> approxcoeffs;
				spa.getApprox(approxvecs, approxcoeffs);

				c.getPrmRef().vs.clear();
				c.getPrmREf().coeffs.clear();
				for(int i = 0; i < approxvecs.size(); i++){
					c.getPrmRef().vs.push_back(fullvs[approxvecs[i]]);
					c.getPrmRef().coeffs.push_back(approxcoeffs[i]);
				}

				c.getPrmRef().w = gamma+N;
			}
		}

		double compare(const Cluster<Data, Parameter>& c, const Data& d) const{
			if (c.isEmpty()){
				double age = c.getAge();
				double gamma = 1.0/(1.0/c.getOldPrm().w +tau*age);
				return Q*age+gamma/(gamma+1.0)*(kernelDD(d, d) - 2*kernelDOldP(d, c) + kernelOldPOldP(c));
			} else {
				return kernelDD(d, d) - 2*kernelDP(d, c) + kernelPP(c);
			}
		}

};
}

#define __EXPKERNELMODEL_HPP
#endif /* __EXPKERNELMODEL_HPP */