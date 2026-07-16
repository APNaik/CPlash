#pragma once

#include <string>
#include <vector>

struct Job {
    int job_number;
    pid_t pid;
    std::string command_string;
    std::string status;
};

extern std::vector<Job> active_background_jobs;