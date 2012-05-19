// featbin/fmpe-apply-transform.cc

// Copyright 2012  Johns Hopkins University (Author: Daniel Povey)  Yanmin Qian

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

#include "base/kaldi-common.h"
#include "util/common-utils.h"
#include "transform/fmpe.h"

int main(int argc, char *argv[]) {
  using namespace kaldi;
  using kaldi::int32;
  try {
    const char *usage =
        "Apply fMPE transform to features\n"
        "Usage:  fmpe-apply-transform [options...] <fmpe-object> "
        "<feat-rspecifier> <gselect-rspecifier> <feat-wspecifier>\n";

    ParseOptions po(usage);
    bool add_to_features = true;
    po.Register("add-to-features", &add_to_features, "If true, add original "
                "features to fMPE offsets (false useful for diagnostics)");
    // no non-default options.
    po.Read(argc, argv);

    if (po.NumArgs() != 4) {
      po.PrintUsage();
      exit(1);
    }

    std::string fmpe_rxfilename = po.GetArg(1),
        feat_rspecifier = po.GetArg(2),
        gselect_rspecifier = po.GetArg(3),
        feat_wspecifier = po.GetArg(4);
    
    Fmpe fmpe;
    ReadKaldiObject(fmpe_rxfilename, &fmpe);

    SequentialBaseFloatMatrixReader feat_reader(feat_rspecifier);
    RandomAccessInt32VectorVectorReader gselect_reader(gselect_rspecifier);
    BaseFloatMatrixWriter feat_writer(feat_wspecifier);

    int32 num_done = 0, num_err = 0;
    
    for (; !feat_reader.Done(); feat_reader.Next()) {
      std::string key = feat_reader.Key();
      const Matrix<BaseFloat> feat_in(feat_reader.Value());
      if (!gselect_reader.HasKey(key)) {
        KALDI_WARN << "No gselect information for key " << key;
        num_err++;
        continue;
      }
      const std::vector<std::vector<int32> > &gselect =
          gselect_reader.Value(key);
      if (static_cast<int32>(gselect.size()) != feat_in.NumRows()) {
        KALDI_WARN << "gselect information has wrong size";
        num_err++;
        continue;
      }
      Matrix<BaseFloat> feat_out(feat_in.NumRows(), feat_in.NumCols());
      fmpe.ComputeFeatures(feat_in, gselect, &feat_out);
      if (add_to_features) // feat_out += feat_in.
        feat_out.AddMat(1.0, feat_in, kNoTrans);

      feat_writer.Write(key, feat_out);
      num_done++;
    }
    KALDI_LOG << " Done " << num_done << " utterances, " << num_err
              << " had errors.";
    return (num_done != 0 ? 0 : 1);
  } catch(const std::exception& e) {
    std::cerr << e.what();
    return -1;
  }
}
