# Simple Job Manager (SJM)

# Summary

SJM is a program for managing a group of related jobs running on a compute cluster. It provides a convenient method for specifying dependencies between jobs and the resource requirements for each job (e.g. memory, CPU cores). It monitors the status of the jobs so you can tell when the whole group is done. If any of the jobs fails (e.g. due to a compute node crashing) SJM allows you to resume without rerunning the jobs that completed successfully. Finally, SJM provides a portable way to submit jobs to different job schedulers such as Sun Grid Engine or Platform LSF.

# Changes

SJM, Cloned from [SJM](https://github.com/StanfordBioinformatics/SJM) with the following modifications by [vanNul](https://github.com/vanNul)
1. remove boost dependency, use embeded [CLI](https://github.com/CLIUtils/CLI11) instead  
2. remove TCLAP dependency, use std::regex instead  
3. remove perl scripts run_with_env, keep perl scripts away  
4. allow compile and executaion under environment without sge/lfs
5. support local job submit and monitoring natively if no sge/lfs found  

# Installation && Usage 
1. clone repo  
   ```
   git clone https://github.com/slw287r/SJM.git
   ```
2. compile  
   ```
   cd SJM
   ./autogen.sh
   ./configure --prefix=/path/to/install/dir/
   make  
   make install
   ```
3. usage  
   <details>
   <summary>prepare a sjm jobfile such as example.sjm</summary>
   
   ```
   job_begin
     name jobA
     time 6m
     memory 10m
     cmd echo "hello from job jobA"
   job_end

   job_begin
     name jobB
     time 8m
     memory 20m
     cmd echo "hello from job jobB"
   job_end

   job_begin
     name jobC
     time 10m
     memory 30m
     cmd echo "hello from job jobB"
   job_end

   job_begin
     name jobD
     time 10m
     memory 35m
     cmd echo "hello from job jobB"
   job_end

   order jobA before jobC
   order jobB before jobC
   order jobC before jobD
   ```
   </details>
   
   and run  
   
   ```
   sjm example.sjm
   ```
   
   you'd better check [manual](https://github.com/slw287r/SJM/tree/master/doc/MANUAL.txt) for more help 
