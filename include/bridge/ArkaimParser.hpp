#pragma once

#include "IMessageParser.hpp"
#include "TerminalEntity.hpp"

namespace bridge {

class ArkaimParser : public IMessageParser {
public:
    ArkaimParser();
    
    // IMessageParser interface
    std::vector<Message> parse(const RawData& data, const std::string& source) override;
    RawData serialize(const Message& message) override;
    
    void setTerminal(TerminalEntity* terminal) { m_terminal = terminal; }

private:
    TerminalEntity* m_terminal;
};

} // namespace bridge
