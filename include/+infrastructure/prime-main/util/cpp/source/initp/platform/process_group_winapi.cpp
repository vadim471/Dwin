#include <initp/platform/process_group.hpp>

extern "C" {

#include <initp/system/debug.h>
#include <initp/system/time.h>

}

namespace initp {
namespace system {

    // Class process

process_group::process::process(void):
    parent_(nullptr),
    started_(false),
    restart_(false)
{}

process_group::process::process(process_group* parent, const std::string& path, const std::string& argv):
    parent_(parent),
    path_(path),
    argv_(argv),
    started_(false),
    restart_(false)
{}

process_group::process::~process(void) {
    this->terminate();
}

error_code process_group::process::create(const std::string& path, const std::string& argv) {

    if (this->started_) return err::invalid_state;

    // set the size of the structures
    ZeroMemory(&this->si_, sizeof(this->si_));
    this->si_.cb = sizeof(this->si_);
    ZeroMemory(&this->pi_, sizeof(this->pi_));

    char data[0x200];
    sprintf(data, "%s", argv.c_str());
    BOOL result = CreateProcessA(path.c_str(),   // the path
        data,           // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory
        &this->si_,     // Pointer to STARTUPINFO structure
        &this->pi_      // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
    );
    if (!result) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::system::process_group::process::create", "CreateProcessA returned %d", (int)GetLastError());
        return err::internal_error;
    }

    if (this->parent_ && this->parent_->job_) {
        if (!AssignProcessToJobObject(this->parent_->job_, this->pi_.hProcess)) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::system::process_group::process::create", "AssignProcessToJobObject returned %d", (int)GetLastError());
        }
    }

    this->started_ = true;
    return err::success;
}

error_code process_group::process::run(const std::string& path, const std::string& argv) {
    error_code ec = this->create(path, argv);
    if (ec) return ec;
    DWORD result;
    while (true) {
        result = WaitForSingleObject(this->pi_.hProcess, 0);
        if (result == WAIT_TIMEOUT) {
            sys_sleep_for(10);
        } else {
            if (result != 0) {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::system::process_group::process::run", "WaitForSingleObject result %d", (int)result);
            }
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
    DWORD result = WaitForSingleObject(this->pi_.hProcess, 0);
    if (result == WAIT_TIMEOUT) {
        // Nothing need to do
    } else {
        if (result != 0) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::system::process_group::process::poll", "WaitForSingleObject result %d", (int)result);
        }
        sys_debug_print(SYSTEM_LEVEL_INFO, "initp::system::process_group::process::poll", "Restarting process %s", this->path_.c_str());
        this->started_ = false;
        if (this->create(this->path_, this->argv_)) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::system::process_group::process::poll", "Failed to restart process %s", this->path_.c_str());
            this->restart_ = true;
        }
    }
}

void process_group::process::terminate(void) {
    if (!this->started_) return;
    this->started_ = false;
    if (this->restart_)
        this->restart_ = false;
    TerminateProcess(this->pi_.hProcess, 0);
}

    // Construction

process_group::process_group(void):
    list_(),
    job_(NULL) {
    HANDLE ghJob = CreateJobObject(NULL, NULL); // GLOBAL
    if (ghJob == NULL) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::system::process_group::process_group", "Failed to create job object");
    } else {
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };
        // Configure all child processes associated with the job to terminate when the parent dies
        jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        if (!SetInformationJobObject(ghJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli))) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::system::process_group::process_group", "SetInformationJobObject returned %d", (int)GetLastError());
        } else {
            this->job_ = ghJob;
        }
    }
}

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
