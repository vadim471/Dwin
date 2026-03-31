#ifndef ARKAIM_PAY_FISCAL_REGISTER_CORRECTION_DATA_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_CORRECTION_DATA_HPP

#include <cereal/access.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>

#include <TypeDecimal.hpp>

namespace services::FiscalRegister::cl2::Data
{
    
struct CorrectionData
{
    std::string NameCorrection;
    time_t DateCorrection;
    std::string NumberDocs;
    uint8_t AttrCorrection;
    uint8_t TypeCorrection;
    Decimal AmountCorrection;
    Decimal AmountCash;
    Decimal AmountBanc;
    Decimal AmountPrePayment;
    Decimal AmountAfterPayment;
    Decimal AmountBuiltIn;
    uint8_t Tax;
    Decimal AmountNds10;
    Decimal AmountNds18;
    Decimal AmountRate0;
    Decimal AmountWithoutNds;
    Decimal AmountNds18_118;
    Decimal AmountNds10_110;

    template <class Archive>
    void save(Archive & ar) const
    {
        std::string amountCorrection   = decimal_to_string(AmountCorrection);
        std::string amountCash         = decimal_to_string(AmountCash);
        std::string amountBanc         = decimal_to_string(AmountBanc);
        std::string amountPrePayment   = decimal_to_string(AmountPrePayment);
        std::string amountAfterPayment = decimal_to_string(AmountAfterPayment);
        std::string amountBuiltIn      = decimal_to_string(AmountBuiltIn);
        std::string amountNds10        = decimal_to_string(AmountNds10);
        std::string amountNds18        = decimal_to_string(AmountNds18);
        std::string amountRate0        = decimal_to_string(AmountRate0);
        std::string amountWithoutNds   = decimal_to_string(AmountWithoutNds);
        std::string amountNds18_118    = decimal_to_string(AmountNds18_118);
        std::string amountNds10_110    = decimal_to_string(AmountNds10_110);

        ar( CEREAL_NVP(NameCorrection),
            CEREAL_NVP(DateCorrection),
            CEREAL_NVP(NumberDocs),
            CEREAL_NVP(AttrCorrection),
            CEREAL_NVP(TypeCorrection),
            CEREAL_NVP(amountCorrection),
            CEREAL_NVP(amountCash),
            CEREAL_NVP(amountBanc),
            CEREAL_NVP(amountPrePayment),
            CEREAL_NVP(amountAfterPayment),
            CEREAL_NVP(amountBuiltIn),
            CEREAL_NVP(Tax),
            CEREAL_NVP(amountNds10),
            CEREAL_NVP(amountNds18),
            CEREAL_NVP(amountRate0),
            CEREAL_NVP(amountWithoutNds),
            CEREAL_NVP(amountNds18_118),
            CEREAL_NVP(amountNds10_110));
    }

    template <class Archive>
    void load(Archive & ar)
    {
        std::string amountCorrection;
        std::string amountCash;
        std::string amountBanc;
        std::string amountPrePayment;
        std::string amountAfterPayment;
        std::string amountBuiltIn;
        std::string amountNds10;
        std::string amountNds18;
        std::string amountRate0;
        std::string amountWithoutNds;
        std::string amountNds18_118;
        std::string amountNds10_110;

        ar(
            CEREAL_NVP(NameCorrection),
            CEREAL_NVP(DateCorrection),
            CEREAL_NVP(NumberDocs),
            CEREAL_NVP(AttrCorrection),
            CEREAL_NVP(TypeCorrection),
            CEREAL_NVP(amountCorrection),
            CEREAL_NVP(amountCash),
            CEREAL_NVP(amountBanc),
            CEREAL_NVP(amountPrePayment),
            CEREAL_NVP(amountAfterPayment),
            CEREAL_NVP(amountBuiltIn),
            CEREAL_NVP(Tax),
            CEREAL_NVP(amountNds10),
            CEREAL_NVP(amountNds18),
            CEREAL_NVP(amountRate0),
            CEREAL_NVP(amountWithoutNds),
            CEREAL_NVP(amountNds18_118),
            CEREAL_NVP(amountNds10_110)
        );

        AmountCorrection   = string_to_decimal(amountCorrection);
        AmountCash         = string_to_decimal(amountCash);
        AmountBanc         = string_to_decimal(amountBanc);
        AmountPrePayment   = string_to_decimal(amountPrePayment);
        AmountAfterPayment = string_to_decimal(amountAfterPayment);
        AmountBuiltIn      = string_to_decimal(amountBuiltIn);
        AmountNds10        = string_to_decimal(amountNds10);
        AmountNds18        = string_to_decimal(amountNds18);
        AmountRate0        = string_to_decimal(amountRate0);
        AmountWithoutNds   = string_to_decimal(amountWithoutNds);
        AmountNds18_118    = string_to_decimal(amountNds18_118);
        AmountNds10_110    = string_to_decimal(amountNds10_110);
    }

};

}

#endif