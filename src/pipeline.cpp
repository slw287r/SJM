#include <future>
#include <fstream>
#include "pipeline.h"

Pipeline::Pipeline(int n, const std::string& fmkf, const std::string& smkf, const int32_t tryn){
    goodMarkFile = smkf;
    failMarkFile = fmkf;
    pipelist.resize(n);
    retryNum = tryn;
}

Pipeline::~Pipeline(){
    for(auto& e: pipelist){
        for(auto& f: e){
            if(f){
                delete f;
                f = NULL;
            }
        }
    }
}

void Pipeline::addRunFile(RunTask* r, int s){
    pipelist[s].push_back(r);
}

void Pipeline::prepareRerun(){
    std::string oriSJM, newSJM, bakSJM;
    for(auto& e: pipelist){
        for(auto& f: e){
            remove(f->failMarkFile.c_str());
            remove(f->goodMarkFile.c_str());
            remove(f->logFile.c_str());
            auto p = f->sjmCMD.find_last_of(" ");
            oriSJM = f->sjmCMD.substr(p + 1);
            newSJM = oriSJM + ".status";
            bakSJM = newSJM + ".bak";
            remove(bakSJM.c_str());
            std::ifstream fr(newSJM);
            if(fr.is_open()){
                fr.close();
                if(!forceUpdateSJM){
                    std::rename(newSJM.c_str(), oriSJM.c_str());
                }
            }
        }
    }
}

int Pipeline::runPipeline(){
    std::vector<std::future<int>> fv(pipelist.size());
    for(size_t s = 0; s < pipelist.size(); ++s){
        fv[s] = std::async(std::launch::deferred, &Pipeline::runStage, this, s);
    }
    for(auto& e: fv){
        retValue += std::abs(e.get());
        if(retValue){
            remove(goodMarkFile.c_str());
            std::ofstream fw(failMarkFile.c_str());
            fw.close();
            return retValue;
        }
    }
    if(retValue){
        remove(goodMarkFile.c_str());
        std::ofstream fw(failMarkFile.c_str());
        fw.close();
    }else{
        remove(failMarkFile.c_str());
        std::ofstream fw(goodMarkFile);
        fw.close();
    }
    return retValue;
}

int Pipeline::runStage(int s){
    int ret = 0;
    std::vector<std::future<int>> fv(pipelist[s].size());
    for(size_t t = 0; t < pipelist[s].size(); ++t){
        fv[t] = std::async(std::launch::async, &Pipeline::runTask, this, std::ref(pipelist[s][t]));
    }
    for(size_t t = 0; t < fv.size(); ++t){
        pipelist[s][t]->retValue = fv[t].get();
        if(pipelist[s][t]->retValue){
            remove(pipelist[s][t]->goodMarkFile.c_str());
            std::ofstream fw(pipelist[s][t]->failMarkFile);
            fw.close();
            ret = 1;
        }else{
            remove(pipelist[s][t]->failMarkFile.c_str());
            std::ofstream fw(pipelist[s][t]->goodMarkFile);
            fw.close();
        }
    }
    return ret;
}

int Pipeline::runTask(RunTask* r){
    for(int i = 0; i <= retryNum; ++i){
        if(!std::system(r->sjmCMD.c_str())){
            return 0;
        }
    }
    return 1;
}
