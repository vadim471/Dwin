#include <iostream>
#include <sstream>
#include <initp/http/cookies.hpp>
#include <initp/http/auth/digest.hpp>
#include <initp/tools/hash.hpp>

int main(int argc, char** argv) {

    initp::http::cookies cookies("KEY1=VALUE1; KEY2=V a l u e 2;");
    std::stringstream s;
    for (auto& it : cookies.params())
        s << "Key: " << it.first << "; Value: " << it.second << std::endl;
    std::string string = s.str();
    std::cout << string << std::endl;

    std::cout << "MD5: " << initp::tools::hash::md5(string) << std::endl;

    const std::string realm = "testrealm@host.com";
    std::cout << initp::http::auth::digest::generate("test_key", realm) << std::endl;

    if (!initp::http::auth::digest::check("Mufasa", "Circle Of Life", "GET", "Digest \
                                    username=\"Mufasa\",\
                                    realm=\"testrealm@host.com\",\
                                    nonce=\"dcd98b7102dd2f0e8b11d0f600bfb0c093\",\
                                    uri=\"/dir/index.html\",\
                                    qop=auth,\
                                    nc=00000001,\
                                    cnonce=\"0a4f113b\",\
                                    response=\"6629fae49393a05397450978507c4ef1\",\
                                    opaque=\"5ccc069c403ebaf9f0171e9517f40e41\"")) {
        std::cout << "Digest doesn't match" << std::endl;
    } else std::cout << "Digest OK" << std::endl;
    return 0;
}
