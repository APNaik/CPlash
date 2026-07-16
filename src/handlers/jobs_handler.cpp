#include "jobs_handler.hpp"
#include "helpers/jobs.hpp"

#include <iomanip>
#include <iostream>

std::vector<Job> active_background_jobs;

void handle_jobs(const Command &command){
  for(size_t i { 0 }; i<active_background_jobs.size(); ++i){
    const auto& job = active_background_jobs[i];

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
  }
}