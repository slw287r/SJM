//
// sjm.cc - simple job manager
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


#include <iostream>
#include <string>
#include "CLI.hpp"
#include "job_mon.hh"

using std::cerr;
using std::endl;
using std::string;
using std::vector;

// class for storing command-line options
class Options {
public:
    string jobFile_;		// input file name
    string outFile_;		// output file name
    string logFile_;		// log file name
    bool render_;		// render job graph into dot-format, output file instead of submitting jobs
    bool interactive_;		// run as a foreground process instead of in the background
    unsigned dispatchCount_;	// max jobs to dispatch per dispatch interval
    unsigned dispatchInterval_;	// dispatch interval (seconds)
    unsigned pendingJobLimit_;	// max pending jobs
    unsigned runningJobLimit_;	// max running jobs
    unsigned checkJobInterval_;	// interval between checking job status (sec)
    unsigned saveInterval_;	// interval between saving job status (sec)
    unsigned logInterval_;	// interval between logging status (sec)
    unsigned maxStatusErr_;	// give up on a job after this many errors
    string logLevel_;		// log level
    vector<string> mailAddr_;	// email addresses for notification

    Options():
	render_(false),
	interactive_(false),
	dispatchCount_(0),
	dispatchInterval_(0),
	pendingJobLimit_(0),
	runningJobLimit_(0),
	checkJobInterval_(0),
	saveInterval_(0),
	logInterval_(0),
	maxStatusErr_(0)
    {
    }

};

// parse the command-line options
int
main(int argc, char **argv)
{
    std::string sys_cmd = std::string(argv[0]) + " -h";
    if(argc < 2){std::system(sys_cmd.c_str()); return 0;}

    Options options;
    CLI::App app("simple job manager");
    app.add_option("-o,--out", options.outFile_, "output file");
    app.add_option("-l,--log", options.logFile_, "log file");
    app.add_flag("-r,--render", options.render_, "render job graph");
    app.add_flag("-i,--interactive", options.interactive_, "run in foreground");
    app.add_option("--max_dispatch", options.dispatchCount_, "maximum jobs to dispatch per dispatch interval");
    app.add_option("--dispatch_interval", options.dispatchInterval_, "dispatch interval");
    app.add_option("--max_pending", options.pendingJobLimit_, "maximum number of pending jobs");
    app.add_option("--max_running", options.runningJobLimit_, "maximum number of running jobs");
    app.add_option("--check_interval", options.checkJobInterval_, "interval between checking job ");
    app.add_option("--save_interval", options.saveInterval_, "interval between saving job status");
    app.add_option("--log_interval", options.logInterval_, "interval between logging status");
    app.add_option("--max_status_errors", options.maxStatusErr_, "maximum status-collection errors");
    app.add_option("--log_level", options.logLevel_, "log level[verbose|info|warning|error]");
    app.add_option("--mail", options.mailAddr_, "email address for notification");
    app.add_option("jobfile", options.jobFile_, "job description file")->required(true)->check(CLI::ExistingFile);
    CLI_PARSE(app, argc, argv);

    JobMonitor jobMon;
    if (options.render_) {
	if (!jobMon.render(options.jobFile_, options.outFile_)) {
	    exit(1);
	}
    } else {
	jobMon.setDispatchCount(options.dispatchCount_);
	jobMon.setDispatchInterval(options.dispatchInterval_);
	jobMon.setPendingJobLimit(options.pendingJobLimit_);
	jobMon.setRunningJobLimit(options.runningJobLimit_);
	jobMon.setCheckJobInterval(options.checkJobInterval_);
	jobMon.setSaveInterval(options.saveInterval_);
	jobMon.setLogInterval(options.logInterval_);
	jobMon.setMaxStatusErr(options.maxStatusErr_);
	if (!options.logLevel_.empty()) {
	    if (options.logLevel_ == "verbose") {
		jobMon.setLogLevel(JobMonitor::LogVerbose);
	    } else if (options.logLevel_ == "info") {
		jobMon.setLogLevel(JobMonitor::LogInfo);
	    } else if (options.logLevel_ == "warning") {
		jobMon.setLogLevel(JobMonitor::LogWarning);
	    } else if (options.logLevel_ == "error") {
		jobMon.setLogLevel(JobMonitor::LogError);
	    } else {
		cerr << "sjm: unknown log level " << options.logLevel_ << endl;
	    }
	}
	if (!jobMon.submit(options.jobFile_, options.outFile_,
			   options.logFile_, options.interactive_,
			   options.mailAddr_)) {
	    exit(1);
	}
    }
    exit(0);
}
