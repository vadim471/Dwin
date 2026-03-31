#ifndef ITP_TUNNEL_HPP_INCLUDED
#define ITP_TUNNEL_HPP_INCLUDED

#include <itp/config.hpp>
#include <itp/endpoint.hpp>

#include <cstring>
#include <memory>
#include <tuple>

namespace itp {

class tunnel;

class tunnel_endpoint:
    public itp::endpoint {
    friend class tunnel;
    typedef tunnel_endpoint self_type;
public:
    typedef std::unique_ptr<self_type> uptr;
public:
    tunnel_endpoint(void):
        length_(0),
        remote_data_(NULL),
        remote_length_(0)
    {}
    tunnel_endpoint(const tunnel_endpoint&) = delete;
    virtual ~tunnel_endpoint(void) = default;
public:
    virtual itp_size_t read(itp_byte_t* data, itp_size_t size) {
        itp_size_t length = this->length_;
        if (size >= this->length_) {
            length = this->length_;
            memcpy(data, this->data_, length);
            this->length_ = 0;
        } else {
            length = size;
            memcpy(data, this->data_, length);
            memcpy(this->data_, this->data_ + length, this->length_ - length);
            this->length_ -= length;
        }
        return length;
    }
    virtual itp_size_t write(const itp_byte_t* data, itp_size_t length) {
        itp_size_t max = ITP_TUNNEL_BUFFER_SIZE - (*this->remote_length_);
        if (length > max) length = max;
        memcpy(this->remote_data_ + (*this->remote_length_), data, length);
        *this->remote_length_ += length;
        return length;
    }
    void remote(itp_byte_t* data, itp_size_t* length) {
        this->remote_data_ = data;
        this->remote_length_ = length;
    }
private:
    itp_byte_t data_[ITP_TUNNEL_BUFFER_SIZE];
    itp_size_t length_;
    itp_byte_t* remote_data_;
    itp_size_t* remote_length_;
};

class tunnel {
public:
    static std::tuple<endpoint::uptr, endpoint::uptr> join(void) {
        tunnel_endpoint::uptr ep0(new tunnel_endpoint());
        tunnel_endpoint::uptr ep1(new tunnel_endpoint());
        ep0->remote(ep1->data_, &ep1->length_);
        ep1->remote(ep0->data_, &ep0->length_);
        return std::make_tuple(std::move(ep0), std::move(ep1));
    }
};

}

#endif // ITP_TUNNEL_HPP_INCLUDED
