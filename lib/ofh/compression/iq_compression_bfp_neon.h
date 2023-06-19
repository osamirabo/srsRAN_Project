/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "iq_compression_bfp_impl.h"

namespace srsran {
namespace ofh {

/// Implementation of the Block Floating Point IQ data compression using NEON intrinsics.
class iq_compression_bfp_neon : public iq_compression_bfp_impl
{
public:
  // Constructor.
  explicit iq_compression_bfp_neon(float iq_scaling_ = 1.0) : iq_compression_bfp_impl(iq_scaling_) {}

  // See interface for the documentation.
  void compress(span<compressed_prb> output, span<const cf_t> input, const ru_compression_params& params) override;

  // See interface for the documentation.
  void decompress(span<cf_t> output, span<const compressed_prb> input, const ru_compression_params& params) override;
};

} // namespace ofh
} // namespace srsran