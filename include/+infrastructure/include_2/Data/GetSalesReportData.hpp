#ifndef ARKAIM_PAY_FISCAL_REGISTER_GET_SALEs_REPORT_DATA_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_GET_SALEs_REPORT_DATA_HPP

#include <cereal/access.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

namespace services::FiscalRegister::cl2::Data
{
    struct GetSalesReportData
    {
        std::string FreeField;

        template <class Archive>
        void serialize(Archive &ar)
        {
            ar(CEREAL_NVP(FreeField));
        }
    };
}

#endif