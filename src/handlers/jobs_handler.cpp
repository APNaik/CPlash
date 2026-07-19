#include "jobs_handler.hpp"
#include "helpers/jobs.hpp"

void handle_jobs(const Command &command){
  poll_and_process_jobs(true);      
}