#ifndef INITPLUS_HTTP_AUTH_DIGEST_HPP_INCLUDED
#define INITPLUS_HTTP_AUTH_DIGEST_HPP_INCLUDED

#include <string>
#include <unordered_map>

namespace initp {
namespace http {
namespace auth {
namespace digest {

typedef std::unordered_map<std::string, std::string> map_type;

bool parse(const std::string& authorization, map_type& map);

bool generate(
    const std::string& private_key,
    std::string& qop,
    std::string& nonce,
    std::string& opaque
);

std::string generate(
    const std::string& private_key,
    const std::string& realm
);

bool check(
    const std::string& username,
    const std::string& password,
    const std::string& method,
    const std::string& uri,
    const std::string& realm,
    const std::string& qop,
    const std::string& nonce,
    const std::string& opaque,
    const std::string& cnonce,
    const std::string& nc,
    const std::string& response
);

bool check(
    const std::string& username,
    const std::string& password,
    const std::string& method,
    const map_type& map
);

bool check(
    const std::string& username,
    const std::string& password,
    const std::string& method,
    const std::string& authorization
);

}}}}

#endif // INITPLUS_HTTP_AUTH_DIGEST_HPP_INCLUDED
