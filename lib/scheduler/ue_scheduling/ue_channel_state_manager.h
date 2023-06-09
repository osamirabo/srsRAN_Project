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

#include "srsran/ran/csi_report/csi_report_wideband_cqi.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include "srsran/scheduler/scheduler_slot_handler.h"

namespace srsran {

/// \brief This classes manages all the information related with the channel state that has been received from the UE
/// via CSI or via gNB PHY measurements.
class ue_channel_state_manager
{
public:
  ue_channel_state_manager(const scheduler_ue_expert_config& expert_cfg_);

  const optional<csi_report_data>& get_latest_csi_report() const { return latest_csi_report; }

  void update_pusch_snr(float snr_db) { pusch_snr_db = snr_db; }

  /// \brief Get PUSCH SNR in dBs.
  float get_pusch_snr() const { return pusch_snr_db; }

  csi_report_wideband_cqi_type get_wideband_cqi() const { return wideband_cqi; }

  /// \brief Gets the number of recommended layers to be used in DL based on reported RI.
  unsigned get_nof_dl_layers() const { return recommended_dl_layers; }

  /// \brief Fetches the number of recommended layers to be used in UL.
  unsigned get_nof_ul_layers() const { return 1; }

  /// \brief Fetches the precoding codebook to be used in DL based on reported PMI and the chosen nof layers.
  optional<pdsch_precoding_info> get_precoding(unsigned chosen_nof_layers, prb_interval pdsch_prbs) const
  {
    optional<pdsch_precoding_info> precoding_info;
    if (chosen_nof_layers <= 1) {
      return precoding_info;
    }
    precoding_info.emplace();
    precoding_info->nof_rbs_per_prg = pdsch_prbs.length();
    precoding_info->prg_infos.emplace_back(recommended_prg_info[chosen_nof_layers >> 2U]);
    return precoding_info;
  }

  /// Update UE with the latest CSI report for a given cell.
  void handle_csi_report(const csi_report_data& csi_report);

private:
  static constexpr size_t NOF_LAYER_CHOICES = 2;

  /// Estimated PUSCH SNR, in dB.
  float pusch_snr_db;

  /// \brief Recommended CQI to be used to derive the DL MCS.
  csi_report_wideband_cqi_type wideband_cqi;

  /// \brief Recommended nof layers based on reports.
  unsigned recommended_dl_layers = 1;

  /// \brief List of Recommended PMIs for different number of active layers. Position 0 is for 1 layer, position 1 is
  /// for 2 layers and position 3 for 4 layers.
  std::array<pdsch_precoding_info::prg_info, NOF_LAYER_CHOICES> recommended_prg_info;

  /// Latest CSI report received from the UE.
  optional<csi_report_data> latest_csi_report;
};

} // namespace srsran