# SJM: a simple job manager
Phil Lacroute
April 2008

Copyright Notice
================

Copyright (c) 2008-2012, The Board of Trustees of The Leland Stanford
Junior University.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

   * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

   * Neither the name of Stanford University nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL STANFORD
UNIVERSITY BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Overview
========

The purpose of sjm is to manage groups of related jobs running on a
compute cluster.  The input to the program is a file containing a
description of the jobs that need to be run, the resources they
require and the dependencies between them.  sjm can do the following:

+ sjm dispatches each job to the batch queuing system once all of its
  dependencies have been satisfied
+ sjm monitors the progress of all running jobs and produces both a
  detailed log and a concise summary indicating any jobs that failed
+ sjm makes it easy to rerun any jobs that failed and the jobs that
  depend on them, without rerunning jobs that finished successfully
+ sjm can run jobs directly on the submission host as well as through
  the batch queuing system; this is useful if some tasks require
  resources only available on the head node of a cluster, such as
  network access to remote hosts
+ sjm can submit jobs using Platform LSF or Sun Grid Engine

Job Description File
====================

1. Example

    The input to sjm is a job description file.  Here is a simple example:
    ```
    # example sjm input file
    job_begin
      name jobA
      time 4h
      memory 3G
      queue standard
      project sequencing
      cmd /home/lacroute/project/jobA.sh
    job_end
    job_begin
      name jobB
      time 2d
      memory 1G
      queue extended
      cmd_begin
        /home/lacroute/project/jobB_prolog.sh;
        /home/lacroute/project/jobB.sh;
        /home/lacroute/project/jobB_epilog.sh
      cmd_end
    job_end
    order jobA before jobB
    log_dir /home/lacroute/project/log
    ```
    This description defines two jobs names jobA and jobB.  Each job has an optional time limit, an optional memory requirement, an optional submission queue, and a command to run.  The "order" statement specifies that jobA must finish before jobB can start.  Finally, the "log_dir" statement specifies a directory where the output of each command will be written.

2. Job Specification Blocks

	A job specification block has the form:
    ```
    job_begin
      STATEMENT1
      STATEMENT2
      ...
    job_end
    ```
    Each statement must be on a single line.  The following statements are allowed within the job specification block:

    `name` STRING
      [REQUIRED]
      The name of the job.  This name will be used in the log files and
      the batch queuing system.  Each job must have a unique name.

    `slots` NUMBER
      [OPTIONAL]
      The number of processor slots (i.e. CPU cores) the job will use.

    `parallel_env` NAME
      [OPTIONAL]
      The name of the parallel environment that defines the slot-allocation
      policy.

    `memory` AMOUNT
      [OPTIONAL]
      The maximum amount of memory the job will use.  The job may be
      killed if it exceeds this amount.  The AMOUNT consists of a number
      followed by "b" (bytes), "k" (kilobytes), "m" (megabytes)
      or "g" (gigabytes).  Capital letters are allowed and spaces are ok.
      For example:
         memory 2G  (2 gigabytes)
         memory 1500 m (1500 megabytes)

    `time` AMOUNT
      [OPTIONAL]
      The maximum amount of wallclock time the job will use.  The job may
      be killed if it exceeds this amount.  The AMOUNT consists of a
      number followed by "s" (seconds), "m" (minutes), "h" (hours)
      or "d" (days).  Capital letters are allowed and spaces are ok.
      For example:
        time 2d   (2 days)
        time 4 H  (4 hours)

    `queue` STRING
      [OPTIONAL]
      The name of the queue for job submission.

    `project` STRING [OPTIONAL]
      The name of the project the job is associated with.  Generally
      the name must be a project that has been registered with the job
      submission system.

    `cmd` STRING [REQUIRED]
      The command to run.  The command must be specified on a single
      line.  The command line will be interpreted by the shell so it
      can contain file redirection characters, etc.  For multi-line
      commands there is an alternate form (see the next statement type).

    ```
    cmd_begin
      CMD_LINE1
      CMD_LINE2
      ...
    cmd_end
    ```
      [REQUIRED]
      The command to run.  Use this form instead of "cmd" for multi-line
      commands.  The command will be interpreted by the shell.  Newlines
      are treated as spaces, so you can spread a single command over
      multiple lines.  If there are multiple commands then separate
      them with semicolons.

    ```
    export NAME
    export NAME=VALUE
    ```
      [OPTIONAL]
      An environment variable to pass to the command.  The first form
      copies the current value of the specified environment variable.  The
      second form sets the value explicitly.

    `module` NAME [OPTIONAL]
      An environment module that will be loaded when running the command.

    `directory` PATHNAME [OPTIONAL]
      A directory (specified as a full path) that will be used as the
      current working directory when running the command.

    `host` localhost [OPTIONAL]
      Run this job on the local host instead of submitting it to the batch system. if you compile sjm in a platform without sge or lsf, you do not need to use this field, because I have support this option via a Loc scheduler.

    `sched_options` STRING ... [OPTIONAL]
      Specify additional scheduler-specific options.  For Sun Grid Engine
      you may include qsub options that are not available through the
      other directives.

