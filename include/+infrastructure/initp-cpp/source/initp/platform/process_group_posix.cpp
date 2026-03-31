#include <initp/platform/process_group.hpp>
#include <boost/algorithm/string.hpp>

extern "C" {

#include <initp/system/debug.h>
#include <initp/system/time.h>

#include <spawn.h>
#include <sys/wait.h>
#include <string.h>

}

extern char **environ;

namespace initp {
namespace system {

    // Class process

process_group::process::process(void):
    parent_(nullptr),
    started_(false),
    restart_(false),
    pid_(0)
{}

process_group::process::process(process_group* parent, const std::string& path, const std::string& argv):
    parent_(parent),
    path_(path),
    argv_(argv),
    started_(false),
    restart_(false),
    pid_(0)
{}

process_group::process::~process(void) {
    this->terminate();
}

error_code process_group::process::create(const std::string& path, const std::string& argv) {

    if (this->started_) return err::invalid_state;

    // split argv to list of args
    std::vector<std::string> argv_list;
    boost::split(argv_list, argv, boost::is_any_of(" "));

    // declare UNIX argv list
    std::vector<std::vector<char>> unix_argv_list;

    // prepare path data
    std::vector<char> path_data;
    std::copy(path.begin(), path.end(), std::back_inserter(path_data));
    path_data.push_back('\0');

    // add args into UNIX argv list
    for (const auto &argv_list_item : argv_list)
    {
        // prepare argv_list_item data
        std::vector<char> argv_list_item_data;
        std::copy(argv_list_item.begin(), argv_list_item.end(), std::back_inserter(argv_list_item_data));
        argv_list_item_data.push_back('\0');

        // add item to UNIX argv list
        unix_argv_list.push_back(argv_list_item_data);
    }

    // fill UNIX array list (each element of such array has to be pointer)
    std::vector<char*> unix_argv_array_list;
    for (auto &unix_argv_list_item : unix_argv_list)
    {
        unix_argv_array_list.push_back(unix_argv_list_item.data());
    }

    // push null pointer to the end of the array
    unix_argv_array_list.push_back(nullptr);

    // get const pointer to the array
    const auto unix_argv_array_list_pointer = unix_argv_array_list.data();

    fflush(NULL);
    int status = posix_spawn(&this->pid_, path.c_str(), NULL, NULL, unix_argv_array_list_pointer, environ);
    if (status == 0) {
        sys_debug_print(SYSTEM_LEVEL_INFO, "initp::system::process_group::process::create", "Spawned child process with PID %d", (int)this->pid_);
    } else {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::system::process_group::process::create", "posix_spawn failed. %s", strerror(status));
        return err::internal_error;
    }

    this->started_ = true;
    return err::success;
}

error_code process_group::process::run(const std::string& path, const std::string& argv) {
    error_code ec = this->create(path, argv);
    if (ec) return ec;
    int status = 0;
    pid_t result;
    while (true) {
        result = waitpid(this->pid_, &status, WNOHANG);
        if (result >= 0) {
            sys_sleep_for(10);
        } else {
            return system::err::success;
        }
    }
}

void process_group::process::poll(void) {
    if (!this->started_) {
        if (this->restart_) {
            if (this->create(this->path_, this->argv_)) {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::system::process_group::process::poll", "Failed to restart process %s", this->path_.c_str());
            } else {
                this->restart_ = false;
            }
        }
        return;
    }
    int status = 0;
    pid_t result = waitpid(this->pid_, &status, WNOHANG);
    /*if (result > 0) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "initp::system::process_group::process::poll", "Child exited with status %d", status);
    } else if (result == 0) {
        //sys_debug_print(SYSTEM_LEVEL_TRACE, "process_group::process::poll", "Child still exist");
    } else {
        this->started_ = false;
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::system::process_group::process::poll", "waitpid failed. %s", strerror(errno));
        sys_debug_print(SYSTEM_LEVEL_INFO, "initp::system::process_group::process::poll", "Restarting process %s", this->path_.c_str());
        if (this->create(this->path_, this->argv_)) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::system::process_group::process::poll", "Failed to restart process %s", this->path_.c_str());
            this->restart_ = true;
        }
    }*/
}

void process_group::process::terminate(void) {
    /*if (!this->started_) return;
    this->started_ = false;
    if (this->restart_)
        this->restart_ = false;
    kill(this->pid_, SIGKILL);*/
}

    // Construction

process_group::process_group(void):
    list_()
{}

process_group::~process_group(void) {
    this->terminate_all();
}

    // Public methods

error_code process_group::create(const std::string& path, const std::string& argv_first_start, const std::string& argv_on_restart) {
    this->list_.emplace_back(this, path, argv_on_restart);
    return this->list_.back().create(path, argv_first_start);
}

void process_group::poll(void) {
    for (process& proc : this->list_) {
        proc.poll();
    }
}

void process_group::terminate_all(void) {
    for (process& proc : this->list_) {
        proc.terminate();
    }
    this->list_.clear();
}

}}
