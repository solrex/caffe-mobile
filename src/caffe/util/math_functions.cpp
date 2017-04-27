#ifdef USE_BOOST
#include <boost/math/special_functions/next.hpp>
#include <boost/random.hpp>
#else
#include <math.h>
#endif // USE_BOOST

#include <limits>

#include "caffe/common.hpp"
#include "caffe/util/math_functions.hpp"
#include "caffe/util/rng.hpp"

#if defined(__APPLE__) && defined(__MACH__)
#include <vecLib.h>
#elif defined(USE_NEON_MATH) && defined(__ARM_NEON)
#include <arm_neon.h>
#endif

namespace caffe {

template<>
void caffe_cpu_gemm<float>(const CBLAS_TRANSPOSE TransA,
    const CBLAS_TRANSPOSE TransB, const int M, const int N, const int K,
    const float alpha, const float* A, const float* B, const float beta,
    float* C) {
  int lda = (TransA == CblasNoTrans) ? K : M;
  int ldb = (TransB == CblasNoTrans) ? N : K;
  cblas_sgemm(CblasRowMajor, TransA, TransB, M, N, K, alpha, A, lda, B,
      ldb, beta, C, N);
}

template<>
void caffe_cpu_gemm<double>(const CBLAS_TRANSPOSE TransA,
    const CBLAS_TRANSPOSE TransB, const int M, const int N, const int K,
    const double alpha, const double* A, const double* B, const double beta,
    double* C) {
  int lda = (TransA == CblasNoTrans) ? K : M;
  int ldb = (TransB == CblasNoTrans) ? N : K;
  cblas_dgemm(CblasRowMajor, TransA, TransB, M, N, K, alpha, A, lda, B,
      ldb, beta, C, N);
}

template <>
void caffe_cpu_gemv<float>(const CBLAS_TRANSPOSE TransA, const int M,
    const int N, const float alpha, const float* A, const float* x,
    const float beta, float* y) {
  cblas_sgemv(CblasRowMajor, TransA, M, N, alpha, A, N, x, 1, beta, y, 1);
}

template <>
void caffe_cpu_gemv<double>(const CBLAS_TRANSPOSE TransA, const int M,
    const int N, const double alpha, const double* A, const double* x,
    const double beta, double* y) {
  cblas_dgemv(CblasRowMajor, TransA, M, N, alpha, A, N, x, 1, beta, y, 1);
}

template <>
void caffe_axpy<float>(const int N, const float alpha, const float* X,
    float* Y) { cblas_saxpy(N, alpha, X, 1, Y, 1); }

template <>
void caffe_axpy<double>(const int N, const double alpha, const double* X,
    double* Y) { cblas_daxpy(N, alpha, X, 1, Y, 1); }

template <typename Dtype>
void caffe_set(const int N, const Dtype alpha, Dtype* Y) {
  if (alpha == 0) {
    memset(Y, 0, sizeof(Dtype) * N);  // NOLINT(caffe/alt_fn)
    return;
  }
  for (int i = 0; i < N; ++i) {
    Y[i] = alpha;
  }
}

#ifdef __VECLIB__
template <>
void caffe_set(const int N, const float alpha, float* Y) {
  vDSP_vfill(&alpha, Y, 1, N);
}

template <>
void caffe_set(const int N, const double alpha, double* Y) {
  vDSP_vfillD(&alpha, Y, 1, N);
}
#elif defined(__ARM_NEON_H)
template <>
void caffe_set(const int N, const float alpha, float* Y) {
  int tail_frames = N % 4;
  const float* end = Y + N - tail_frames;
  while (Y < end) {
    float32x4_t alpha_dup = vld1q_dup_f32(&alpha);
    vst1q_f32(Y, alpha_dup);
    Y += 4;
  }
  for (int i = 0; i < tail_frames; ++i) {
    Y[i] = alpha;
  }
}
#endif

template void caffe_set<int>(const int N, const int alpha, int* Y);
template void caffe_set<float>(const int N, const float alpha, float* Y);
template void caffe_set<double>(const int N, const double alpha, double* Y);

template <>
void caffe_add_scalar(const int N, const float alpha, float* Y) {
#ifdef __VECLIB__
  vDSP_vsadd(Y, 1, &alpha, Y, 1, N);
#elif defined(__ARM_NEON_H)
  int tail_frames = N % 4;
  const float* end = Y + N - tail_frames;
  while (Y < end) {
    float32x4_t a_frame = vld1q_f32(Y);
    float32x4_t alpha_dup = vld1q_dup_f32(&alpha);
    vst1q_f32(Y, vaddq_f32(a_frame, alpha_dup));
    Y += 4;
  }
  for (int i = 0; i < tail_frames; ++i) {
    Y[i] += alpha;
  }
#else
  for (int i = 0; i < N; ++i) {
    Y[i] += alpha;
  }
#endif
}

template <>
void caffe_add_scalar(const int N, const double alpha, double* Y) {
#ifdef __VECLIB__
  vDSP_vsaddD(Y, 1, &alpha, Y, 1, N);
#else
  for (int i = 0; i < N; ++i) {
    Y[i] += alpha;
  }
#endif
}

template <typename Dtype>
void caffe_copy(const int N, const Dtype* X, Dtype* Y) {
  if (X != Y) {
    if (Caffe::mode() == Caffe::GPU) {
#ifndef CPU_ONLY
      // NOLINT_NEXT_LINE(caffe/alt_fn)
      CUDA_CHECK(cudaMemcpy(Y, X, sizeof(Dtype) * N, cudaMemcpyDefault));
#else
      NO_GPU;
#endif
    } else {
      memcpy(Y, X, sizeof(Dtype) * N);  // NOLINT(caffe/alt_fn)
    }
  }
}

template void caffe_copy<int>(const int N, const int* X, int* Y);
template void caffe_copy<unsigned int>(const int N, const unsigned int* X,
    unsigned int* Y);
#if defined(__ARM_NEON_H)
template <>
void caffe_copy<float>(const int N, const float* X, float* Y) {
  int tail_frames = N % 4;
  const float* end = Y + N - tail_frames;
  while (Y < end) {
    float32x4_t x_frame = vld1q_f32(X);
    vst1q_f32(Y, x_frame);
    X += 4;
    Y += 4;
  }
  for (int i = 0; i < tail_frames; ++i) {
    Y[i] = X[i];
  }
}
#else
template void caffe_copy<float>(const int N, const float* X, float* Y);
#endif

template void caffe_copy<double>(const int N, const double* X, double* Y);

template <>
void caffe_scal<float>(const int N, const float alpha, float *X) {
  cblas_sscal(N, alpha, X, 1);
}

template <>
void caffe_scal<double>(const int N, const double alpha, double *X) {
  cblas_dscal(N, alpha, X, 1);
}

template <>
void caffe_cpu_axpby<float>(const int N, const float alpha, const float* X,
                            const float beta, float* Y) {
  cblas_saxpby(N, alpha, X, 1, beta, Y, 1);
}

template <>
void caffe_cpu_axpby<double>(const int N, const double alpha, const double* X,
                             const double beta, double* Y) {
  cblas_daxpby(N, alpha, X, 1, beta, Y, 1);
}

template <>
void caffe_add<float>(const int n, const float* a, const float* b,
    float* y) {
#ifdef __VECLIB__
  vDSP_vadd(a, 1, b, 1, y, 1, n);
#elif defined(__ARM_NEON_H)
  int tail_frames = n % 4;
  const float* end = y + n - tail_frames;
  while (y < end) {
    float32x4_t a_frame = vld1q_f32(a);
    float32x4_t b_frame = vld1q_f32(b);
    vst1q_f32(y, vaddq_f32(a_frame, b_frame));
    a += 4;
    b += 4;
    y += 4;
  }
  vsAdd(tail_frames, a, b, y);
#else
  vsAdd(n, a, b, y);
#endif
}

template <>
void caffe_add<double>(const int n, const double* a, const double* b,
    double* y) {
#ifdef __VECLIB__
  vDSP_vaddD(a, 1, b, 1, y, 1, n);
#else
  vdAdd(n, a, b, y);
#endif
}

template <>
void caffe_sub<float>(const int n, const float* a, const float* b,
    float* y) {
#ifdef __VECLIB__
  vDSP_vsub(a, 1, b, 1, y, 1, n);
#elif defined(__ARM_NEON_H)
  int tail_frames = n % 4;
  const float* end = y + n - tail_frames;
  while (y < end) {
    float32x4_t a_frame = vld1q_f32(a);
    float32x4_t b_frame = vld1q_f32(b);
    vst1q_f32(y, vsubq_f32(a_frame, b_frame));
    a += 4;
    b += 4;
    y += 4;
  }
  vsSub(tail_frames, a, b, y);
#else
  vsSub(n, a, b, y);
#endif
}

template <>
void caffe_sub<double>(const int n, const double* a, const double* b,
    double* y) {
#ifdef __VECLIB__
  vDSP_vsubD(a, 1, b, 1, y, 1, n);
#else
  vdSub(n, a, b, y);
#endif
}

template <>
void caffe_mul<float>(const int n, const float* a, const float* b,
    float* y) {
#ifdef __VECLIB__
  vDSP_vmul(a, 1, b, 1, y, 1, n);
#elif defined(__ARM_NEON_H)
  int tail_frames = n % 4;
  const float* end = y + n - tail_frames;
  while (y < end) {
    float32x4_t a_frame = vld1q_f32(a);
    float32x4_t b_frame = vld1q_f32(b);
    vst1q_f32(y, vmulq_f32(a_frame, b_frame));
    a += 4;
    b += 4;
    y += 4;
  }
  vsMul(tail_frames, a, b, y);
#else
  vsMul(n, a, b, y);
#endif
}

template <>
void caffe_mul<double>(const int n, const double* a, const double* b,
    double* y) {
#ifdef __VECLIB__
  vDSP_vmulD(a, 1, b, 1, y, 1, n);
#else
  vdMul(n, a, b, y);
#endif
}

template <>
void caffe_div<float>(const int n, const float* a, const float* b,
    float* y) {
#ifdef __VECLIB__
  vDSP_vdiv(b, 1, a, 1, y, 1, n);
#elif defined(__ARM_NEON_H)
  int tail_frames = n % 4;
  const float* end = y + n - tail_frames;
  while (y < end) {
    float32x4_t a_frame = vld1q_f32(a);
    float32x4_t b_frame = vld1q_f32(b);
    vst1q_f32(y, vdivq_f32(a_frame, b_frame));
    a += 4;
    b += 4;
    y += 4;
  }
  vsDiv(tail_frames, a, b, y);
#else
  vsDiv(n, a, b, y);
#endif
}

template <>
void caffe_div<double>(const int n, const double* a, const double* b,
    double* y) {
#ifdef __VECLIB__
  vDSP_vdivD(b, 1, a, 1, y, 1, n);
#else
  vdDiv(n, a, b, y);
#endif
}

template <>
void caffe_powx<float>(const int n, const float* a, const float b,
    float* y) {
  vsPowx(n, a, b, y);
}

template <>
void caffe_powx<double>(const int n, const double* a, const double b,
    double* y) {
  vdPowx(n, a, b, y);
}

template <>
void caffe_sqr<float>(const int n, const float* a, float* y) {
#ifdef __VECLIB__
  vDSP_vsq(a, 1, y, 1, n);
#elif defined(__ARM_NEON_H)
  int tail_frames = n % 4;
  const float* end = y + n - tail_frames;
  while (y < end) {
    float32x4_t a_frame = vld1q_f32(a);
    vst1q_f32(y, vmulq_f32(a_frame, a_frame));
    a += 4;
    y += 4;
  }
  vsSqr(tail_frames, a, y);
#else
  vsSqr(n, a, y);
#endif
}

template <>
void caffe_sqr<double>(const int n, const double* a, double* y) {
#ifdef __VECLIB__
  vDSP_vsqD(a, 1, y, 1, n);
#else
  vdSqr(n, a, y);
#endif
}

template <>
void caffe_sqrt<float>(const int n, const float* a, float* y) {
#ifdef __VECLIB__
  vvsqrtf(y, a, &n);
#elif defined(__ARM_NEON_H)
  int tail_frames = n % 4;
  const float* end = y + n - tail_frames;
  while (y < end) {
    float32x4_t a_frame = vld1q_f32(a);
    vst1q_f32(y, vsqrtq_f32(a_frame));
    a += 4;
    y += 4;
  }
  vsSqrt(tail_frames, a, y);
#else
  vsSqrt(n, a, y);
#endif
}

template <>
void caffe_sqrt<double>(const int n, const double* a, double* y) {
#ifdef __VECLIB__
  vvsqrt(y, a, &n);
#else
  vdSqrt(n, a, y);
#endif
}

template <>
void caffe_exp<float>(const int n, const float* a, float* y) {
#ifdef __VECLIB__
  vvexpf(y, a, &n);
#else
  vsExp(n, a, y);
#endif
}

template <>
void caffe_exp<double>(const int n, const double* a, double* y) {
#ifdef __VECLIB__
  vvexp(y, a, &n);
#else
  vdExp(n, a, y);
#endif
}

template <>
void caffe_log<float>(const int n, const float* a, float* y) {
#ifdef __VECLIB__
  vvlogf(y, a, &n);
#else
  vsLn(n, a, y);
#endif
}

template <>
void caffe_log<double>(const int n, const double* a, double* y) {
#ifdef __VECLIB__
  vvlog(y, a, &n);
#else
  vdLn(n, a, y);
#endif
}

template <>
void caffe_abs<float>(const int n, const float* a, float* y) {
#ifdef __VECLIB__
  vDSP_vabs(a, 1, y , 1, n);
#elif defined(__ARM_NEON_H)
  int tail_frames = n % 4;
  const float* end = y + n - tail_frames;
  while (y < end) {
    float32x4_t a_frame = vld1q_f32(a);
    vst1q_f32(y, vabsq_f32(a_frame));
    a += 4;
    y += 4;
  }
  vsAbs(tail_frames, a, y);
#else
  vsAbs(n, a, y);
#endif
}

template <>
void caffe_abs<double>(const int n, const double* a, double* y) {
#ifdef __VECLIB__
  vDSP_vabsD(a, 1, y , 1, n);
#else
  vdAbs(n, a, y);
#endif
}

unsigned int caffe_rng_rand() {
  return (*caffe_rng())();
}

#ifdef USE_BOOST
template <typename Dtype>
Dtype caffe_nextafter(const Dtype b) {
  return boost::math::nextafter<Dtype>(
      b, std::numeric_limits<Dtype>::max());
}

template
float caffe_nextafter(const float b);

template
double caffe_nextafter(const double b);
#else
// std::nextafter has some problems with tr1 & _GLIBCXX_USE_C99_MATH_TR1
// when using android ndk
float caffe_nextafter(const float b) {
    return ::nextafterf(b, std::numeric_limits<float>::max());
}
double caffe_nextafter(const double b) {
    return ::nextafter(b, std::numeric_limits<float>::max());
}
#endif

template <typename Dtype>
void caffe_rng_uniform(const int n, const Dtype a, const Dtype b, Dtype* r) {
  CHECK_GE(n, 0);
  CHECK(r);
  CHECK_LE(a, b);
#ifdef USE_BOOST
  boost::uniform_real<Dtype> random_distribution(a, caffe_nextafter<Dtype>(b));
  boost::variate_generator<caffe::rng_t*, boost::uniform_real<Dtype> >
      variate_generator(caffe_rng(), random_distribution);
  for (int i = 0; i < n; ++i) {
    r[i] = variate_generator();
  }
#else
  std::uniform_real_distribution<Dtype> random_distribution(a, caffe_nextafter(b));
  for (int i = 0; i < n; ++i) {
    r[i] = random_distribution(*caffe_rng());
  }
#endif
}

template
void caffe_rng_uniform<float>(const int n, const float a, const float b,
                              float* r);

template
void caffe_rng_uniform<double>(const int n, const double a, const double b,
                               double* r);

template <typename Dtype>
void caffe_rng_gaussian(const int n, const Dtype a,
                        const Dtype sigma, Dtype* r) {
  CHECK_GE(n, 0);
  CHECK(r);
  CHECK_GT(sigma, 0);
#ifdef USE_BOOST
  boost::normal_distribution<Dtype> random_distribution(a, sigma);
  boost::variate_generator<caffe::rng_t*, boost::normal_distribution<Dtype> >
      variate_generator(caffe_rng(), random_distribution);
  for (int i = 0; i < n; ++i) {
    r[i] = variate_generator();
  }
#else
  std::normal_distribution<Dtype> random_distribution(a, sigma);
  for (int i = 0; i < n; ++i) {
    r[i] = random_distribution(*caffe_rng());
  }
#endif
}

template
void caffe_rng_gaussian<float>(const int n, const float mu,
                               const float sigma, float* r);

template
void caffe_rng_gaussian<double>(const int n, const double mu,
                                const double sigma, double* r);

template <typename Dtype>
void caffe_rng_bernoulli(const int n, const Dtype p, int* r) {
  CHECK_GE(n, 0);
  CHECK(r);
  CHECK_GE(p, 0);
  CHECK_LE(p, 1);
#ifdef USE_BOOST
  boost::bernoulli_distribution<Dtype> random_distribution(p);
  boost::variate_generator<caffe::rng_t*, boost::bernoulli_distribution<Dtype> >
      variate_generator(caffe_rng(), random_distribution);
  for (int i = 0; i < n; ++i) {
    r[i] = variate_generator();
  }
#else
  std::bernoulli_distribution random_distribution(p);
  for (int i = 0; i < n; ++i) {
    r[i] = random_distribution(*caffe_rng());
  }
#endif
}

template
void caffe_rng_bernoulli<double>(const int n, const double p, int* r);

template
void caffe_rng_bernoulli<float>(const int n, const float p, int* r);

template <typename Dtype>
void caffe_rng_bernoulli(const int n, const Dtype p, unsigned int* r) {
  CHECK_GE(n, 0);
  CHECK(r);
  CHECK_GE(p, 0);
  CHECK_LE(p, 1);
#ifdef USE_BOOST
  boost::bernoulli_distribution<Dtype> random_distribution(p);
  boost::variate_generator<caffe::rng_t*, boost::bernoulli_distribution<Dtype> >
      variate_generator(caffe_rng(), random_distribution);
  for (int i = 0; i < n; ++i) {
    r[i] = static_cast<unsigned int>(variate_generator());
  }
#else
  std::bernoulli_distribution random_distribution(p);
  for (int i = 0; i < n; ++i) {
    r[i] = static_cast<unsigned int>(random_distribution(*caffe_rng()));
  }
#endif
}

template
void caffe_rng_bernoulli<double>(const int n, const double p, unsigned int* r);

template
void caffe_rng_bernoulli<float>(const int n, const float p, unsigned int* r);

template <>
float caffe_cpu_strided_dot<float>(const int n, const float* x, const int incx,
    const float* y, const int incy) {
  return cblas_sdot(n, x, incx, y, incy);
}

template <>
double caffe_cpu_strided_dot<double>(const int n, const double* x,
    const int incx, const double* y, const int incy) {
  return cblas_ddot(n, x, incx, y, incy);
}

template <typename Dtype>
Dtype caffe_cpu_dot(const int n, const Dtype* x, const Dtype* y) {
  return caffe_cpu_strided_dot(n, x, 1, y, 1);
}

template
float caffe_cpu_dot<float>(const int n, const float* x, const float* y);

template
double caffe_cpu_dot<double>(const int n, const double* x, const double* y);

template <>
float caffe_cpu_asum<float>(const int n, const float* x) {
  return cblas_sasum(n, x, 1);
}

template <>
double caffe_cpu_asum<double>(const int n, const double* x) {
  return cblas_dasum(n, x, 1);
}

template <>
void caffe_cpu_scale<float>(const int n, const float alpha, const float *x,
                            float* y) {
  cblas_scopy(n, x, 1, y, 1);
  cblas_sscal(n, alpha, y, 1);
}

template <>
void caffe_cpu_scale<double>(const int n, const double alpha, const double *x,
                             double* y) {
  cblas_dcopy(n, x, 1, y, 1);
  cblas_dscal(n, alpha, y, 1);
}

}  // namespace caffe
