#ifndef ARKAIM_PAY_FISCAL_REGISTER_STATUS_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_STATUS_HPP

#include <string>
#include <itp/enum.h>

namespace services::FiscalRegister::fiscal_register_interface
{
    struct FiscalRegisterStatus
    {
        //itp::cl2_device_status_t Status;   // состояние ноды CL2_CMD_ALL_GET_STATUS
        bool IsError;
        std::string Description;             // текст ошибки, если есть
        std::string ComPort;
        bool IsComPortExist;
        bool IsComPortOpened;
        std::string PCTimestamp;             // время на ПК
        std::string FiscalRegisterTimestamp; // время на кассе
        std::string LastDocumentTimestamp;   // время последнего фискального документа
        bool IsShiftOpened;                  // состояние смены(открыта/закрыта)
        bool IsReceiptOpened;                // состояние чека(открыт/закрыт)
        int HowManyFiscalDocumentsWaitSanding;

        template <class Archive>
        void serialize(Archive &ar)
        {
            ar(//CEREAL_NVP(Status),
               CEREAL_NVP(IsError),
               CEREAL_NVP(Description),
               CEREAL_NVP(ComPort),
               CEREAL_NVP(IsComPortExist),
               CEREAL_NVP(IsComPortOpened),
               CEREAL_NVP(PCTimestamp),
               CEREAL_NVP(FiscalRegisterTimestamp),
               CEREAL_NVP(LastDocumentTimestamp),
               CEREAL_NVP(IsShiftOpened),
               CEREAL_NVP(IsReceiptOpened),
               CEREAL_NVP(HowManyFiscalDocumentsWaitSanding)
               );
        }
    };
} // namespace services::FiscalRegister::cl2

#endif