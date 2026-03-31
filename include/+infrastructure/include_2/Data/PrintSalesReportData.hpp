#ifndef ARKAIM_PAY_FISCAL_REGISTER_PRINT_SALES_REPORT_DATA_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_PRINT_SALES_REPORT_DATA_HPP

#include <cereal/access.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

namespace services::FiscalRegister::cl2::Data
{
    struct PrintSalesReportData
    {
        std::string SalesReport;

        template <class Archive>
        void serialize(Archive &ar)
        {
            ar(CEREAL_NVP(SalesReport));
        }
    };
}

#endif