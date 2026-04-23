#include "bridge/Printer.hpp"

#include <boost/make_unique.hpp>
#include <iostream>

namespace bridge {

Printer::Printer(itp::root& node, uint8_t address)
    : m_node(node)
    , m_address(address)
{
}

void Printer::printText(const std::string& text, PrintCallback callback) {
    std::cout << "[Printer] Printing text: \"" << text << "\"" << std::endl;

    itp::frame::uptr request = boost::make_unique<itp::frame>(itp::CL2_CMD_PRT_PRINT_TEXT);
    itp_error_code_t errc = request->write_wide_string(text);

    if (errc) {
        std::cerr << "[Printer] Failed to create print request, error=" << errc << std::endl;
        if (callback) callback(false, "Failed to create request");
        return;
    }

    errc = m_node.push_request(
        std::move(request),
        m_address,
        [this, callback](itp::root&, uint16_t err, itp::frame& resp) {
            this->onPrintTextResponse(err, resp, callback);
        }
    );

    if (errc) {
        std::cerr << "[Printer] Failed to send print request, error=" << errc << std::endl;
        if (callback) callback(false, "Failed to send request");
    }
}

void Printer::cutPaper(bool fullCut, PrintCallback callback) {
    std::cout << "[Printer] Cutting paper, fullCut=" << fullCut << std::endl;

    uint8_t command_data = fullCut ? 0 : 1;

    itp::frame::uptr request = boost::make_unique<itp::frame>(itp::CL2_CMD_PRT_CUT_PAPER);
    itp_error_code_t errc = request->write_value(command_data);

    if (errc) {
        std::cerr << "[Printer] Failed to create cut paper request, error=" << errc << std::endl;
        if (callback) callback(false, "Failed to create request");
        return;
    }

    errc = m_node.push_request(
        std::move(request),
        m_address,
        [this, callback](itp::root&, uint16_t err, itp::frame& resp) {
            this->onCutPaperResponse(err, resp, callback);
        }
    );

    if (errc) {
        std::cerr << "[Printer] Failed to send cut paper request, error=" << errc << std::endl;
        if (callback) callback(false, "Failed to send request");
    }
}

void Printer::onPrintTextResponse(uint16_t error, itp::frame& response, PrintCallback callback) {
    if (error) {
        std::cerr << "[Printer] Print text failed, error=" << error << std::endl;
        if (callback) callback(false, "Print error " + std::to_string(error));
        return;
    }

    std::cout << "[Printer] Print text success" << std::endl;
    if (callback) callback(true, "");
}

void Printer::onCutPaperResponse(uint16_t error, itp::frame& response, PrintCallback callback) {
    if (error) {
        std::cerr << "[Printer] Cut paper failed, error=" << error << std::endl;
        if (callback) callback(false, "Cut paper error " + std::to_string(error));
        return;
    }

    std::cout << "[Printer] Cut paper success" << std::endl;
    if (callback) callback(true, "");
}

} // namespace bridge
