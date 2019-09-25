/*
 * Copyright (c) 2018-2019, NVIDIA CORPORATION.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <algorithm>
#include "cuda_utils.h"
#include "vectorized.h"

namespace MLCommon {
namespace LinAlg {

template <typename InType, int VecLen, typename Lambda, typename OutType,
          typename IdxType>
__global__ void unaryOpKernel(OutType *out, const InType *in, IdxType len,
                              Lambda op) {
  typedef TxN_t<InType, VecLen> InVecType;
  typedef TxN_t<OutType, VecLen> OutVecType;
  InVecType a;
  OutVecType b;
  IdxType idx = threadIdx.x + ((IdxType)blockIdx.x * blockDim.x);
  idx *= InVecType::Ratio;
  if (idx >= len) return;
  a.load(in, idx);
#pragma unroll
  for (int i = 0; i < InVecType::Ratio; ++i) {
    b.val.data[i] = op(a.val.data[i]);
  }
  b.store(out, idx);
}

template <typename InType, int VecLen, typename Lambda, typename OutType,
          typename IdxType, int TPB>
void unaryOpImpl(OutType *out, const InType *in, IdxType len, Lambda op,
                 cudaStream_t stream) {
  const IdxType nblks = ceildiv(VecLen ? len / VecLen : len, (IdxType)TPB);
  unaryOpKernel<InType, VecLen, Lambda, OutType, IdxType>
    <<<nblks, TPB, 0, stream>>>(out, in, len, op);
  CUDA_CHECK(cudaPeekAtLastError());
}

/**
 * @brief perform element-wise unary operation in the input array
 * @tparam InType input data-type
 * @tparam Lambda the device-lambda performing the actual operation
 * @tparam OutType output data-type
 * @tparam IdxType Integer type used to for addressing
 * @tparam TPB threads-per-block in the final kernel launched
 * @param out the output array
 * @param in the input array
 * @param len number of elements in the input array
 * @param op the device-lambda
 * @param stream cuda stream where to launch work
 * @note Lambda must be a functor with the following signature:
 *       `OutType func(const InType& val);`
 */
template <typename InType, typename Lambda, typename IdxType = int,
          typename OutType = InType, int TPB = 256>
void unaryOp(OutType *out, const InType *in, IdxType len, Lambda op,
             cudaStream_t stream) {
  if (len <= 0) return;  //silently skip in case of 0 length input
  constexpr auto minSize =
    sizeof(InType) < sizeof(OutType) ? sizeof(InType) : sizeof(OutType);
  size_t bytes = len * minSize;
  uint64_t inAddr = uint64_t(in);
  uint64_t outAddr = uint64_t(out);
  if (16 / minSize && bytes % 16 == 0 && inAddr % 16 == 0 &&
      outAddr % 16 == 0) {
    unaryOpImpl<InType, 16 / minSize, Lambda, OutType, IdxType, TPB>(
      out, in, len, op, stream);
  } else if (8 / minSize && bytes % 8 == 0 && inAddr % 8 == 0 &&
             outAddr % 8 == 0) {
    unaryOpImpl<InType, 8 / minSize, Lambda, OutType, IdxType, TPB>(
      out, in, len, op, stream);
  } else if (4 / minSize && bytes % 4 == 0 && inAddr % 4 == 0 &&
             outAddr % 4 == 0) {
    unaryOpImpl<InType, 4 / minSize, Lambda, OutType, IdxType, TPB>(
      out, in, len, op, stream);
  } else if (2 / minSize && bytes % 2 == 0 && inAddr % 2 == 0 &&
             outAddr % 2 == 0) {
    unaryOpImpl<InType, 2 / minSize, Lambda, OutType, IdxType, TPB>(
      out, in, len, op, stream);
  } else if (1 / minSize) {
    unaryOpImpl<InType, 1 / minSize, Lambda, OutType, IdxType, TPB>(
      out, in, len, op, stream);
  } else {
    unaryOpImpl<InType, 1, Lambda, OutType, IdxType, TPB>(out, in, len, op,
                                                          stream);
  }
}

};  // end namespace LinAlg
};  // end namespace MLCommon
