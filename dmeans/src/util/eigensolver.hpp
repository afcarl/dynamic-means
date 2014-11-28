#ifndef __EIGENSOLVER_HPP
#include<Eigen/Dense>
#include<iostream>
#include "random.hpp"


namespace dmeans{
typedef Eigen::MatrixXd MXd;
typedef Eigen::VectorXd VXd;

class EigenSolver{
	public:
		enum Type{
			EIGEN_SELF_ADJOINT,
			REDSVD
		};
		class EigenSolverTypeNotFoundException{
			public:
				EigenSolverTypeNotFoundException(int id){
					std::cout << "No Eigensolver type for id = " << id << std::endl;
				}
		};

		EigenSolver(MXd& A_UpperTriangle, Type t, uint64_t nEigs = 0, double lowerThresh=-1);
		void redsvdSolver(MXd& AUp, uint64_t r);
		void selfadjointSolver(MXd& A_Up);
		void getResults(VXd& eigvals, MXd& eigvecs);
	private:
		MXd eigvecs;
		VXd eigvals;
		void columnGramSchmidt(MXd& m);
		void pruneSmallEigs(double thresh);
		class MatrixNotSquareException{
			public:
				MatrixNotSquareException(uint64_t rows, uint64_t cols){
					std::cout << "Matrix input to EigenSolver nonsquare: rows = " << rows << " cols = " << cols << std::endl;
				}
		};
};

#include "eigensolver_impl.hpp"
}

#define __EIGENSOLVER_HPP
#endif /* __EIGENSOLVER_HPP */
