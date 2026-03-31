#include <iostream>
#include <functional>
#include <vector>

#include <initp/http/query.hpp>
#include <initp/http/rest.hpp>
//#include <initp/http/auth/digest.hpp>

using namespace initp;

typedef http::rest::node<int, std::string, void, const http::query::params_type&> node_type;

struct C: public node_type {
    C(const std::string& id):
        id_(id),
        method_("GET") {}
    virtual const std::string& rest_id(void) const {
        return this->id_;
    }
    virtual const std::string& rest_method(void) const {
        return this->method_;
    }
    virtual const int& rest_meta(void) const {
        return this->meta_;
    }
    virtual void operator()(const http::query::params_type&) {
        std::cout << "Hello from C(" << this->id_ << ")" << std::endl;
    }
    std::string id_;
    const std::string method_;
    const int meta_ = 0;
};

struct B: public node_type {
    B(const std::string& id):
        id_(id),
        method_("GET") {}
    virtual const std::string& rest_id(void) const {
        return this->id_;
    }
    virtual const std::string& rest_method(void) const {
        return this->method_;
    }
    virtual const int& rest_meta(void) const {
        return this->meta_;
    }
    virtual void operator()(const http::query::params_type&) {
        std::cout << "Hello from B(" << this->id_ << ")" << std::endl;
    }
    void c(const http::query::params_type& args) {
        std::cout << "Hello from B(" << this->id_ << ")::c()" << std::endl;
    }
    void c5(const http::query::params_type& args) {
        std::cout << "Hello from B(" << this->id_ << ")::c5()" << std::endl;
    }
    std::string id_;
    const std::string method_;
    const int meta_ = 0;
};

struct A: public node_type {
    A(void):
        id_("a"),
        method_("GET") {}
    virtual const std::string& rest_id(void) const {
        return this->id_;
    }
    virtual const std::string& rest_method(void) const {
        return this->method_;
    }
    virtual const int& rest_meta(void) const {
        return this->meta_;
    }
    virtual void operator()(const http::query::params_type&) {
        std::cout << "Hello from A(" << this->id_ << ")" << std::endl;
    }
    void f(const http::query::params_type& args) {
        std::cout << "Hello from A(" << this->id_ << ")::f()" << std::endl;
    }
private:
    const std::string id_;
    const std::string method_;
    const int meta_ = 0;
};

int main(int argc, char** argv) {
    /*std::string qop, nonce, opaque;
    const std::string user("user");
    const std::string password("test");
    const std::string key("prime-g6vOl12m");
    const std::string realm("prime@init-plus.com");
    http::auth::digest::generate(key, qop, nonce, opaque);
    http::auth::digest::check(user, password, "GET", "/", realm, qop, nonce, opaque, "qwerty", "1", "");
    return 0;*/
    using namespace std::placeholders;
    A a;
    a.create_rest_function("GET", "f", std::bind(&A::f, &a, _1), 1);
    B b1("b1"), b2("b2");
    a.create_rest_child(b1);
    a.create_rest_child(b2);
    C c1("c1"), c2("c2");
    {
        node_type& group = b1.create_rest_group("GET", "c", std::bind(&B::c, &b1, _1), 2);
        group.create_rest_child(c1);
        group.create_rest_child(c2);
    }
    C c3("c3"), c4("c4");
    {
        node_type& group = b2.create_rest_group("GET", "c", std::bind(&B::c, &b2, _1), 3);
        group.create_rest_child(c3);
        group.create_rest_child(c4);
        group.create_rest_function("GET", "c5", std::bind(&B::c5, &b2, _1), 4);
    }
    http::query q1("/a/f");
    (*a.find_rest_node("GET", q1.path()))(q1.params());
    http::query q2("/a/b1");
    (*a.find_rest_node("GET", q2.path()))(q2.params());
    http::query q3("/a/b1/c");
    (*a.find_rest_node("GET", q3.path()))(q3.params());
    http::query q4("/a/b1/c/c1");
    (*a.find_rest_node("GET", q4.path()))(q4.params());
    http::query q5("/a/b2/c/c5");
    (*a.find_rest_node("GET", q5.path()))(q5.params());
    std::cout << "Meta: " << a.find_rest_node("GET", q5.path())->rest_meta() << std::endl;

    //q5.escape(false);
    q5.add("key1", "value1");
    q5.add("key2", "value 2");
    std::cout << "URI: " << q5.dump() << std::endl;
    http::query q6(q5.dump());
    std::cout << q6.params()["key2"] << std::endl;
    return 0;
}
