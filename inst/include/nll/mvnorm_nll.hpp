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
    matrix<Type> Sigma;
    Type nll;
    bool osa_flag;
    bool simulate_prior_flag;
    bool simulate_data_flag;
    //data_indicator<tmbutils::vector<Type> , Type> keep;
    ::objective_function<Type>
      *of;  

    MVNormNLL() : NLLBase<Type>() {

    }

    virtual ~MVNormNLL() {}

    virtual const Type evaluate(){
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