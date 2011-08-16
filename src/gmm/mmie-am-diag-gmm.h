// gmm/mmie-am-diag-gmm.h

// Copyright 2009-2011  Arnab Ghoshal

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
// WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
// MERCHANTABLITY OR NON-INFRINGEMENT.
// See the Apache 2 License for the specific language governing permissions and
// limitations under the License.


#ifndef KALDI_GMM_MMIE_AM_DIAG_GMM_H_
#define KALDI_GMM_MMIE_AM_DIAG_GMM_H_ 1

#include <vector>

#include "gmm/am-diag-gmm.h"
#include "gmm/mmie-diag-gmm.h"

namespace kaldi {

/** \class MmieAmDiagGmm
 *  Class for computing the maximum mutual information estimate of the
 *  parameters of an acoustic model that uses diagonal Gaussian mixture models
 *  as emission densities.
 */
// TODO(arnab): maybe we don't really need a class, and can make this a function.
class MmieAmDiagGmm {
 public:
  MmieAmDiagGmm() {}
  ~MmieAmDiagGmm();

  void Update(const MleDiagGmmOptions &config, GmmFlagsType flags,
              AmDiagGmm *am_gmm, BaseFloat *obj_change_out,
              BaseFloat *count_out) const;

 private:
  // Cannot have copy constructor and assigment operator
  KALDI_DISALLOW_COPY_AND_ASSIGN(MmieAmDiagGmm);
};

}  // End namespace kaldi


#endif  // KALDI_GMM_MMIE_AM_DIAG_GMM_H_
