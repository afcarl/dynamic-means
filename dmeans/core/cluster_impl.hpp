#ifndef __CLUSTER_IMPL_HPP

template<class D, class P>
Cluster<D, P>::Cluster() : id(nextId++){
	this->age = 0;
	this->w = 0.0;
	this->gamma = 0.0;
}

template<class D, class P>
void Cluster<D, P>::updatePrm(){
	this->prm.update(this->clusData.begin(), this->clusData.end(), this->gamma);
}

template<class D, class P>
void Cluster<D, P>::finalize(double tau){
	if(this->clusData.empty()){
		this->age++;
	} else {
		this->prm.updateOld(this->clusData.begin(), this->clusData.end(), this->gamma);
		this->w = this->gamma + std::distance(this->clusData.begin(), this->clusData.end());
		this->age = 1;
	}
	this->gamma = 1.0/(1.0/this->w + this->tau*this->age);
	this->clusData.clear();
}

template<class D, class P>
std::vector<uint64_t> Cluster<D, P>::getAssignedIds() const{
	std::vector<uint64_t> asids;
	for (auto it = this->clusData.begin(); it != this->clusData.end(); ++it){
		asids.push_back(it->first);
	}
	return asids;
}

template<class D, class P>
void Cluster<D, P>::assignData(uint64_t did, D& d){
	if (this->clusData.find(did) != this->clusData.end()){
		throw DataAlreadyInClusterException(this->id, did);
	}
	this->clusData[did] = d;
}

template<class D, class P>
D Cluster<D, P>::deassignData(uint64_t did){
	if (this->clusData.find(did) == this->clusData.end()){
			throw DataNotInClusterException(this->id, did); 
	}
	D d = this->clusData[did];
	this->clusData.erase(did);
	return d;
}

template<class D, class P>
void Cluster<D, P>::clearData(){
	this->clusData.clear();
}

template<class D, class P>
double Cluster<D, P>::distTo(const D& d) const{
	return this->prm.distTo(d, !this->clusData.empty());
}

template<class D, class P>
double Cluster<D, P>::cost(double lambda, double Q) const{
	return this->clusData.empty() ? 0.0 : (this->age == 0 ? lambda : Q*this->age) 
		+ this->prm.cost(this->clusData.begin(), this->clusData.end(), this->gamma);
}

template<class D, class P>
bool Cluster<D, P>::isEmpty() const{
	return this->clusData.empty();
}

template<class D, class P>
bool Cluster<D, P>::isNew() const{
	return this->age == 0;
}

template<class D, class P>
P Cluster<D, P>::getPrm() const{
	return this->prm;
}

#define __CLUSTER_IMPL_HPP
#endif /* __CLUSTER_IMPL_HPP */