#pragma once

#include <itp/itp.hpp>
#include <itp/cl2.h>

#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace bridge {

class Printer {
public:
    using PrintCallback = std::function<void(bool success, const std::string& error)>;

    Printer(itp::root& node, uint8_t address);
    ~Printer() = default;

    // Печать текста
    void printText(const std::string& text, PrintCallback callback);
    
    // Отрезать бумагу
    void cutPaper(bool fullCut, PrintCallback callback);

    uint8_t address() const { return m_address; }

private:
    void onPrintTextResponse(uint16_t error, itp::frame& response, PrintCallback callback);
    void onCutPaperResponse(uint16_t error, itp::frame& response, PrintCallback callback);

    itp::root& m_node;
    uint8_t m_address;
};

} // namespace bridge
