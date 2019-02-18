//
// loc.cc - sjm interface to Local job submission API
//
// Phil Lacroute
// March 2008
//
// Copyright(c) 2008-2012 The Board of Trustees of The Leland Stanford
// Junior University.  All Rights Reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
// 
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
// 
//     * Neither the name of Stanford University nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL STANFORD
// UNIVERSITY BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//


#include <string>
#include <sstream>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

using std::string;
using std::ostringstream;
using std::runtime_error;

#include "loc.hh"

LocBatchSystem::LocBatchSystem ()
{
}

void
LocBatchSystem::init ()
{
    //do some initialization work
}

bool
LocBatchSystem::submit (Job& job)
{
    string jobName = job.name();
    const char* command = job.command().c_str();

    //memory limit
    size_t memoryLimit = job.memoryLimit();
    size_t unit_m = 1024 * 1024;
    size_t totalMemoryB = sysconf(_SC_PHYS_PAGES) *  sysconf(_SC_PAGE_SIZE);
    size_t totalMemoryM = totalMemoryB / unit_m;
    if(totalMemoryM < memoryLimit){
        return false;
    }
    //execute job
    int pid = fork();
    if(pid == 0){
        job.setJobId(getpid());
        execl("/bin/sh", "sh", "-c", command, NULL);
        job.changeStatus(Job::Dispatched);
    }else{
        return false;
    }
    return true;
}

void
LocBatchSystem::kill (Job& job)
{
    if (job.status() == Job::Dispatched ||
	job.status() == Job::Pending ||
	job.status() == Job::Running) {
	if (::kill(job.jobId(), SIGKILL) < 0) {
	    ostringstream err;
	    err << "cannot kill job " << job.name();
	    throw runtime_error(err.str());
	}
	job.changeStatus(Job::Failed);
    }
}

bool
LocBatchSystem::checkStatus (Job& job)
{
    int status = 0;
    struct rusage res;
    int pid = wait4(job.jobId(), &status, WNOHANG, &res);
    if (pid < 0) {
        ostringstream err;
        err << "cannot check job " << job.name() << ": wait4 failed: "
            << pid;
        throw runtime_error(err.str());
    }
    if (pid == 0) {
        return false;
    }
    if (WIFEXITED(status)) {
        unsigned exitCode = WEXITSTATUS(status);
        if (exitCode == 0) {
            job.changeStatus(Job::Done);
        } else {
            job.changeStatus(Job::Failed);
            job.setExitCode(exitCode);
        }
    } else {
        job.changeStatus(Job::Failed);
    }
    job.setCpuTime(res.ru_utime.tv_sec + res.ru_stime.tv_sec +
                   (res.ru_utime.tv_usec + res.ru_stime.tv_usec) / 1000000);
    job.setMemoryUsage(res.ru_maxrss * 1024);
    return true;
}

bool
LocBatchSystem::canWait ()
{
    // LSF does not support a wait() call
    return false;
}

Job *
LocBatchSystem::wait (JobGraph& jobs, unsigned timeout_sec)
{
    return 0;
}

void
LocBatchSystem::cleanup ()
{
}
