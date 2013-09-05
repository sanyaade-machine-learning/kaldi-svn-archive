// cudamatrix/cu-sp-matrix-test.cc
//
// Copyright 2013  Ehsan Variani
//                 Lucas Ondel

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//  http://www.apache.org/licenses/LICENSE-2.0

// THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
// WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
// MERCHANTABLITY OR NON-INFRINGEMENT.
// See the Apache 2 License for the specific language governing permissions and
// limitations under the License.
//
// UnitTests for testing cu-sp-matrix.h methods.
//

#include <iostream>
#include <vector>
#include <cstdlib>

#include "base/kaldi-common.h"
#include "cudamatrix/cu-device.h"
#include "cudamatrix/cu-tp-matrix.h"
#include "cudamatrix/cu-vector.h"
#include "cudamatrix/cu-math.h"

using namespace kaldi;

namespace kaldi {

/*
 * INITIALIZERS
 */

/*
 * ASSERTS
 */
template<class Real>
static void AssertEqual(VectorBase<Real> &A, VectorBase<Real> &B, float tol = 0.001) {
  KALDI_ASSERT(A.Dim() == B.Dim());
  for (MatrixIndexT i = 0; i < A.Dim(); i++)
    KALDI_ASSERT(std::abs(A(i)-B(i)) < tol);
}

template<class Real>
static bool ApproxEqual(VectorBase<Real> &A, VectorBase<Real> &B, float tol = 0.001) {
  KALDI_ASSERT(A.Dim() == B.Dim());
  for (MatrixIndexT i = 0; i < A.Dim(); i++)
    if (std::abs(A(i)-B(i)) > tol) return false;
  return true;
}

template<class Real>
static void AssertEqual(const CuPackedMatrix<Real> &A,
                        const CuPackedMatrix<Real> &B,
                        float tol = 0.001) {
  KALDI_ASSERT(A.NumRows() == B.NumRows());
  for (MatrixIndexT i = 0; i < A.NumRows(); i++)
    for (MatrixIndexT j = 0; j <= i; j++)
      KALDI_ASSERT(std::abs(A(i, j) - B(i, j))
                   < tol * std::max(1.0, (double) (std::abs(A(i, j)) + std::abs(B(i, j)))));
}

template<class Real>
static void AssertEqual(const PackedMatrix<Real> &A,
                        const PackedMatrix<Real> &B,
                        float tol = 0.001) {
  KALDI_ASSERT(A.NumRows() == B.NumRows());
  for (MatrixIndexT i = 0; i < A.NumRows(); i++)
    for (MatrixIndexT j = 0; j <= i; j++)
      KALDI_ASSERT(std::abs(A(i, j) - B(i, j))
                   < tol * std::max(1.0, (double) (std::abs(A(i, j)) + std::abs(B(i, j)))));
}

template<class Real>
static void AssertEqual(const PackedMatrix<Real> &A,
                        const CuPackedMatrix<Real> &B,
                        float tol = 0.001) {
  KALDI_ASSERT(A.NumRows() == B.NumRows());
  for (MatrixIndexT i = 0; i < A.NumRows(); i++)
    for (MatrixIndexT j = 0; j <= i; j++)
      KALDI_ASSERT(std::abs(A(i, j) - B(i, j))
                   < tol * std::max(1.0, (double) (std::abs(A(i, j)) + std::abs(B(i, j)))));
}

/*
 * Unit Tests
 */
template<class Real>
static void UnitTestCuTpMatrixInvert() {
  for (MatrixIndexT i = 1; i < 10; i++) {
    MatrixIndexT dim = 5 * i + rand() % 10;
    
    TpMatrix<Real> A(dim);
    A.SetRandn();
    CuTpMatrix<Real> B(A);
    
    AssertEqual<Real>(A, B, 0.005);
    A.Invert();
    B.Invert();
    AssertEqual<Real>(A, B, 0.005);
  }
}

template<class Real>
static void UnitTestCuTpMatrixCopyFromTp() {
  for (MatrixIndexT i = 1; i < 10; i++) {
    MatrixIndexT dim = 5 * i + rand() % 10;
    
    TpMatrix<Real> A(dim);
    A.SetRandn();
    CuTpMatrix<Real> B(dim);
    B.CopyFromTp(A);
    CuTpMatrix<Real> C(dim);
    C.CopyFromTp(B);
    
    AssertEqual<Real>(A, B);
    AssertEqual<Real>(B, C);
  }
}

template<class Real>
static void UnitTestCuTpMatrixCholesky() {
  for (MatrixIndexT i = 1; i < 10; i++) {
    MatrixIndexT dim = 5 * i + rand() % 10;
    
    SpMatrix<Real> A(dim);
    A.SetRandn();
    CuSpMatrix<Real> B(A);

    TpMatrix<Real> C(dim);
    C.SetRandn();
    CuTpMatrix<Real> D(C);
    C.Cholesky(A);
    D.Cholesky(B);
    
    AssertEqual<Real>(C, D);
  }
}


template<class Real> void CudaTpMatrixUnitTest() {
  UnitTestCuTpMatrixInvert<Real>();
  UnitTestCuTpMatrixCopyFromTp<Real>();
}

} // namespace kaldi


int main() {
  using namespace kaldi;


  for (int32 loop = 0; loop < 2; loop++) {
#if HAVE_CUDA == 1
    if (loop == 0)
      CuDevice::Instantiate().SelectGpuId(-1); // -1 means no GPU
    else
      CuDevice::Instantiate().SelectGpuId(-2); // -2 .. automatic selection
#endif
    kaldi::CudaTpMatrixUnitTest<float>();
#if HAVE_CUDA == 1
    if (CuDevice::Instantiate().DoublePrecisionSupported()) {
      kaldi::CudaTpMatrixUnitTest<double>();
    } else {
      KALDI_WARN << "Double precision not supported";
    }
#else
    kaldi::CudaTpMatrixUnitTest<double>();
#endif
  
    if (loop == 0)
      KALDI_LOG << "Tests without GPU use succeeded.\n";
    else
      KALDI_LOG << "Tests with GPU use (if available) succeeded.\n";
  }
#if HAVE_CUDA == 1
  CuDevice::Instantiate().PrintProfile();
#endif
  return 0;
}