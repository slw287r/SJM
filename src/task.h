#ifndef TASK_H
#define TASK_H

#include <vector>
#include <string>
#include <iostream>
#include <utility>
#include "job.h"

/** Task class to hold a series of jobs in the order they are executed */
class Task{
    public:
        std::vector<std::vector<Job*>> joblist;                       ///< jobs in joblist[i] will execute before joblist[i+1], jobs in each joblist[i] execute parallely
        std::pair<std::string, std::string> logdir = {"log_dir", ""}; ///< logging directory of this task

    public:
        /** default constructor of Task */
        Task()=default;

        /** construct a Task which can hold n stages of jobs
         * @param n stages of this Task
         */
        Task(int n);

        /** destructor of Task */
        ~Task();

        /** add a Job to stage i in this Task
         * @param j pointer to a Job object
         * @param i stage at which this Job will be put
         */
        void addJob(Job* j, int i);

        /** operator to output a Task into a sjm control file
         * @param os reference of a std::ostream object
         * @param t reference of a Task object
         * @return os reference of a std::ostream object
         */
        friend std::ostream& operator<<(std::ostream& os, const Task& t);

        /** operator to output a Task into a sjm control file
         * @param os reference of a std::ostream object
         * @param t pointer to a Task object
         * @return os reference of a std::ostream object
         */
        friend std::ostream& operator<<(std::ostream& os, const Task* t){
            os << (*t);
            return os;
        }
};

#endif
