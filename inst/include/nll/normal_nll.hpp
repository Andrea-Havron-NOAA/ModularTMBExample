#ifndef Normal_NLL_HPP
#define Normal_NLL_HPP

#include "nll_base.hpp"
#include "../common/fims_vector.hpp"
#include "../common/def.hpp"

/**
 * Normal Negative Log-Likelihood
 */
template<typename Type>
struct NormalNLL : public NLLBase<Type> {
    //need: dnorm(Type x, Type eta, Type sd)
    //      dnorm(fims::Vector<Type> x, Type eta, Type sd)
    //      dnorm(fims::Vector<Type> x, fims::Vector<Type> eta, Type sd)
    //      dnorm(fims::Vector<Type> x, fims::Vector<Type> eta, fims::Vector<Type> sd)
    fims::Vector<Type> log_sd;
    fims::Vector<Type> mu;
    fims::Vector<Type> sd;
    Type nll = 0.0;
    bool osa_flag;
    bool simulate_prior_flag;
    bool simulate_data_flag;
    //data_indicator<tmbutils::vector<Type> , Type> keep;
    ::objective_function<Type>
      *of;  

    NormalNLL() : NLLBase<Type>() {

    }

    virtual ~NormalNLL() {}

    virtual const Type evaluate(){
        this->mu.resize(this->observed_value.size());
        this->sd.resize(this->observed_value.size());
        for(int i=0; i<this->expected_value.size(); i++){
            if(this->expected_value.size() == 1){
                this->mu[i] = this->expected_value[0];
            } else {
                this->mu[i] = this->expected_value[i];
            }
       
            if(log_sd.size() == 1){
                sd[i] = exp(log_sd[0]);
            } else {
                sd[i] = exp(log_sd[i]);
            }
        }
        for(int i=0; i<this->observed_value.size(); i++){
      //      nll -= keep[i] * dnorm(x[i], eta[i], sd[i], true);
            nll -= dnorm(this->observed_value[i], mu[i], sd[i], true);
            /*
            if(osa_flag){//data observation type implements osa residuals
                //code for osa cdf method
                nll -= keep.cdf_lower[i] * log( pnorm(x[i], eta[i], sd[i]) );
                nll -= keep.cdf_upper[i] * log( 1.0 - pnorm(x[i], eta[i], sd[i]) );
            }
            */
        }
        return(nll);
    }

};
#endif;