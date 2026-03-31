#ifndef ARKAIM_PAY_FISCAL_REGISTER_XREPORTDATA_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_XREPORTDATA_HPP

#include <string>
#include <vector>
#include <map>

#include <cereal/access.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>

#include <TypeDecimal.hpp>

namespace services::FiscalRegister::fiscal_register_interface
{

class XreportData
{
private:
    bool isEndOfShift;
    bool encashmentInZOrder;

public:
    std::string Name1;
    std::string Name2;
    int ZreportNumber;
    time_t StartShiftDateTime;
    std::string KkmSerialNumber;
    int TerminalNumber;
    std::vector<std::string> PayTypeNames;
    std::map<std::string, Decimal> PayTypeSalesAmount;
    std::map<std::string, Decimal> PayTypeRefundAmount;
    int TotalSalesCount;
    Decimal TotalSalesAmount;
    int TotalRefundCount;
    Decimal TotalRefundAmount;
    Decimal Revenue;
    Decimal ShiftTotalAmount;
    int KkmAddCashCount;
    Decimal KkmAddCashAmount;
    int KkmEncashmentCount;
    Decimal KkmEncashmentAmount;
    Decimal CashInKkm;
    Decimal NotResettableAmount;
    Decimal NotResettableAmountStartShift;

    XreportData(bool encashmentInZOrdersettings, bool isEndOfShift = false);

    std::vector<std::string> GetReportByPayType();

    std::vector<std::string> GetReportToScreen();

    template<class Archive>
    void save(Archive& ar) const
    {
        std::map<std::string, std::string> salesStr;
        std::map<std::string, std::string> refundStr;

        for (const auto& [k, v] : PayTypeSalesAmount)
            salesStr[k] = decimal_to_string(v);

        for (const auto& [k, v] : PayTypeRefundAmount)
            refundStr[k] = decimal_to_string(v);

        std::string totalSalesAmountStr         = decimal_to_string(TotalSalesAmount);
        std::string totalRefundAmountStr        = decimal_to_string(TotalRefundAmount);
        std::string revenueStr                  = decimal_to_string(Revenue);
        std::string shiftTotalAmountStr         = decimal_to_string(ShiftTotalAmount);
        std::string addCashAmountStr            = decimal_to_string(KkmAddCashAmount);
        std::string encashmentAmountStr         = decimal_to_string(KkmEncashmentAmount);
        std::string cashInKkmStr                = decimal_to_string(CashInKkm);
        std::string notResettableAmountStr      = decimal_to_string(NotResettableAmount);
        std::string notResettableStartStr       = decimal_to_string(NotResettableAmountStartShift);

        ar(
            CEREAL_NVP(isEndOfShift),
            CEREAL_NVP(encashmentInZOrder),

            CEREAL_NVP(Name1),
            CEREAL_NVP(Name2),
            CEREAL_NVP(ZreportNumber),
            CEREAL_NVP(StartShiftDateTime),
            CEREAL_NVP(KkmSerialNumber),
            CEREAL_NVP(TerminalNumber),
            CEREAL_NVP(PayTypeNames),

            CEREAL_NVP(salesStr),
            CEREAL_NVP(refundStr),

            CEREAL_NVP(TotalSalesCount),
            CEREAL_NVP(totalSalesAmountStr),
            CEREAL_NVP(TotalRefundCount),
            CEREAL_NVP(totalRefundAmountStr),
            CEREAL_NVP(revenueStr),
            CEREAL_NVP(shiftTotalAmountStr),

            CEREAL_NVP(KkmAddCashCount),
            CEREAL_NVP(addCashAmountStr),
            CEREAL_NVP(KkmEncashmentCount),
            CEREAL_NVP(encashmentAmountStr),
            CEREAL_NVP(cashInKkmStr),
            CEREAL_NVP(notResettableAmountStr),
            CEREAL_NVP(notResettableStartStr)
        );
    }

    template<class Archive>
    void load(Archive& ar)
    {
        std::string totalSalesAmountStr;
        std::string totalRefundAmountStr;
        std::string revenueStr; 
        std::string shiftTotalAmountStr;
        std::string addCashAmountStr;   
        std::string encashmentAmountStr;
        std::string cashInKkmStr;
        std::string notResettableAmountStr;
        std::string notResettableStartStr;

        std::map<std::string, std::string> salesStr;
        std::map<std::string, std::string> refundStr;

        ar(
            CEREAL_NVP(isEndOfShift),
            CEREAL_NVP(encashmentInZOrder),

            CEREAL_NVP(Name1),
            CEREAL_NVP(Name2),
            CEREAL_NVP(ZreportNumber),
            CEREAL_NVP(StartShiftDateTime),
            CEREAL_NVP(KkmSerialNumber),
            CEREAL_NVP(TerminalNumber),
            CEREAL_NVP(PayTypeNames),

            CEREAL_NVP(salesStr),
            CEREAL_NVP(refundStr),

            CEREAL_NVP(TotalSalesCount),
            CEREAL_NVP(totalSalesAmountStr),
            CEREAL_NVP(TotalRefundCount),
            CEREAL_NVP(totalRefundAmountStr),
            CEREAL_NVP(revenueStr),
            CEREAL_NVP(shiftTotalAmountStr),

            CEREAL_NVP(KkmAddCashCount),
            CEREAL_NVP(addCashAmountStr),
            CEREAL_NVP(KkmEncashmentCount),
            CEREAL_NVP(encashmentAmountStr),
            CEREAL_NVP(cashInKkmStr),
            CEREAL_NVP(notResettableAmountStr),
            CEREAL_NVP(notResettableStartStr)
        );

        for (const auto& [k, v] : salesStr)
            PayTypeSalesAmount[k] = string_to_decimal(v);

        for (const auto& [k, v] : refundStr)
            PayTypeRefundAmount[k] = string_to_decimal(v);

        TotalSalesAmount = string_to_decimal(totalSalesAmountStr);
        TotalRefundAmount = string_to_decimal(totalRefundAmountStr);
        Revenue = string_to_decimal(revenueStr);
        ShiftTotalAmount = string_to_decimal(shiftTotalAmountStr);

        KkmAddCashAmount = string_to_decimal(addCashAmountStr);
        KkmEncashmentAmount = string_to_decimal(encashmentAmountStr);
        CashInKkm = string_to_decimal(cashInKkmStr);
        NotResettableAmount = string_to_decimal(notResettableAmountStr);
        NotResettableAmountStartShift = string_to_decimal(notResettableStartStr);
    }

};

}

#endif