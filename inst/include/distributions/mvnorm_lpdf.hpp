#ifndef MVNORM_LPDF_HPP
#define MVNORM_LPDF_HPP

#include "density_components_base.hpp"
#include "../common/fims_vector.hpp"
#include "../common/def.hpp"

/**
 * Normal Negative Log-Likelihood
 */
template<typename Type>
struct MVNormLPDF : public DensityComponentBase<Type> {
    fims::Vector<Type> log_sd;
    fims::Vector<Type> logit_phi;
    bool osa_flag;
    
    Type log_likelihood;
    //data_indicator<tmbutils::vector<Type> , Type> keep;

    MVNormLPDF() : DensityComponentBase<Type>() {

    }

    virtual ~MVNormLPDF() {}

    virtual const Type evaluate(){
        const int n = this->log_sd.size();
        matrix<Type> Sigma(n,n);
        int phi_idx = 0;
        fims::Vector<Type> phi;
        phi.resize(logit_phi.size());
        for(int i=0; i<n; i++){
            Sigma(i,i) = exp(this->log_sd[i]) * exp(this->log_sd[i]);
            for(int j=0; j<i; j++){
                phi[phi_idx] = 1 / (1 + exp(- this->logit_phi[phi_idx] ))* 2 - 1;
                Sigma(i,j) = phi[phi_idx] * exp(this->log_sd[i]) * exp(this->log_sd[j]);
                Sigma(j,i) = Sigma(i,j);
                phi_idx += 1;
            }
        }
        fims::Vector<Type> resid;
        resid.resize(this->observed_value.size());
        this->log_likelihood_vec.resize(1);
        for(int i=0; i<this->observed_value.size(); i++){
            resid[i] = this->observed_value[i] - this->expected_value[i];
        } 
        log_likelihood -= density::MVNORM(Sigma)(resid); //MVNORN returns neg-log-likelihood
        this->log_likelihood_vec[0] = log_likelihood;
        

        return(log_likelihood);
    }

};
#endif;