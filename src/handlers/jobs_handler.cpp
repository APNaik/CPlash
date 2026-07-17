#include "jobs_handler.hpp"
#include "helpers/jobs.hpp"

#include <iomanip>
#include <iostream>
#include <sys/wait.h>
#include <algorithm>

std::vector<Job> active_background_jobs;

void handle_jobs(const Command &command){
  std::vector<Job> next_cycle_jobs;

  for(size_t i { 0 }; i<active_background_jobs.size(); ++i){
    auto& job = active_background_jobs[i];
    int status;
    pid_t result = waitpid(job.pid, &status, WNOHANG);   // WNOHANG checks the process status instantly without halting shell thread
    
    char marker {};
    if(i == active_background_jobs.size() - 1){
      marker = '+';
    }
    else if(i == active_background_jobs.size() - 2){
      marker = '-';
    }
    else marker = ' ';

    if(result == 0){    // Process is still executing
      char marker {};
      if(i == active_background_jobs.size() - 1){
        marker = '+';
      }
      else if(i == active_background_jobs.size() - 2){
        marker = '-';
      }
      else marker = ' ';
      
      std::cout << "[" << job.job_number << "]" << marker << " "
                << std::left << std::setw(24) << job.status
                << job.command_string << "\n"; 
      next_cycle_jobs.push_back(job);
    }
    else if(result > 0){    // Process is exited
      job.status = "Done";
      std::string display_cmd = job.command_string;
      if (display_cmd.ends_with(" &")) {
        display_cmd.erase(display_cmd.length() - 2);
      }
            
      std::cout << "[" << job.job_number << "]" << marker << " "
                << std::left << std::setw(24) << job.status
                << display_cmd << "\n";
    }
  }
  active_background_jobs = std::move(next_cycle_jobs);
}