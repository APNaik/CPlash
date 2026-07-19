#include "jobs.hpp"

#include <iomanip>
#include <iostream>
#include <sys/wait.h>
#include <algorithm>

int JobIDManager::acquire_id(){
  for(int i { 1 }; i < MAX_JOBS; ++i){
    if(!used_ids.test(i)){
      used_ids.set(i);
      return i;
    }
  }
  return -1;
}

void JobIDManager::release_id(int id){
  if(id > 0 && id < MAX_JOBS){
    used_ids.reset(id);
  }
}

JobIDManager jim;

std::vector<Job> active_background_jobs;

void poll_and_process_jobs(bool display_running){
  std::vector<Job> next_cycle_jobs;

  size_t original_size { active_background_jobs.size() };
  for(size_t i { 0 }; i<active_background_jobs.size(); ++i){
    auto& job = active_background_jobs[i];
    int status;
    pid_t result = waitpid(job.pid, &status, WNOHANG);   // WNOHANG checks the process status instantly without halting shell thread
    
    char marker {};
    if(i == original_size - 1){
      marker = '+';
    }
    else if(i == original_size - 2){
      marker = '-';
    }
    else marker = ' ';

    if(result == 0){    // Process is still executing
      if(display_running){
        std::cout << "[" << job.job_number << "]" << marker << "  "
                << std::left << std::setw(24) << job.status
                << job.command_string << "\n"; 
      }
      next_cycle_jobs.push_back(job);
    }
    else if(result > 0){    // Process is exited
      job.status = "Done";
      jim.release_id(job.job_number);
      std::string display_cmd = job.command_string;
      if (display_cmd.ends_with(" &")) {
        display_cmd.erase(display_cmd.length() - 2);
      }
            
      std::cout << "[" << job.job_number << "]" << marker << "  "
                << std::left << std::setw(24) << job.status
                << display_cmd << "\n";
    }
  }
  active_background_jobs = std::move(next_cycle_jobs);
}