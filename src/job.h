#ifndef JOB_H
#define JOB_H

#include <map>
#include <string>
#include <utility>
#include <fstream>
#include <sstream>
#include <iostream>

/** Job class to represent a execute unit in sjm control pipeline */
class Job{
    public:
        std::string begin = "job_begin";   ///< placeholder of begining of a execution unit
        std::string end = "job_end";       ///< placeholder of begining of a execution unit
        std::string sep = "  ";            ///< delimiters used for indentation
        std::pair<std::string, std::string> name = {"name", ""};            ///< job name pair
        std::pair<std::string, std::string> memory = {"memory", ""};        ///< hardlimt of memory, a job used more than this will be killed
        std::pair<std::string, std::string> queue = {"queue", ""};          ///< queue name to submit this job
        std::pair<std::string, std::string> slots = {"slots", ""};          ///< slots in PE if write it into sjm, so I use it as a convenient key to specify memory
        std::pair<std::string, std::string> cmd = {"cmd", ""};              ///< commands of this job
        std::pair<std::string, std::string> envexp = {"export", ""};        ///< environment variables to be exported before executing this job
        std::pair<std::string, std::string> workdir = {"directory", ""};    ///< working directory when executing this job
        std::pair<std::string, std::string> host = {"host", ""};            ///< hostname executing this job
        std::pair<std::string, std::string> sopt = {"sched_options", "-V"}; ///< optional options passed to the job manager(sge.etc)
        std::pair<std::string, std::string> status = {"status", ""};        ///< job running staus marker(job will be passed without executing its cmd if set to done)
        int stage_marker = 0;                                               ///< stage marker of this job
        std::string o1;                                                     ///< output 1 file path generated in this job
        std::string o2;                                                     ///< output 2 file path generated in this job
        std::string pre;                                                    ///< prefix used for various filenames generated in this job
    public:
        /** construct a Job object to represent a job
         * @param jname job name
         * @param jout  job results output
         * @param jpre prefix used for output/log filename
         * @param jstage job stage marker
         */
        Job(const std::string& jname, const std::string& jout, const std::string& jpre, const int& jstage);
        
        /** default constructor of Job class */
        Job();
        
        /** deafult destructor of Job class */
        ~Job();

        /** append an item in Job object to ostream
         * @param os reference of std::ostream object
         * @param item an <string, string> pair item in Job
         * @return os reference to std::ostream
         */
        static std::ostream& appendItem(std::ostream& os, const std::pair<std::string, std::string>& item);
        
        /** output sjm executing unit control information of a Job to ostream
         * @param os reference of std::ostream object
         * @param j reference of a Job object
         * @return os reference of std::ostream object
         */
        friend std::ostream& operator<<(std::ostream& os, const Job& j);
        
        /** output sjm executing unit control information of a Job to ostream
         * @param os reference of std::ostream object
         * @param j pointer to a Job object
         * @return os reference of std::ostream object
         */
        friend std::ostream& operator<<(std::ostream& os, const Job* j){
            os << (*j);
            return os;
        }

        /** get execution status of all jobs in a SJM status file
         * @param jmap <stage, status> map
         * @param jfile SJM status file
         */
        static void getStatus(std::map<std::string, std::string>& jmap, const std::string& jfile);
};

#endif