3. Other Statements

    Outside of a job specification block (not within the job_begin/job_end keywords) the following statements are allowed:

    ```
    order JOB1 before JOB2
    ```
      [OPTIONAL]
      JOB1 and JOB2 are job names.  JOB1 must finish before JOB2 can start.
    
    ```
    order JOB2 after JOB1
    ```
    [OPTIONAL]
      JOB1 and JOB2 are job names.  JOB1 must finish before JOB2 can start.

    `log_dir` STRING
      [OPTIONAL]
      Specifies a directory where all of the output from each job will be
      stored.  Standard output will go into a file named JOBNAME_oTIME.txt
      and standard error will go into a file named JOBNAME_eTIME.txt where
      JOBNAME is the name of the job and TIME is the time when sjm started
      running.  Both of these files will be in the directory specified by
      this statement.  Note that this statement only applies to jobs
      submitted through the batch system and not to jobs run on the local
      host (use command-line I/O redirection in the job description for local
      host jobs).

    The job specification file may also contain comment lines beginning with a "#".

Running sjm
===========

Run sjm as follows:
```
sjm [options] JOB_FILE
```
JOB_FILE is the job description file. For a description of all the options, run `sjm --help`. Most of them allow you to adjust parameters but usually the defaults are fine.  The most useful options are:

`-i`  run sjm in the foreground and print log messages to the terminal;
    the default is to run in the background and store log messages in
    a file names JOB_FILE.status.log

`-r`  instead of dispatching the jobs, output a graphical representation
    of the job dependency graph in "dot" format.  See www.graphviz.org
    for programs to display this format.

<img src="example.svg">

sjm produces three output files:

1. The log file (JOB_FILE.status.log by default, or standard output with
   `-i` option) contains a running log of all the jobs sjm submits and
   has a summary at the end indicating which jobs completed
   successfully.

2. The status file (JOB_FILE.status by default) contains detailed
   information about the status of each job.  It can be used to rerun
   sjm if some of the jobs fail.

3. The backup status file (JOB_FILE.status.bak by default) is a
   slightly-older backup copy of the status file.  Use this file if
   sjm crashes while updating the status file and leaves it in a
   corrupted state (hopefully very rare!)

If the log file indicates that some jobs failed, once you have
determined the cause and fixed anything that needs to be modified you
can rerun the failed jobs by running sjm on the status file or the
backup status file:

```
cp JOB_FILE.status JOB_FILE2
sjm JOB_FILE2
```

The job summary in the log file consists of lines that look like:
```
job0 (5714): 1:23:06 (513 sec, 120/167 MB)
```
The fields are:

| Description | Filed example|
|-------------|--------------|
| job name     | job0 |
| batch system job ID | 5714 |
| wallclock time | 1 hour, 23 minutes, 6 seconds|
| CPU time | 513 sec |
| maximum virtual memory usage | 120 MB |
| maximum swap usage |167 MB |


While sjm is running you can kill all the currently-running jobs by sending an interrupt to the sjm process.  If it was run with the `-i` (interactive) flag, just hit `CTRL`-`C`.  Otherwise, find the process id (e.g. `ps | fgrep sjm`) and then run `kill -INT process_id`.
