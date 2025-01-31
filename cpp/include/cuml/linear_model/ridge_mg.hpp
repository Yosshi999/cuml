/*
 * Copyright (c) 2020-2022, NVIDIA CORPORATION.
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

#include <opg/matrix/data.hpp>
#include <opg/matrix/part_descriptor.hpp>

#include "glm.hpp"

namespace ML {
namespace Ridge {
namespace opg {

/**
 * @brief performs MNMG fit operation for the ridge regression
 * @param[in] handle: the internal cuml handle object
 * @param[in] input_data: vector holding all partitions for that rank
 * @param[in] input_desc: PartDescriptor object for the input
 * @param[in] labels: labels data
 * @param[in] alpha: ridge parameter
 * @param[in] n_alpha: number of ridge parameters. Only one parameter is supported right now.
 * @param[out] coef: learned regression coefficients
 * @param[out] intercept: intercept value
 * @param[in] fit_intercept: fit intercept or not
 * @param[in] normalize: normalize the data or not
 * @param[in] algo: the algorithm to use for fitting
 * @param[in] verbose
 */
void fit(raft::handle_t& handle,
         std::vector<MLCommon::Matrix::Data<float>*>& input_data,
         MLCommon::Matrix::PartDescriptor& input_desc,
         std::vector<MLCommon::Matrix::Data<float>*>& labels,
         float* alpha,
         int n_alpha,
         float* coef,
         float* intercept,
         bool fit_intercept,
         bool normalize,
         int algo,
         bool verbose);

void fit(raft::handle_t& handle,
         std::vector<MLCommon::Matrix::Data<double>*>& input_data,
         MLCommon::Matrix::PartDescriptor& input_desc,
         std::vector<MLCommon::Matrix::Data<double>*>& labels,
         double* alpha,
         int n_alpha,
         double* coef,
         double* intercept,
         bool fit_intercept,
         bool normalize,
         int algo,
         bool verbose);

/**
 * @brief performs MNMG prediction for OLS
 * @param[in] handle: the internal cuml handle object
 * @param[in] rank_sizes: includes all the partition size information for the rank
 * @param[in] n_parts: number of partitions
 * @param[in] input: input data
 * @param[in] n_rows: number of rows of input data
 * @param[in] n_cols: number of cols of input data
 * @param[in] coef: OLS coefficients
 * @param[in] intercept: the fit intercept
 * @param[out] preds: predictions
 * @param[in] verbose
 */
void predict(raft::handle_t& handle,
             MLCommon::Matrix::RankSizePair** rank_sizes,
             size_t n_parts,
             MLCommon::Matrix::Data<float>** input,
             size_t n_rows,
             size_t n_cols,
             float* coef,
             float intercept,
             MLCommon::Matrix::Data<float>** preds,
             bool verbose);

void predict(raft::handle_t& handle,
             MLCommon::Matrix::RankSizePair** rank_sizes,
             size_t n_parts,
             MLCommon::Matrix::Data<double>** input,
             size_t n_rows,
             size_t n_cols,
             double* coef,
             double intercept,
             MLCommon::Matrix::Data<double>** preds,
             bool verbose);

};  // end namespace opg
};  // end namespace Ridge
};  // end namespace ML
