#ifndef __CLUSTER_IMPL_HPP

template<class D, class P>
Cluster<D, P>::Cluster(){
	this->age = 0;
	this->w = 0.0;
	this->gamma = 0.0;
	this->id_ = nextId++;
}


template<class D, class P>
Cluster<D, P>::Cluster(const Cluster<D, P>& rhs) {
	this->age = rhs.age;
	this->w = rhs.w;
	this->gamma = rhs.gamma;
	this->prm = rhs.prm;
	this->clusData = rhs.clusData;
	this->id_ = rhs.id_;
}


template<class D, class P>
Cluster<D, P>& Cluster<D, P>::operator=(const Cluster<D, P>& rhs) {
	if (this != &rhs){
		this->age = rhs.age;
		this->w = rhs.w;
		this->gamma = rhs.gamma;
		this->prm = rhs.prm;
		this->clusData = rhs.clusData;
		this->id_ = rhs.id_;
	}
	return *this;
}

template<class D, class P>
uint64_t Cluster<D, P>::id() const{
	return this->id_;
}



template<class D, class P>
void Cluster<D, P>::updatePrm(){
	this->prm.update(this->clusData.begin(), this->clusData.end(), this->gamma);
}

template<class D, class P>
void Cluster<D, P>::finalize(double tau){
	if(this->isEmpty()){
		this->age++;
	} else {
		this->prm.updateOld(this->clusData.begin(), this->clusData.end(), this->gamma);
		this->w = this->gamma + std::distance(this->clusData.begin(), this->clusData.end());
		this->age = 1;
	}
	this->gamma = 1.0/(1.0/this->w + tau*this->age);
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
		throw DataAlreadyInClusterException(this->id_, did);
	}
	this->clusData[did] = d;
}

template<class D, class P>
D Cluster<D, P>::deassignData(uint64_t did){
	if (this->clusData.find(did) == this->clusData.end()){
		throw DataNotInClusterException(this->id_, did); 
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
	if (this->isEmpty()){
		throw ClusterEmptyDistanceException(this->id_);
	}
	return this->prm.distTo(d);
}

template<class D, class P>
double Cluster<D, P>::distToOld(const D& d) const{
	return this->prm.distToOld(d);
}

template<class D, class P>
double Cluster<D, P>::cost(double lambda, double Q) const{
	return this->isEmpty() ? 0.0 :
		(this->age == 0 ? lambda : Q*this->age)
		+this->prm.cost(this->clusData.begin(), this->clusData.end(), this->gamma);
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
