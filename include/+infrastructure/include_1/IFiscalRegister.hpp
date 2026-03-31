#ifndef ARKAIM_PAY_FISCAL_REGISTER_I_FISCAL_REGISTER_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_I_FISCAL_REGISTER_HPP

#include <vector>
#include <string>

#include <IDevice.hpp>
#include <KkmSettings.hpp>
#include <XreportData.hpp>
#include <KkmDocument.hpp>
#include <ExtendedResponse.hpp>
#include <FiscalCounters.hpp>
#include <FiscalRegisterStatus.hpp>
#include <TypeDecimal.hpp>

//#include <itp/enum.h>

#include <DeviceInfoData.hpp>
#include <DeviceState.hpp>

namespace services::FiscalRegister::fiscal_register_interface
{

    class IFiscalRegister : public IDevice<KkmSettings>
    {
    public:
        virtual bool OpenPort() = 0;
        virtual bool ClosePort() = 0;

        virtual bool OpenShift() = 0;
        virtual bool PrePrintDocument() = 0;
        virtual bool PrintTicket(std::vector<std::string> strings) = 0;
        virtual bool PrintDocument(KkmDocument kkmDocument, bool silent=false) = 0;
        virtual std::string GetQrCodeLastPrintDocument() = 0;
        virtual bool RemoveReceipt(bool forceMode) = 0;
        virtual bool SyncDateTime() = 0;

        virtual bool AddCashToKkm(Decimal cashSum = -1) = 0;
        virtual bool Incassation(Decimal incassationSum = -1) = 0;

        virtual bool CloseShift() = 0;
        virtual int CloseShiftToMemory() = 0;

        virtual ExtendedResponse<FiscalCounters> GetFiscalCounters(bool isShowLog = true) = 0;
        virtual XreportData GetShiftSalesInfo(int terminalNumber, time_t startshift, bool isEndOfShift = false) = 0;
        virtual std::vector<std::string> GetSalesReport(std::string freeField) = 0;
        virtual int GetZOrderFreeMemory() = 0;

        virtual void EnablePrint() = 0;
        virtual void DisablePrint() = 0;
        //virtual void EnablePrintFromClient() = 0;
        //virtual void DisablePrintFromClient() = 0;

        virtual bool PrintSalesReport(std::string freeField) = 0;
        virtual bool PrintXReport() = 0;
        virtual int PrintZReportFromMemory() = 0;

        virtual bool Correction(std::string nameCorrection,
            time_t dateCorrection,
            std::string numberDocs,
            uint8_t typeCorrection,
            uint8_t attrCorrection,
            Decimal amountCorrection,
            Decimal amountCash,
            Decimal amountBanc,
            Decimal amountPrePayment,
            Decimal amountAfterPayment,
            Decimal amountBuiltIn,
            uint8_t tax,
            Decimal amountNds10,
            Decimal amountNds18,
            Decimal amountRate0,
            Decimal amountWithoutNds,
            Decimal amountNds18_118,
            Decimal amountNds10_110) = 0;

        virtual services::FiscalRegister::fiscal_register_interface::FiscalRegisterStatus GetStatus() = 0;

        /// <summary>
        ///     (Метод класса IDevice) Возвращает состояние устройства
        /// </summary>
        virtual services::FiscalRegister::fiscal_register_interface::DeviceState GetDeviceState() override = 0;

        /// <summary>
        ///     (Метод класса IDevice) Получить статус устройства
        /// </summary>
        /// <returns></returns>
        virtual services::FiscalRegister::fiscal_register_interface::DeviceInfoData GetDeviceInfo() override = 0;

        /// <summary>
        ///     (Метод класса IDevice) Установка настроек для ККТ
        /// </summary>
        /// <param name="data"></param>
        virtual void Setup(services::FiscalRegister::fiscal_register_interface::KkmSettings data) override = 0;

        // virtual void SetNodeStatus(itp::cl2_device_status_t nodeStatus) override = 0;

        // virtual itp::cl2_device_status_t GetNodeStatus() override = 0;
    };

}

#endif