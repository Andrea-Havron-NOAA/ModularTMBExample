#ifndef MVNorm_NLL_HPP
#define MVNorm_NLL_HPP

#include "nll_base.hpp"
#include "../common/fims_vector.hpp"
#include "../common/def.hpp"

/**
 * Normal Negative Log-Likelihood
 */
template<typename Type>
struct MVNormNLL : public NLLBase<Type> {
    fims::Vector<Type> log_sd;
    fims::Vector<Type> logit_phi;
    
    Type nll;
    //data_indicator<tmbutils::vector<Type> , Type> keep;

    MVNormNLL() : NLLBase<Type>() {

    }

    virtual ~MVNormNLL() {}

    virtual const Type evaluate(){
        const int n = this->log_sd.size();
        Rcout << "log_sd[0] is: " << log_sd[0] << std::endl;
        Rcout << "log_sd[1] is: " << log_sd[1] << std::endl;
        Rcout << "logit_phi is: " << logit_phi[0] << std::endl;
        matrix<Type> Sigma(n,n);
        int phi_idx = 0;
        fims::Vector<Type> phi;
        phi.resize(logit_phi.size());
        for(int i=0; i<n; i++){
            Sigma(i,i) = exp(this->log_sd[i]) * exp(this->log_sd[i]);
            for(int j=0; j<i; j++){
                phi[phi_idx] = 1 / (1 + exp(- this->logit_phi[phi_idx] ))* 2 - 1;
                Rcout << "phi is: " << phi[phi_idx] << std::endl;
                Sigma(i,j) = phi[phi_idx] * exp(this->log_sd[i]) * exp(this->log_sd[j]);
                Sigma(j,i) = Sigma(i,j);
                phi_idx += 1;
            }
        }
        Rcout << "Sigma[0,0] is: " << Sigma(0,0) << std::endl;
        Rcout << "Sigma[0,1] is: " << Sigma(0,1) << std::endl;
        fims::Vector<Type> resid;
        resid.resize(this->observed_value.size());
        this->nll_vec.resize(1);
        for(int i=0; i<this->observed_value.size(); i++){
            resid[i] = this->observed_value[i] - this->expected_value[i];
        } 
        nll += density::MVNORM(Sigma)(resid);
        this->nll_vec[0] = nll;
        

        return(nll);
    }

};
#endif;