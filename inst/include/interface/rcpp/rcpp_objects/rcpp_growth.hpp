#ifndef RCPP_GROWTH_HPP
#define  RCPP_GROWTH_HPP


#include <sstream>
#include "../../../common/model.hpp"
#include "rcpp_interface_base.hpp"
#include "../../../pop_dy/von_bertalanffy.hpp"

/****************************************************************
 * Growth Rcpp interface                                   *
 ***************************************************************/

/**
 * @brief Rcpp interface that serves as the parent class for
 * Rcpp growth interfaces. This type should be inherited and not
 * called from R directly.
 *
 */
class GrowthInterfaceBase : public RcppInterfaceBase {
public:
    static uint32_t id_g; /**< static id of the GrowthInterfaceBase object */
    uint32_t id; /**< local id of the GrowthInterfaceBase object */
    static std::map<uint32_t, GrowthInterfaceBase*> growth_objects; /**<
    map relating the ID of the GrowthInterfaceBase to the GrowthInterfaceBase
    objects */
    uint32_t module_id;

    GrowthInterfaceBase() {
        this->id = GrowthInterfaceBase::id_g++;
        GrowthInterfaceBase::growth_objects[this->id] = this;
        RcppInterfaceBase::interface_objects.push_back(this);
    }

    virtual ~GrowthInterfaceBase() {
    }

    virtual uint32_t get_id() = 0;
};

uint32_t GrowthInterfaceBase::id_g = 1;
std::map<uint32_t, GrowthInterfaceBase*> GrowthInterfaceBase::growth_objects;

class vonBertalanffyInterface : public GrowthInterfaceBase {
public:
    Variable k;
    Variable l_inf;
    Variable a_min;
    Variable alpha;
    Variable beta;

    vonBertalanffyInterface() : GrowthInterfaceBase() {
    }

    virtual ~vonBertalanffyInterface() {
    }

    virtual uint32_t get_id() {
        return this->id;
    }

    virtual std::string get_module_name() {
        return "vonBertalanffyInterface";
    }

    template<typename Type>
    bool prepare_local() {
        std::shared_ptr<Information<Type> > info =
                Information<Type>::getInstance();

        std::shared_ptr<Model<Type> > model = Model<Type>::getInstance();
        std::shared_ptr< VonBertalanffy<Type> > vb =
                std::make_shared<VonBertalanffy<Type> >();

        this->set_r_name();

        std::stringstream ss;

        ss << "growth_" << this->get_module_name() << "_" << this->get_id() << "_" << this->r_name;

        std::string key = ss.str();
        ss.str("");
        //initialize k
        vb->k.resize(1);
        vb->k[0] = this->k.value;


        if (this->k.estimable) {
            ss << key << "_k";
            model->parameters.push_back(&(vb)->k[0]);
            model->info->variable_map[ss.str()] = &(vb)->k[0];
            ss.str("");
        }

        //initialize l_inf
        vb->l_inf.resize(1);
        vb->l_inf[0] = this->l_inf.value;

        if (this->l_inf.estimable) {
            ss << key << "_l_inf";
            model->parameters.push_back(&(vb)->l_inf[0]);
            model->info->variable_map[ss.str()] = &(vb)->l_inf[0];
            Rcpp::Rcout << ss.str() << std::endl;
            ss.str("");
        }

        //initialize a_min
        vb->a_min.resize(1);
        vb->a_min[0] = this->a_min.value;

        if (this->a_min.estimable) {
            ss << key << "_a_min";
            model->parameters.push_back(&(vb)->a_min[0]);
            model->info->variable_map[ss.str()] = &(vb)->a_min[0];
            Rcpp::Rcout << ss.str() << std::endl;
            ss.str("");
        }

        //initialize alpha
        vb->alpha.resize(1);
        vb->alpha[0] = this->alpha.value;

        if (this->alpha.estimable) {
            ss << key << "_alpha";
            model->parameters.push_back(&(vb)->alpha[0]);
            model->info->variable_map[ss.str()] = &(vb)->alpha[0];
            Rcpp::Rcout << ss.str() << std::endl;
            ss.str("");
        }
        //initialize beta
        vb->beta.resize(1);
        vb->beta[0] = this->beta.value;

        if (this->beta.estimable) {
            ss << key << "_beta";
            model->parameters.push_back(&(vb)->beta[0]);
            model->info->variable_map[ss.str()] = &(vb)->beta[0];
            Rcpp::Rcout << ss.str() << std::endl;
            ss.str("");
        }








        model->vb = vb;
        info->vb_models[vb->id] = vb;
        return true;

    }

    /**
     * Prepares the model to work with TMB.
     */
    virtual bool prepare() {

#ifdef TMB_MODEL
        this->prepare_local<TMB_FIMS_REAL_TYPE>();
        this->prepare_local<TMB_FIMS_FIRST_ORDER>();
        this->prepare_local<TMB_FIMS_SECOND_ORDER>();
        this->prepare_local<TMB_FIMS_THIRD_ORDER>();

#endif
        return true;

    }

    /**
     * Update the model parameter values and finalize. Sets the parameter values and evaluates the
     * portable model once and transfers values back to the Rcpp interface.
     */
    void finalize(Rcpp::NumericVector v) {
        std::shared_ptr< Model<double> > model = Model<double>::getInstance();
        //std::shared_ptr< VonBertalanffy<double> > vb;
        //vb = std::make_shared<VonBertalanffy<double> >();


        for (int i = 0; i < v.size(); i++) {
            (*model->parameters[i]) = v[i];
        }

        double f = model->evaluate();

        this->k.value = model->vb->k[0];
        this->a_min.value = model->vb->a_min[0];
        this->l_inf.value = model->vb->l_inf[0];


    }

    /**
     * Print model values.
     */
    void show_() {
        Rcpp::Rcout << "vonBertalanffy:\n";
        Rcpp::Rcout << "k = " << this->k.value << "\n";
        Rcpp::Rcout << "a_min = " << this->a_min.value << "\n";
        Rcpp::Rcout << "l_inf = " << this->l_inf.value << "\n";
    }

};

#endif