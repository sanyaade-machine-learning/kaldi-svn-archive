#include "character-matrix.h"
#include <iostream>
#include "kaldi-matrix.h"
#include <cstdio>
#include <ctime>
#include <time.h>
#include "util/kaldi-io.h"

namespace kaldi {

template<class Real> static void AssertEqual(const Matrix<Real> &A,
                                             const Matrix<Real> &B,
                                             float tol = 0.003) {
  KALDI_ASSERT(A.NumRows() == B.NumRows()&&A.NumCols() == B.NumCols());
  for (MatrixIndexT i = 0;i < A.NumRows();i++)
	for (MatrixIndexT j = 0;j < A.NumCols();j++) {
	  KALDI_ASSERT(std::abs(A(i, j)-B(i, j)) < tol);
    }
}
//
template <typename Real>
static void Norm(Matrix<Real> &M, Real * NormI, Real* NormII) {
  *NormI = static_cast<Real> (0);
  *NormII = static_cast<Real> (0);
  Real NormMax = static_cast<Real>(0);
  int32 num_rows = M.NumRows(),
        num_cols = M.NumCols();
   for(int32 row = 0; row < num_rows; ++row) {
    *NormI = static_cast<Real>(0) ;
      for(int32 col = 0; col < num_cols; ++col) {
        *NormII += M(row, col) * M(row, col) ;
        *NormI += fabs(M(row, col)) ;
      }
      if(*NormI > NormMax) NormMax = *NormI ;

   }
   *NormII = pow(*NormII, 0.5);
   *NormI = NormMax;
}

//
template <typename Real>
static void  GenerateMatrix4U (Matrix<Real> &M) {
  int32 num_rows = M.NumRows(), 
        num_cols = M.NumCols();
  Real min = static_cast<Real>(0);
  KALDI_ASSERT(num_rows > 0 && num_cols > 0);
  std::vector<Real> v(256);
  for(size_t i = 0; i < v.size(); i++) {
    v[i] = static_cast<Real>(i*1.0/255);
  }
  for(int32 row = 0; row < num_rows; ++row)
    for(int32 col = 0; col < num_cols; ++col) {
      int32  k = rand() % 256;
      M(row, col) = min + v[k];
    }
}
//
template <typename Real>
static void  GenerateMatrix4S (Matrix<Real> &M) {
   int32 num_rows = M.NumRows(), 
         num_cols = M.NumCols();
   KALDI_ASSERT(num_rows > 0 && num_cols > 0);
   Real base = static_cast<Real>(0);
   std::vector<Real> v(256);
   for(int32 i = -128, j=0; i < 128; i++, j++) {
     Real x;
     if(i <= 0) {
       x = static_cast<Real>(i*1.0/128);
     } else {
       x = static_cast<Real>(i*1.0/127);
     }
     v[j] = x;
   }
   for(int32 row = 0; row < num_rows; ++row)
    for(int32 col = 0; col < num_cols; ++col) {
      int32  k = rand() % 256;
      M(row, col) = base + v[k];
    }
}
template <typename Real>
static void  GenerateMatrixI (Matrix<Real> &M) {
   int32 num_rows = M.NumRows(),
         num_cols = M.NumCols();
   KALDI_ASSERT(num_rows > 0 && num_cols > 0);
   srand(time(NULL));
   Real base = static_cast<Real>(0);
   std::vector<Real> v(256);
   for(int32 row = 0; row < num_rows; ++row) {
    for(int32 col = 0; col < num_cols; ++col) {
      if( row == col ) {
         M(row, col) = static_cast<Real>(1) ;
       } else {
    	 M(row, col) = base;
    }
 }
 }
}
// use gdb to display these
template<typename Real>
static void ShowMatrix(const Matrix<Real> &M) {
  std::cout << "\nrow=" << M.NumRows()
            << ", col=" << M.NumCols()
            << "\nmax=" << M.Max() 
            <<", min=" <<M.Min() << "\n";
  for(int32 row = 0; row < M.NumRows(); ++row) {
    for(int32 col = 0; col < M.NumCols(); ++col) {
      std::cout << M(row,col) << " ";
    }
    std::cout << "\n";
  }
}
//
template<typename Real> 
static void TestCopyMat02() {
  int32 num_rows = 2;
  int32 num_cols = 10;
  Matrix<Real> M(num_rows,num_cols);
  Real res = static_cast<Real>(1.0/ 10);
  Real base;
  for(int32 i = 0; i < M.NumRows(); i++) {
     base = 0;
    for(int32 j = 0; j < M.NumCols(); j++) {
      M(i,j) = base + res;
      base += res;
    }
  }
  CharacterMatrix<unsigned char> C;
  C.CopyFromMat(M);
  Matrix<Real> M1;
  C.RecoverMatrix(M1);
  for(int32 i = 0; i < M1.NumRows() ; i++) {
    for(int32 j = 0; j < M1.NumCols(); j++) {
      Real x = M1(i,j);
      std::cout << x << " ";
    }
    std::cout << "\n";
  }
  AssertEqual(M,M1);
}

template<typename Real>
static void TestAddMatMatError(int32 MatNum ) {
  Real error_avg = static_cast<Real> (0);
  Real error_std = static_cast<Real> (0);
  std::ostringstream os;
  os << "temp_err_sum_" << MatNum;
  Output ko(os.str(), false, false);
 
  srand(time(NULL));
  ko.Stream() << " Number Of Matrices : " << MatNum << "\n";
  std::vector<Real> v(MatNum);
  for(int32 i = 0; i< MatNum; ++i) {
    // srand(time(NULL));
    int32  row = 100 + rand() % 10;
    int32  col = 100 + rand() % 10;
    Matrix<Real> M1(row, col);
    GenerateMatrix4U(M1);
   
    int32 row2 = 100 + rand() % 10;
    Matrix<Real> M2(row2,col);
    GenerateMatrix4S(M2);

    Matrix<Real> M(row,row2);
    M.AddMatMat(1.0, M1, kNoTrans, M2, kTrans, 0);
   
    CharacterMatrix<unsigned char> Mc1;
    Mc1.CopyFromMat(M1);
    CharacterMatrix<signed char> Mc2;
    Mc2.CopyFromMat(M2);
    Matrix<Real> Mc(row,row2);
    Mc.AddMatMat(1.0, Mc1, kNoTrans, Mc2, kTrans, 0);
    Matrix<Real> diff(M);
    Matrix<Real> Identity(row2,row2);
    GenerateMatrixI(Identity);
    diff.AddMatMat(1.0, Mc, kNoTrans, Identity, kNoTrans, -1.0);

    Real NormI_Dan;
    Real NormII_Dan;
    Norm(M, &NormI_Dan, &NormII_Dan); 
    Real NormI_diff;
    Real NormII_diff;
    Norm( diff, &NormI_diff, &NormII_diff);
    //std::cout<<" Initial Matrix: NormI: "<<NormI_Dan<<" Norm II : "<<NormII_Dan<<std::endl;
    //std::cout<<" The error Matrix: NormI: "<<NormI_diff<<" Norm II : "<<NormII_diff<<std::endl;
    ko.Stream() << " The relative error NormI :"
       << NormI_diff/NormI_Dan/row/row2 
       << " Norm II : " << NormII_diff/NormII_Dan/row/row2<<std::endl;
    v[i] = NormII_diff/NormII_Dan/row/row2 ;
    error_avg += v[i];
    sleep(1);
  }
  error_avg = error_avg/MatNum;
  for(int32 i = 0; i< MatNum; ++i) {
     error_std += pow((v[i]-error_avg), 2);
  }
  error_std = pow( error_std/MatNum, 0.5);
  ko.Stream() << " Average Error is : " 
     << error_avg << " Standard Deviation : "
     << error_std << std::endl; 
  ko.Close();
  std::cout << " Average Error is : " 
     << error_avg << " Standard Deviation : "
     << error_std << std::endl; 
}
//
template<typename Real>
static void TestAddMatMatTime (int32 numTest) {
  std::ostringstream os;
  os << "temp_time_sum_" << numTest;
  Output ko(os.str(), false,false);

  double tot_ft1 = 0, 
         tot_ft2 = 0;
  for(int32 i =0; i < numTest; i++) {
    ko.Stream() << "\nround " << i+1 << "\n";
    int32  row = 2000 + rand() % 5;
    int32  col = 2000 + rand() % 7;

    Matrix<Real> M1(row, col);
    GenerateMatrix4U(M1);
    int32 row2 = 400 + rand() % 4;
    Matrix<Real> M2(row2,col);
    GenerateMatrix4S(M2);
    Matrix<Real> M(row,row2);
    clock_t start = std::clock();
    M.AddMatMat(1.0, M1, kNoTrans, M2, kTrans, 0);
    tot_ft1 += (std::clock() - start) / (double)CLOCKS_PER_SEC;

    ko.Stream() << "\nMax=" << M.Max()
              << ", Min=" << M.Min() << "\n";

    CharacterMatrix<unsigned char> Mc1;
    Mc1.CopyFromMat(M1);
    CharacterMatrix<signed char> Mc2;
    Mc2.CopyFromMat(M2);
    Matrix<Real> Mc(row,row2);
    start = std::clock();   
    Mc.AddMatMat(1.0, Mc1, kNoTrans, Mc2, kTrans, 0);
    tot_ft2 += (std::clock() - start) / (double)CLOCKS_PER_SEC;
    ko.Stream() << "\ncMax=" << Mc.Max()
              << ", cMin=" << Mc.Min() << "\n";

    sleep(1);
  }
  ko.Stream() << "\nfloat_AddMatMat=" << tot_ft1 
            << ", char_AddMatMat=" << tot_ft2 
            << ",rate=" << tot_ft2/tot_ft1 << "\n";
  std::cout << "Time consumed: float_AddMatMat=" << tot_ft1 << ", char_AddMatMat=" << tot_ft2 << ",rate=" << tot_ft2/tot_ft1 << std::endl;
  ko.Close();
}


} // kaldi namespace

int main() {
  kaldi::TestAddMatMatError<float>(5);
  kaldi::TestAddMatMatTime<float>(3); 
  KALDI_LOG << "character-matrix-test succeeded.\n";
  return 0;
}
