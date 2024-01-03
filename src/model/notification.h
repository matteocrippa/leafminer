#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <vector>
#include <string>
#include "utils/log.h"

struct Notification
{
    std::string job_id;
    std::string prevhash;
    std::string coinb1;
    std::string coinb2;
    std::vector<std::string> merkle_branch;
    std::string version;
    std::string nbits;
    std::string ntime;
    bool clean_jobs;

    // Constructor using member initialization list
    Notification(const std::string &job_id, const std::string &prevhash, const std::string &coinb1, const std::string &coinb2, const std::vector<std::string> &merkle_branch, const std::string &version, const std::string &nbits, const std::string &ntime, const bool &clean_jobs)
        : job_id(job_id),
          prevhash(prevhash),
          coinb1(coinb1),
          coinb2(coinb2),
          merkle_branch(merkle_branch),
          version(version),
          nbits(nbits),
          ntime(ntime),
          clean_jobs(clean_jobs)
    {
    }
};

#endif
