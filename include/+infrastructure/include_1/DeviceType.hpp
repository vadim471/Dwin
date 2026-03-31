#ifndef ARKAIM_PAY_FISCAL_REGISTER_DEVICE_TYPE_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_DEVICE_TYPE_HPP

namespace services::FiscalRegister::fiscal_register_interface::DeviceType
{

enum DeviceType
    {
        Unknown = 0,
        Service,
        FiscalRegister,
        CashAcceptor,
        CardReader,
        PinPad,
        BarCodeScanner,
        CashDispenser,
        CoinAcceptor,
        LcdController,
        CardReaderCurtainDriver,
        SAMmodule
    };

}

#endif