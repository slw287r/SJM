#include "task.h"

Task::Task(int j){
    joblist.resize(j);
}

Task::~Task(){
    for(auto& e: joblist){
        for(auto& f: e){
            if(f){
                delete f;
                f = NULL;
            }
        }
    }
}

void Task::addJob(Job* j, int i){
    joblist[i].push_back(j);
}

std::ostream& operator<<(std::ostream& os, const Task& t){
    std::vector<std::vector<std::string>> udlist;
    int udonen = 0; // undone stage 
    bool stgdone = true; // stage all done or not
    // output undone job definition
    for(auto& e: t.joblist){
        stgdone = true;
        for(auto& f: e){
            if(f->status.second != "done"){
                stgdone = false;
                os << (*f);
            }
        }
        if(!stgdone) ++udonen;
    }
    // logdir 
    if(!t.logdir.second.empty()){
        os << "  " << t.logdir.first << " " << t.logdir.second << "\n";
    }
    // skip one stage task
    if(t.joblist.size() <= 1){
        return os;
    }
    // reconstruct undone task joblist
    udlist.resize(udonen);
    udonen = 0;
    for(size_t i = 0; i < t.joblist.size(); ++i){
        stgdone = true;
        for(auto& e: t.joblist[i]){
            if(e->status.second != "done"){
                udlist[udonen].push_back(e->name.second);
                stgdone = false;
            }
        }
        if(!stgdone) ++udonen;
    }
    // output job order
    for(size_t i = 1; i < udlist.size(); ++i){
        for(auto& e: udlist[i-1]){
            for(auto& f: udlist[i]){
                os << "order  " << e << " before " << f << "\n";
            }
        }
    }
    return os;
 }
