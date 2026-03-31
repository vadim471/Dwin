#pragma once
#include <cereal/archives/json.hpp>

#include <KkmDocument.hpp>

namespace services::FiscalRegister::fiscal_register_interface
{

struct PrintDocumentRequest
{
public:
    KkmDocument kkmDocument;
    bool printDocument;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(
            CEREAL_NVP(kkmDocument),
            CEREAL_NVP(printDocument)
        );
    }
};

}
