#pragma once

#include "__SymbolicExpressions.h"
#include <iostream>
#include <iomanip>

namespace sym{

    using namespace SymEngine;

    inline double round_to(double value, int decimal_places = 3) {
        const double multiplier = std::pow(10.0, decimal_places);
        return std::round(value * multiplier) / multiplier;
    }

    inline std::string double_to_string(double value, int decimal_places = 3) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(decimal_places) << value;
        return oss.str();
    }

    inline void round_subs_map(SymEngine::map_basic_basic& subs, int decimal_places = 3) {
        for(auto& [sym, sub] : subs) {

            try {
                double value = SymEngine::eval_double(*sub);
                sub = SymEngine::parse(double_to_string(round_to(value, decimal_places)));

            } catch (const SymEngine::SymEngineException& e) {

            }
        }
    }

    // Visitor der RealDoubles rundet
    class RoundingVisitor : public BaseVisitor<RoundingVisitor> {
    private:
        RCP<const Basic> result_;
        int decimal_places_;

    public:
        RoundingVisitor(int decimal_places = 3) : decimal_places_(decimal_places) {}

        RCP<const Basic> apply(const RCP<const Basic> &expr) {
            expr->accept(*this);
            return result_;
        }

        void visit(const RealDouble &x) {
            double rounded = round_to(x.as_double(), decimal_places_);
            result_ = real_double(rounded);
        }

        // Behandle andere Typen rekursiv
        void visit(const Add &x) {
            SymEngine::umap_basic_num dict;
            for (const auto &p : x.get_dict()) {
                dict[apply(p.first)] = rcp_dynamic_cast<const Number>(apply(p.second));
            }
            result_ = Add::from_dict(rcp_dynamic_cast<const Number>(apply(x.get_coef())), std::move(dict));
        }

        void visit(const Mul &x) {
            map_basic_basic dict;
            for (const auto &p : x.get_dict()) {
                dict[apply(p.first)] = apply(p.second);
            }
            result_ = Mul::from_dict(rcp_dynamic_cast<const Number>(apply(x.get_coef())), std::move(dict));
        }

        void visit(const Pow &x) {
            result_ = SymEngine::pow(apply(x.get_base()), apply(x.get_exp()));
        }

        // Standardbehandlung für alle anderen Typen
        template <typename T>
        void bvisit(const T &x) {
            result_ = x.rcp_from_this();
        }
    };

    // Einfache Funktion zum Runden aller Zahlen in einem Ausdruck
    inline RCP<const Basic> round_all_numbers(const RCP<const Basic> &expr, int decimal_places = 3) {
        RoundingVisitor visitor(decimal_places);
        return visitor.apply(expr);
    }

    inline void roundMatrix(SymEngine::DenseMatrix& mat, int decimal_places = 3) {
        RoundingVisitor visitor(decimal_places); // nur einmal erzeugen!
        for (unsigned i = 0; i < mat.nrows(); ++i) {
            for (unsigned j = 0; j < mat.ncols(); ++j) {
                mat.set(i, j, visitor.apply(mat.get(i, j)));
            }
        }
    }
}