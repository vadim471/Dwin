#include <initp/utils/hash.hpp>

#include <boost/version.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <boost/algorithm/hex.hpp>

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>

#include <algorithm>
#include <sstream>

namespace initp {
namespace utils {
namespace hash {

#if (BOOST_VERSION >= 107300)

std::string md5(const void* data, size_t bytes) {
    using boost::uuids::detail::md5;
    md5 hash;
    md5::digest_type digest;
    hash.process_bytes(data, bytes);
    hash.get_digest(digest);
    const int* int_digest = reinterpret_cast<const int*>(&digest);
    std::string result;
    boost::algorithm::hex(int_digest, int_digest + (sizeof(md5::digest_type)/sizeof(int)), std::back_inserter(result));
    return result;
}

#else // (BOOST_VERSION >= 107300)

std::string md5(const void* data, size_t bytes) {
    using boost::uuids::detail::md5;
    md5 hash;
    md5::digest_type digest;
    hash.process_bytes(data, bytes);
    hash.get_digest(digest);
    const char* char_digest = reinterpret_cast<const char*>(&digest);
    std::string result;
    boost::algorithm::hex(char_digest, char_digest + sizeof(md5::digest_type), std::back_inserter(result));
    return result;
}

#endif // (BOOST_VERSION >= 107300)


std::string md5(const std::string& data) {
    return md5(data.c_str(), data.size());
}

std::string encode_base64(const std::string& data) {

    using namespace boost::archive::iterators;

    std::stringstream os;
    typedef
        insert_linebreaks<         // insert line breaks every 72 characters
            base64_from_binary<    // convert binary values to base64 characters
                transform_width<   // retrieve 6 bit integers from a sequence of 8 bit bytes
                    const char*,
                    6,
                    8
                >
            >
            , 72
        >
        base64_text; // compose all the above operations in to a new iterator

    std::copy(
        base64_text(data.c_str()),
        base64_text(data.c_str() + data.size()),
        std::ostream_iterator<char>(os)
    );

    os.seekp(0, std::ios::end);
    std::stringstream::pos_type offset = os.tellp();
    while (offset % 4) {
        os << "=";
        os.seekp(0, std::ios::end);
        offset = os.tellp();
    }
    return os.str();
}

std::string decode_base64(std::string& input) {

    using namespace boost::archive::iterators;
    typedef transform_width<binary_from_base64<remove_whitespace
        <std::string::const_iterator> >, 8, 6> ItBinaryT;

    try {

        // If the input isn't a multiple of 4, pad with =
        size_t num_pad_chars((4 - input.size() % 4) % 4);
        input.append(num_pad_chars, '=');

        size_t pad_chars(std::count(input.begin(), input.end(), '='));
        std::replace(input.begin(), input.end(), '=', 'A');
        std::string output(ItBinaryT(input.begin()), ItBinaryT(input.end()));
        output.erase(output.end() - pad_chars, output.end());
        return output;
    } catch (std::exception const&) {
        return std::string();
    }
}

}}}
