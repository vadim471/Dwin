#ifndef ARKAIM_PAY_FISCAL_REGISTER_SETTINGS_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_SETTINGS_HPP

#include <cstdint>
#include <string>

#include <cereal/cereal.hpp>

namespace services::FiscalRegister::fiscal_register_interface
{

class KkmSettings
{
public:
    std::string FiscalRegisterManufacturer;
    uint32_t  ComPortBaudRate;
    std::string ComPortName;
    uint32_t  TimeoutRetrakt;
    uint32_t  CountStepsToPush;
    bool IsThrowPrevReceipt;
    std::string Password;
    std::string AzsName;
    bool IsUsePaymentAgent;
    bool IsIsPrintAgenInfo;
    uint32_t  AgentSign;
    std::string InnPurveyor;
    std::string PhonePurveyor;
    std::string NamePurveyor;
    std::string AzsAddress;
    bool UsePresenter;
    bool SDCardRequired;
    bool UseCashPaymentInKkmOnReturn;
    bool UseFulCashPaymentFromKkmOnIncassation;
    bool EncashmentInZOrder;
    bool NotUsePaperFinishedSensor;
    uint32_t  SoonNeedFinalizeShiftTimeoutInMinutes;
    bool IsPrintPriceForDocuments;
    bool IsUseAdvancePayment;
    uint32_t  CountLineMoveAfterCutPaper;
    uint32_t  HeightLogo;
    bool IsPrintLogoAfterCutPaper;
    bool IsOnClearKkmCache;
    bool IsPrintAllReceipt;
    bool IsNoPrintReceiptIsEnterPhoneMail;
    bool FiscalShowCustomNdsString;
    int64_t FiscalGreaterHostAllowedDelta;
    int64_t HostGreaterFiscalAllowedDelta;
    bool TimeAutoSyncOnError;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(FiscalRegisterManufacturer),
           CEREAL_NVP(ComPortBaudRate),
           CEREAL_NVP(ComPortName),
           CEREAL_NVP(TimeoutRetrakt),
           CEREAL_NVP(CountStepsToPush),
           CEREAL_NVP(IsThrowPrevReceipt),
           CEREAL_NVP(Password),
           CEREAL_NVP(AzsName),
           CEREAL_NVP(IsUsePaymentAgent),
           CEREAL_NVP(IsIsPrintAgenInfo),
           CEREAL_NVP(AgentSign),
           CEREAL_NVP(InnPurveyor),
           CEREAL_NVP(PhonePurveyor),
           CEREAL_NVP(NamePurveyor),
           CEREAL_NVP(AzsAddress),
           CEREAL_NVP(UsePresenter),
           CEREAL_NVP(SDCardRequired),
           CEREAL_NVP(UseCashPaymentInKkmOnReturn),
           CEREAL_NVP(UseFulCashPaymentFromKkmOnIncassation),
           CEREAL_NVP(EncashmentInZOrder),
           CEREAL_NVP(NotUsePaperFinishedSensor),
           CEREAL_NVP(SoonNeedFinalizeShiftTimeoutInMinutes),
           CEREAL_NVP(IsPrintPriceForDocuments),
           CEREAL_NVP(IsUseAdvancePayment),
           CEREAL_NVP(CountLineMoveAfterCutPaper),
           CEREAL_NVP(HeightLogo),
           CEREAL_NVP(IsPrintLogoAfterCutPaper),
           CEREAL_NVP(IsOnClearKkmCache),
           CEREAL_NVP(IsPrintAllReceipt),
           CEREAL_NVP(IsNoPrintReceiptIsEnterPhoneMail),
           CEREAL_NVP(FiscalShowCustomNdsString),
           CEREAL_NVP(FiscalGreaterHostAllowedDelta),
           CEREAL_NVP(HostGreaterFiscalAllowedDelta),
           CEREAL_NVP(TimeAutoSyncOnError)
           );
    }
};

}

#endif //ARKAIM_PAY_FISCAL_REGISTER_SETTINGS_HPP