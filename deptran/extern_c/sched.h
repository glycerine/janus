#pragma once

#include "../scheduler.h"

//
// Created by Shuai on 6/25/17.
//

namespace janus {

class ExternCScheduler : public Scheduler {
  using Scheduler::Scheduler;
 public:
  virtual bool HandleConflicts(DTxn& dtxn,
                               innid_t inn_id,
                               vector<string>& conflicts) override;

};

} // namespace janus
