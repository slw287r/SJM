#ifndef PIPELINE_H
#define PIPELINE_H

#include <string>
#include <vector>
#include "task.h"

/** struct to store information of a Task */
struct RunTask{
    std::string sjmCMD;       ///< sjm execution command
    std::string failMarkFile; ///< file to touch if this task failed
    std::string goodMarkFile; ///< file to touch if this task succeeded
    std::string logFile;      ///< logging file of sjm execution of this task
    int retValue = 0;         ///< return value of this execution 
};

/** Pipeline class hold a series of Tasks */
class Pipeline{
    public:
        std::vector<std::vector<RunTask*>> pipelist; ///< tasks in pipeline[i] will execute parallely, and will be excecuted before pipeline[i+1]
        std::string failMarkFile;                    ///< file to touch if this pipeline failed
        std::string goodMarkFile;                    ///< file to touch if this pipeline succeeded
        int retValue = 0;                            ///< return value of this pipeline
        bool forceUpdateSJM = false;                 ///< force update sjm file of Task if true
        int retryNum = 1;                            ///< number of retries to do after failure

    public:
        /** default constructor of Pipeline */
        Pipeline() = default;

        /** construct a Pipeline which have n stages
         * @param n stages of Pipeline
         * @param fmkf fail marker file of Pipeline
         * @param smkf success marker file of Pipeline
         * @param tryn number of retries to do after failing of one job
         */
        Pipeline(int n, const std::string& fmkf, const std::string& smkf, const int32_t tryn = 1);

        /** destroy Pipeline */
        ~Pipeline();

        /** add a RunTask to pipeline
         * @param r pointer to a RunTask
         * @param s stage number of pipeline to add this RunTask to
         */
        void addRunFile(RunTask* r, int s);

        /** prepare Pipeline to resume running from last failure */
        void prepareRerun();
        
        /** runing a Task
         * @param r pointer to RunTask object
         */
        int runTask(RunTask* r);

        /** running a stage of a pipeline
         * @param s stage of pipeline to run
         */
        int runStage(int s);
        
        /** execute Pipeline */
        int runPipeline();
};

#endif
