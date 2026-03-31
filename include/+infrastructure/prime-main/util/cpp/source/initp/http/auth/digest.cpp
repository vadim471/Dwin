#include <initp/http/auth/digest.hpp>
#include <initp/system/time.hpp>
#include <initp/system/debug.hpp>
#include <initp/tools/hash.hpp>

#include <boost/algorithm/string.hpp>

#include <vector>
#include <sstream>

namespace initp {
namespace http {
namespace auth {
namespace digest {

bool parse(const std::string& authorization, map_type& map) {
    if (authorization.size() < 7) return false;
    std::string digest = authorization.substr(0, 6);
    if (digest.compare("Digest") != 0) {
        return false;
    }
    std::string data = authorization.substr(7);
    std::vector<std::string> params;
    boost::split(params, data, boost::is_any_of(","), boost::token_compress_on);
    for (std::string param : params) {
        std::string key, value;
        const auto index = param.find_first_of('=');
        if (index != std::string::npos) {
            key = param.substr(0, index);
            value = param.substr(index + 1);
        } else continue;
        if (key.empty()) continue;
        boost::trim(key);
        boost::trim(value);
        boost::algorithm::to_lower(key);
        map[key] = value;
    }
    return true;
}

bool generate(
    const std::string& private_key,
    std::string& qop,
    std::string& nonce,
    std::string& opaque
) {
    qop = "auth";
    system::time_t time = system::time::now();
    std::stringstream stream;
    stream << time << ":" << private_key;
    nonce = tools::hash::md5(stream.str());
    boost::algorithm::to_lower(nonce);
    stream.str("");
    stream << time << ":http:digest";
    opaque = tools::hash::md5(stream.str());
    boost::algorithm::to_lower(opaque);
    //nonce = "dcd98b7102dd2f0e8b11d0f600bfb0c093";
    //opaque = "5ccc069c403ebaf9f0171e9517f40e41";
    return true;
}

std::string generate(
    const std::string& private_key,
    const std::string& realm
) {
    std::string qop, nonce, opaque;
    if (!initp::http::auth::digest::generate(private_key, qop, nonce, opaque)) {
        return std::string();
    }
    std::stringstream stream;
    stream << "Digest realm=\"" << realm;
    stream << "\", qop=\"" << qop;
    stream << "\", nonce=\"" << nonce;
    stream << "\", opaque=\"" << opaque;
    stream << "\"";
    return stream.str();
}

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
) {
    std::stringstream stream;
    // A1
    stream << username << ":" << realm << ":" << password;
    std::string HA1 = tools::hash::md5(stream.str());
    boost::algorithm::to_lower(HA1);
    stream.str("");
    // A2
    stream << method << ":" << uri;
    std::string HA2 = tools::hash::md5(stream.str());
    boost::algorithm::to_lower(HA2);
    stream.str("");
    // Response
    stream << HA1 << ":" << nonce << ":" << nc << ":" << cnonce << ":" << qop << ":" << HA2;
    std::string result = tools::hash::md5(stream.str());
    boost::algorithm::to_lower(result);

    #ifdef SYSTEM_TRACE_HTTP_DIGEST
    sys_debug_print(SYSTEM_LEVEL_TRACE, "initp::http::digest::check", "--- TRACE -----------");
    sys_debug_print(SYSTEM_LEVEL_TRACE, "initp::http::digest::check", "Username: %s", username.c_str());
    sys_debug_print(SYSTEM_LEVEL_TRACE, "initp::http::digest::check", "Password: %s", password.c_str());
    sys_debug_print(SYSTEM_LEVEL_TRACE, "initp::http::digest::check", "Method: %s", method.c_str());
    sys_debug_print(SYSTEM_LEVEL_TRACE, "initp::http::digest::check", "URI: %s", uri.c_str());
    sys_debug_print(SYSTEM_LEVEL_TRACE, "initp::http::digest::check", "Realm: %s", realm.c_str());
    sys_debug_print(SYSTEM_LEVEL_TRACE, "initp::http::digest::check", "QOP: %s", qop.c_str());
    sys_debug_print(SYSTEM_LEVEL_TRACE, "initp::http::digest::check", "Nonce: %s", nonce.c_str());
    sys_debug_print(SYSTEM_LEVEL_TRACE, "initp::http::digest::check", "Opaque: %s", opaque.c_str());
    sys_debug_print(SYSTEM_LEVEL_TRACE, "initp::http::digest::check", "Cnonce: %s", cnonce.c_str());
    sys_debug_print(SYSTEM_LEVEL_TRACE, "initp::http::digest::check", "NC: %s", nc.c_str());
    sys_debug_print(SYSTEM_LEVEL_TRACE, "initp::http::digest::check", "Response: %s", response.c_str());
    sys_debug_print(SYSTEM_LEVEL_TRACE, "initp::http::digest::check", "HA1: %s", HA1.c_str());
    sys_debug_print(SYSTEM_LEVEL_TRACE, "initp::http::digest::check", "HA2: %s", HA2.c_str());
    sys_debug_print(SYSTEM_LEVEL_TRACE, "initp::http::digest::check", "Result: %s", result.c_str());
    sys_debug_print(SYSTEM_LEVEL_TRACE, "initp::http::digest::check", "---------------------");
    #endif // SYSTEM_TRACE_HTTP_DIGEST

    return response.compare(result) == 0;
}

static std::string unquote(const std::string& string) {
    if (*string.begin() == '"') {
        if (*string.rbegin() == '"') {
            return string.substr(1, string.size() - 2);
        } else {
            return string.substr(1, string.size() - 1);
        }
    } else if (*string.rbegin() == '"') {
        return string.substr(0, string.size() - 1);
    }
    return std::string(string);
}

bool check(
    const std::string& username,
    const std::string& password,
    const std::string& method,
    const map_type& map
) {
    map_type::const_iterator it;

    std::string user;
    it = map.find("username");
    if (it != map.cend()) {
        user = unquote(it->second);
    } else return false;
    if (user.compare(username) != 0) {
        return false;
    }

    std::string qop;
    it = map.find("qop");
    if (it != map.cend()) {
        qop = unquote(it->second);
    } else return false;
    if (qop.compare("auth") != 0) {
        return false;
    }

    std::string realm;
    it = map.find("realm");
    if (it != map.cend()) {
        realm = unquote(it->second);
    } else return false;

    std::string uri;
    it = map.find("uri");
    if (it != map.cend()) {
        uri = unquote(it->second);
    } else return false;

    std::string nonce;
    it = map.find("nonce");
    if (it != map.cend()) {
        nonce = unquote(it->second);
    } else return false;

    std::string opaque;
    it = map.find("opaque");
    if (it != map.cend()) {
        opaque = unquote(it->second);
    } else return false;

    std::string cnonce;
    it = map.find("cnonce");
    if (it != map.cend()) {
        cnonce = unquote(it->second);
    } else return false;

    std::string nc;
    it = map.find("nc");
    if (it != map.cend()) {
        nc = unquote(it->second);
    } else return false;

    std::string response;
    it = map.find("response");
    if (it != map.cend()) {
        response = unquote(it->second);
        boost::algorithm::to_lower(response);
    } else return false;

    return check(username, password, method, uri, realm, qop, nonce, opaque, cnonce, nc, response);
}

bool check(
    const std::string& username,
    const std::string& password,
    const std::string& method,
    const std::string& authorization
) {
    map_type map;
    if (!parse(authorization, map)) {
        return false;
    }
    return check(username, password, method, map);
}

}}}}
