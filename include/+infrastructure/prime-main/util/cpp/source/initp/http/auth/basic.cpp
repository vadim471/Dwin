#include <initp/http/auth/basic.hpp>
#include <initp/tools/hash.hpp>

#include <sstream>

namespace initp {
namespace http {
namespace auth {
namespace basic {

std::string generate(
    const std::string& user,
    const std::string& password
) {
    std::stringstream stream;
    stream << user << ":" << password;
    std::string b64 = tools::hash::encode_base64(stream.str());
    stream.str("");
    stream << "Basic " << b64;
    return stream.str();
}

}}}}
