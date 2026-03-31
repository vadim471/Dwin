#pragma once

#include <cstdint>
#include <string>

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>

#include <TypeDecimal.hpp>

namespace services::FiscalRegister::fiscal_register_interface
{

class FiscalCounters
{
    public:
    Decimal KkmRegMoneySales1;
    Decimal KkmRegMoneyRet1;
    Decimal KkmRegMoneySales2;
    Decimal KkmRegMoneyRet2;
    Decimal KkmRegMoneySales3;
    Decimal KkmRegMoneyRet3;
    Decimal KkmRegMoneySales4;
    Decimal KkmRegMoneyRet4;
    int32_t KkmRegCountSalesFiscalDocuments;
    int32_t KkmRegCountReturnFiscalDocuments;
    int32_t KkmRegCountCancelFiscalDocuments;
    int32_t TotalDocsForCurrentShift;

    template <class Archive>
    void save(Archive & ar) const
    {
        // Преобразуем Decimal → string
        std::string s1 = decimal_to_string(KkmRegMoneySales1);
        std::string s2 = decimal_to_string(KkmRegMoneyRet1);
        std::string s3 = decimal_to_string(KkmRegMoneySales2);
        std::string s4 = decimal_to_string(KkmRegMoneyRet2);
        std::string s5 = decimal_to_string(KkmRegMoneySales3);
        std::string s6 = decimal_to_string(KkmRegMoneyRet3);
        std::string s7 = decimal_to_string(KkmRegMoneySales4);
        std::string s8 = decimal_to_string(KkmRegMoneyRet4);

        ar(CEREAL_NVP(s1),
           CEREAL_NVP(s2),
           CEREAL_NVP(s3),
           CEREAL_NVP(s4),
           CEREAL_NVP(s5),
           CEREAL_NVP(s6),
           CEREAL_NVP(s7),
           CEREAL_NVP(s8),
           CEREAL_NVP(KkmRegCountSalesFiscalDocuments),
           CEREAL_NVP(KkmRegCountReturnFiscalDocuments),
           CEREAL_NVP(KkmRegCountCancelFiscalDocuments),
           CEREAL_NVP(TotalDocsForCurrentShift));
    }

    template <class Archive>
    void load(Archive & ar)
    {
        std::string s1, s2, s3, s4, s5, s6, s7, s8;

        ar(CEREAL_NVP(s1),
           CEREAL_NVP(s2),
           CEREAL_NVP(s3),
           CEREAL_NVP(s4),
           CEREAL_NVP(s5),
           CEREAL_NVP(s6),
           CEREAL_NVP(s7),
           CEREAL_NVP(s8),
           CEREAL_NVP(KkmRegCountSalesFiscalDocuments),
           CEREAL_NVP(KkmRegCountReturnFiscalDocuments),
           CEREAL_NVP(KkmRegCountCancelFiscalDocuments),
           CEREAL_NVP(TotalDocsForCurrentShift));

        KkmRegMoneySales1 = string_to_decimal(s1);
        KkmRegMoneyRet1   = string_to_decimal(s2);
        KkmRegMoneySales2 = string_to_decimal(s3);
        KkmRegMoneyRet2   = string_to_decimal(s4);
        KkmRegMoneySales3 = string_to_decimal(s5);
        KkmRegMoneyRet3   = string_to_decimal(s6);
        KkmRegMoneySales4 = string_to_decimal(s7);
        KkmRegMoneyRet4   = string_to_decimal(s8);
    }
};

}
