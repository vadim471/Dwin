#ifndef ARKAIM_PAY_FISCAL_REGISTER_PRINT_TICKET_DATA_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_PRINT_TICKET_DATA_HPP

#include <cereal/access.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

namespace services::FiscalRegister::cl2::Data
{
    struct PrintTicketData
    {
        std::vector<std::string> Ticket;

        template <class Archive>
        void serialize(Archive &ar)
        {
            ar(CEREAL_NVP(Ticket));
        }
    };
}

#endif