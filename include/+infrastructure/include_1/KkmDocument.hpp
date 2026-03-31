#ifndef ARKAIM_PAY_FISCAL_REGISTER_KKMDOCUMENT_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_KKMDOCUMENT_HPP

#include <string>
#include <memory>
#include <cstdint>

#include <DocumentType.hpp>
#include <CorrectionType.hpp>
#include <PaymentType.hpp>

#include <cereal/cereal.hpp>

namespace services::FiscalRegister::fiscal_register_interface
{

struct KkmDocument
{
public:
    DocumentType Type;
    CorrectionType Correction;
    PaymentType Payment;
    bool Fiscalize;
    bool PrintLogo;
    int HeightLogo;
    int PaymentNumber;
    std::string PaymentName;
    std::string ProductName;
    int Quantity;
    int BasePriceInKop;
    int PrevPriceInKop;
    int AmountInKop;
    int DiscountInKop;
    int CalcSignMethod;
    int CalcSignSubject;
    bool IsAdvanceAmount;
    bool IsConfirm;
    int ConfirmTax;
    int ConfirmCalcSignMethod;
    int ConfirmCalcSignSubject;
    int OSN;
    int PayTax;
    int PayCalcSignMethod;
    int PayCalcSignSubject;
    int Tax;
    int FiscalCounters;
    std::string Code;
    std::string Section;
    std::string Measure;
    std::string DocumentHeader;
    std::string Barcode;
    std::string BarcodeHeader;
    std::string BarcodeFooter;
    std::string Email;
    std::string Telephone;
    bool IsUseEmail;
    std::string DiscountCardNumber;
    std::string StringDiskount;
    bool NeedNotDiskountToSeparateString;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar( CEREAL_NVP(Type),
            CEREAL_NVP(Correction),
            CEREAL_NVP(Payment),
            CEREAL_NVP(Fiscalize),
            CEREAL_NVP(PrintLogo),
            CEREAL_NVP(HeightLogo),
            CEREAL_NVP(PaymentNumber),
            CEREAL_NVP(PaymentName),
            CEREAL_NVP(ProductName),
            CEREAL_NVP(Quantity),
            CEREAL_NVP(BasePriceInKop),
            CEREAL_NVP(PrevPriceInKop),
            CEREAL_NVP(AmountInKop),
            CEREAL_NVP(DiscountInKop),
            CEREAL_NVP(CalcSignMethod),
            CEREAL_NVP(CalcSignSubject),
            CEREAL_NVP(IsAdvanceAmount),
            CEREAL_NVP(IsConfirm),
            CEREAL_NVP(ConfirmTax),
            CEREAL_NVP(ConfirmCalcSignMethod),
            CEREAL_NVP(ConfirmCalcSignSubject),
            CEREAL_NVP(OSN),
            CEREAL_NVP(PayTax),
            CEREAL_NVP(PayCalcSignMethod),
            CEREAL_NVP(PayCalcSignSubject),
            CEREAL_NVP(Tax),
            CEREAL_NVP(FiscalCounters),
            CEREAL_NVP(Code),
            CEREAL_NVP(Section),
            CEREAL_NVP(Measure),
            CEREAL_NVP(DocumentHeader),
            CEREAL_NVP(Barcode),
            CEREAL_NVP(BarcodeHeader),
            CEREAL_NVP(BarcodeFooter),
            CEREAL_NVP(Email),
            CEREAL_NVP(Telephone),
            CEREAL_NVP(IsUseEmail),
            CEREAL_NVP(DiscountCardNumber),
            CEREAL_NVP(StringDiskount),
            CEREAL_NVP(NeedNotDiskountToSeparateString)
        );
    }
};

}

#endif