#ifndef RCPP_INTERFACE_BASE_HPP
#define  RCPP_INTERFACE_BASE_HPP

#include "../../../common/def.hpp"



#define RCPP_NO_SUGAR
#include <Rcpp.h>

/**
 * Rcpp representation of a variable
 * interface between R and cpp.
 */
class Variable {
    static uint32_t id_g;

   public:

     uint32_t id;
    static std::vector<Variable*> parameters;
    static std::vector<Variable*> estimated_parameters;
    bool estimable = FALSE;
    double value = 0;
    std::string name_m;

    Variable() {
        this->id = Variable::id_g++;
        Variable::parameters.push_back(this);
    }
    
    Variable(const Variable& other){
        this->id = other.id;
        this->estimable = other.estimable;
        this->value = other.value;
        this->name_m = other.name_m;
    }

};

uint32_t Variable::id_g = 0;

class VariableVector{
  static uint32_t id_g;
  Rcpp::List vec_m;
public:
  uint32_t id;

  VariableVector(){
    this->id = VariableVector::id_g++;
    Variable v;
    this->vec_m.push_back(Rcpp::wrap(v));
  }

  VariableVector(size_t size ){
    this->id = VariableVector::id_g++;
    for(size_t i =0; i < size; i++){
      Variable v;
      this->vec_m.push_back(Rcpp::wrap(v));
    }
  }

  inline Variable operator[](size_t pos) { return this->vec_m[pos]; }

  SEXP at(size_t pos){
    if(pos == 0 || pos > this->vec_m.size()){
      Rcpp::Rcout <<"Index out of range.\n";
      return NULL;
    }
    return this->vec_m[pos-1];
  }
    
    size_t size(){
        return this->vec_m.size();
    }


};
uint32_t VariableVector::id_g = 0;




/**
 *@brief Base class for all interface objects
 */
class RcppInterfaceBase {
public:
    static uint32_t module_id_g; /**< static id of the GrowthInterfaceBase object */
    uint32_t module_id; /**< local id of the GrowthInterfaceBase object */
    std::string r_name;

    /**< FIMS interface object vectors */
    static std::vector<RcppInterfaceBase *> interface_objects;

    RcppInterfaceBase() {
        this->module_id = RcppInterfaceBase::module_id_g++;
    }

    /** @brief virtual method to inherit to add objects to the TMB model */
    virtual bool prepare() {
        return false;
    }

    virtual std::string get_module_name() = 0;

    static void Tokenize(const std::string& str, std::vector<std::string>& tokens,
            const std::string& delimiters = " ", const bool trimEmpty = true) {
        std::string::size_type pos, lastPos = 0;
        while (true) {
            pos = str.find_first_of(delimiters, lastPos);
            if (pos == std::string::npos) {
                pos = str.size();

                if (pos != lastPos || !trimEmpty)
                    tokens.push_back(std::vector<std::string>::value_type(str.data() + lastPos,
                        (std::vector<std::string>::value_type::size_type)pos - lastPos));

                break;
            } else {
                if (pos != lastPos || !trimEmpty)
                    tokens.push_back(std::vector<std::string>::value_type(str.data() + lastPos,
                        (std::vector<std::string>::value_type::size_type)pos - lastPos));
            }

            lastPos = pos + 1;
        }
    }

    template <typename T>
    T StringToNumber(const std::string &Text) {
        std::istringstream ss(Text);
        T result;
        return (ss >> result) ? result : 0;
    }
    
    virtual void show_() {}

    bool StartsWith(const std::string &value1, const std::string &value2) {
        return value1.find(value2) == 0;
    }

    bool contains(std::string s1, std::string s2) {
        if (s1.find(s2) != std::string::npos) {
            return true;
        }
        return false;
    }

    void set_r_name() {
        Rcpp::Environment env = Rcpp::Environment::global_env();
        Rcpp::List l = Rcpp::as<Rcpp::List>(env.ls(true));
        SEXP e, E, EE;


        for (int i = 0; i < l.size(); i++) {
            std::stringstream ss;
            ss << "capture.output(show(" << Rcpp::as<std::string>(l[i]) << "))"; //, file = NULL, append = FALSE, type = c(\"output\", \"message\"), split = FALSE)";
            if (StartsWith(Rcpp::as<std::string>(l[i]), ".")) {
                continue;
            }


            SEXP expression, result;
            ParseStatus status;

            PROTECT(expression = R_ParseVector(Rf_mkString(ss.str().c_str()), 1, &status, R_NilValue));

            if (status != PARSE_OK) {
                std::cout << "Error parsing expression" << std::endl;
                UNPROTECT(1);
            }

            PROTECT(result = Rf_eval(VECTOR_ELT(expression, 0), R_GlobalEnv));

            if (TYPEOF(result) == STRSXP) {
                for (int j = 0; j < LENGTH(result); j++) {
                    std::string str(CHAR(STRING_ELT(result, j)));
           
                    if (str == this->get_module_name()) {

                        std::string line(CHAR(STRING_ELT(result, j + 1)));
                        std::vector<std::string> tokens;
                  
                        Tokenize(line, tokens, ":");
                        if (StringToNumber<size_t> (tokens[1]) == this->module_id) {
                            this->r_name = Rcpp::as<std::string>(l[i]);
                        }
                    } else {
                        break;
                    }
//                    std::cout << CHAR(STRING_ELT(result, j)) << std::endl;
                }

            }
            UNPROTECT(2);
        }




    }


};

uint32_t RcppInterfaceBase::module_id_g = 1;
std::vector<RcppInterfaceBase *>
RcppInterfaceBase::interface_objects;
std::vector<Variable*> Variable::parameters;
std::vector<Variable*> Variable::estimated_parameters;

#endif
