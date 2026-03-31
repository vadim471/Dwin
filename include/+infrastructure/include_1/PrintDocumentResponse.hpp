#pragma once

#include <string>
#include <memory>
#include <cstdint>

#include <DocumentType.hpp>
#include <CorrectionType.hpp>
#include <PaymentType.hpp>

#include <cereal/cereal.hpp>

namespace services::FiscalRegister::fiscal_register_interface
{

struct PrintDocumentResponse
{
public:
    std::string CodeQR;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(CodeQR));
    }
};

}
