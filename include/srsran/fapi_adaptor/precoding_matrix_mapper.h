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

#include "srsran/adt/optional.h"
#include "srsran/mac/mac_cell_result.h"
#include <vector>

namespace srsran {

/// MAC PDSCH precoding information.
struct mac_pdsch_precoding_info {
  /// \brief CSI-RS report.
  ///
  /// This field is empty in case of 1-antenna port setups.
  optional<csi_report_pmi> report;
};

struct mac_pdcch_precoding_info {};
struct mac_ssb_precoding_info {};
struct mac_csi_rs_precoding_info {};

namespace fapi_adaptor {

/// Precoding matrix mapper codebook offset configuration.
struct precoding_matrix_mapper_codebook_offset_configuration {
  /// Codebook offsets for SSB. Each entry represents a codebook.
  std::vector<unsigned> ssb_codebook_offsets;
  /// Codebook offsets for PDSCH. Each entry represents a codebook.
  std::vector<unsigned> pdsch_codebook_offsets;
  /// Codebook offsets for PDCCH. Each entry represents a codebook.
  std::vector<unsigned> pdcch_codebook_offsets;
  /// Codebook offsets for CSI-RS. Each entry represents a codebook.
  std::vector<unsigned> csi_rs_codebook_offsets;
};

/// \brief Precoding matrix mapper.
///
/// Maps the given arguments to a precoding matrix index.
class precoding_matrix_mapper
{
public:
  precoding_matrix_mapper(const precoding_matrix_mapper_codebook_offset_configuration& config);

  /// Maps the given MAC precoding information into a precoding matrix index.
  unsigned map(const mac_pdsch_precoding_info& precoding_info, unsigned nof_layers) const;

  /// Maps the given MAC precoding information into a precoding matrix index.
  unsigned map(const mac_pdcch_precoding_info& precoding_info) const;

  /// Maps the given MAC precoding information into a precoding matrix index.
  unsigned map(const mac_csi_rs_precoding_info& precoding_info) const;

  /// Maps the given MAC precoding information into a precoding matrix index.
  unsigned map(const mac_ssb_precoding_info& precoding_info) const;

private:
  /// Codebook offsets for SSB. Each entry represents a codebook.
  std::vector<unsigned> ssb_codebook_offsets;
  /// Codebook offsets for PDSCH. Each entry represents a codebook.
  std::vector<unsigned> pdsch_codebook_offsets;
  /// Codebook offsets for PDCCH. Each entry represents a codebook.
  std::vector<unsigned> pdcch_codebook_offsets;
  /// Codebook offsets for CSI-RS. Each entry represents a codebook.
  std::vector<unsigned> csi_rs_codebook_offsets;
};

} // namespace fapi_adaptor
} // namespace srsran