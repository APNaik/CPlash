#pragma once

#include <string>
#include <vector>
#include <bitset>

struct Job {
  int job_number;
  pid_t pid;
  std::string command_string;
  std::string status;
};

extern std::vector<Job> active_background_jobs;

constexpr int MAX_JOBS { 1024 };

class JobIDManager {
  std::bitset<MAX_JOBS> used_ids;
public:
  int acquire_id();
  void release_id(int id);
};

extern JobIDManager jim;

// If arg is set to true, this function displays running processes
// Else it only shows completed processes.
void poll_and_process_jobs(bool display_running);    