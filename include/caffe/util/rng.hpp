#ifndef CAFFE_RNG_CPP_HPP_
#define CAFFE_RNG_CPP_HPP_

#include <algorithm>
#include <iterator>

#ifdef USE_BOOST
#FIXME extract random generator from boost
#include "boost/random/mersenne_twister.hpp"
#include "boost/random/uniform_int.hpp"
#else
#include <random>
#endif

#include "caffe/common.hpp"

namespace caffe {

#ifdef USE_BOOST
typedef boost::mt19937 rng_t;
#else
typedef std::mt19937   rng_t;
#endif

inline rng_t* caffe_rng() {
  return static_cast<caffe::rng_t*>(Caffe::rng_stream().generator());
}

// Fisher–Yates algorithm
template <class RandomAccessIterator, class RandomGenerator>
inline void shuffle(RandomAccessIterator begin, RandomAccessIterator end,
                    RandomGenerator* gen) {
#ifdef NO_CAFFE_MOBILE
  typedef typename std::iterator_traits<RandomAccessIterator>::difference_type
      difference_type;
  typedef typename boost::uniform_int<difference_type> dist_type;

  difference_type length = std::distance(begin, end);
  if (length <= 0) return;

  for (difference_type i = length - 1; i > 0; --i) {
    dist_type dist(0, i);
    std::iter_swap(begin + i, begin + dist(*gen));
  }
#else
  NOT_IMPLEMENTED;
#endif
}

template <class RandomAccessIterator>
inline void shuffle(RandomAccessIterator begin, RandomAccessIterator end) {
#ifdef NO_CAFFE_MOBILE
  shuffle(begin, end, caffe_rng());
#else
  NOT_IMPLEMENTED;
#endif
}
}  // namespace caffe

#endif  // CAFFE_RNG_HPP_
