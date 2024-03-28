#ifndef Normal_NLL_HPP
#define Normal_NLL_HPP

#include "nll_base.hpp"
#include "../common/fims_vector.hpp"

/**
 * Normal Negative Log-Likelihood
 */
template<typename Type>
struct NormalNLL : public NLLBase<Type> {
    //need: dnorm(Type x, Type eta, Type sd)
    //      dnorm(fims::Vector<Type> x, Type eta, Type sd)
    //      dnorm(fims::Vector<Type> x, fims::Vector<Type> eta, Type sd)
    //      dnorm(fims::Vector<Type> x, fims::Vector<Type> eta, fims::Vector<Type> sd)
    fims::Vector<Type> x;
    fims::Vector<Type> mu;
    fims::Vector<Type> log_sd;
    fims::Vector<Type> eta;
    fims::Vector<Type> sd;
    Type nll = 0.0;
    bool osa_flag;
    bool simulate_prior_flag;
    bool simulate_data_flag;
    //data_indicator<tmbutils::vector<Type> , Type> keep;
    ::objective_function<Type>
      *of;  

    NormalNLL() : NLLBase<Type>() {}

    virtual ~NormalNLL() {}

    virtual const Type evaluate(){
        eta.resize(this->x.size());
        sd.resize(this->x.size());
        if(mu.size() == 1){
            std::fill(eta.begin(), eta.end(), mu[0]);
        } else {
            eta = mu;
        }
        if(log_sd.size() == 1){
            std::fill(log_sd.begin(), log_sd.end(), exp(log_sd[0]));
        } else {
            sd = exp(log_sd);
        }
        for(int i=0; i<x.size(); i++){
      //      nll -= keep[i] * dnorm(x[i], eta[i], sd[i], true);
            nll -= dnorm(x[i], eta[i], sd[i], true);
            /*
            if(osa_flag){//data observation type implements osa residuals
                //code for osa cdf method
                nll -= keep.cdf_lower[i] * log( pnorm(x[i], eta[i], sd[i]) );
                nll -= keep.cdf_upper[i] * log( 1.0 - pnorm(x[i], eta[i], sd[i]) );
            }
            */
        }
    }

};
#endif;