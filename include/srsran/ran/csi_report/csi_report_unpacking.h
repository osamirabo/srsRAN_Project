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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/ran/csi_report/csi_report_configuration.h"
#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/support/units.h"

namespace srsran {

/// \brief Maximum Channel State Information (CSI) report size in bits.
///
/// Maximum CSI-Part1 report payload size for report quantity \e cri-RI-LI-PMI-CQI, wideband, four port
/// TypeI-SinglePanel PMI codebook, and multiplexed PUCCH.
static constexpr units::bits csi_report_max_size(17U);

/// Packed Channel State Information (CSI) report data type.
using csi_report_packed = bounded_bitset<csi_report_max_size.value(), false>;

/// \brief Gets the Channel State Information (CSI) report size when the CSI report is transmitted in PUCCH.
///
/// Fields widths are defined in TS38.212 Section 6.3.1.1.2.
///
/// \param[in] config CSI report configuration.
/// \return The report size in bits.
units::bits get_csi_report_pucch_size(const csi_report_configuration& config);

/// \brief Unpacks Channel State Information (CSI) report multiplexed in PUCCH.
///
/// The unpacking is CSI report unpacking defined in TS38.212 Section 6.3.1.1.2.
///
/// \param[in] packed Packed CSI report.
/// \param[in] config CSI report configuration.
/// \return The CSI report data.
csi_report_data csi_report_unpack_pucch(const csi_report_packed& packed, const csi_report_configuration& config);

} // namespace srsran