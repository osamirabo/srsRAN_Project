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

#include "downlink_processor_single_executor_state.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/upper/downlink_processor.h"
#include <mutex>

namespace srsran {

class upper_phy_rg_gateway;
class task_executor;

namespace detail {

class downlink_processor_callback
{
public:
  virtual ~downlink_processor_callback() = default;

  /// Sends the resource grid and updates the processor state to allow configuring a new resource grid.
  virtual void send_resource_grid() = 0;

  /// Decrements the number of pending PDUs to be processed and tries to send the resource grid through the gateway.
  virtual void on_task_completion() = 0;
};

} // namespace detail

/// \brief Implementation of a downlink processor.
///
/// This implementation process the PDUs and config using one executor, which
/// hides the execution threading model. The implementation ensures that all the PDUs and configs will be processed
/// before transmitting the resource grid through the gateway, even if finish_processing_pdus() method is called before
/// every PDU is processed.
/// \note This implementation follows the same method order that the interface. Not following this order will result in
/// undefined behaviour. As a reminder, the order is the following: first configure the downlink processor by calling
/// configure_resource_grid(). After it is configured it can start to process PDUs. Finally, when no more PDUs need to
/// be processed, finish_processing_pdus() can be called so the grid can be send through the gateway. Note that
/// finish_processing_pdus() can be called before the PDUs finished processing, but the resource grid is sent through
/// the gateway as soon as every enqueued PDU before finish_processing_pdus() is processed . This is controlled counting
/// the PDUs that are processed and finished processing.
///  \note Thread safe class.
class downlink_processor_single_executor_impl : public downlink_processor, private detail::downlink_processor_callback
{
public:
  /// \brief Builds a downlink processor single executor impl object with the given parameters.
  ///
  /// \param gateway_     Gateway that will be used to send the resource grids.
  /// \param pdcch_proc_  PDSCH processor used to process PDSCH PDUs.
  /// \param pdsch_proc_  PDCCH processor used to process PDCCH PDUs.
  /// \param ssb_proc_    SSB processor used to process SSB PDUs.
  /// \param csi_rs_proc_ CSI-RS processor used to process CSI-RS configurations.
  /// \param executor_    Task executor that will be used to process every PDU.
  /// \param logger_      Logger instance.
  downlink_processor_single_executor_impl(upper_phy_rg_gateway&                 gateway_,
                                          std::unique_ptr<pdcch_processor>      pdcch_proc_,
                                          std::unique_ptr<pdsch_processor>      pdsch_proc_,
                                          std::unique_ptr<ssb_processor>        ssb_proc_,
                                          std::unique_ptr<nzp_csi_rs_generator> csi_rs_proc_,
                                          task_executor&                        executor_,
                                          srslog::basic_logger&                 logger_);

  // See interface for documentation.
  void process_pdcch(const pdcch_processor::pdu_t& pdu) override;

  // See interface for documentation.
  void process_pdsch(unique_tx_buffer                                                                     softbuffer,
                     const static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS>& data,
                     const pdsch_processor::pdu_t&                                                        pdu) override;

  // See interface for documentation.
  void process_ssb(const ssb_processor::pdu_t& pdu) override;

  // See interface for documentation.
  void process_nzp_csi_rs(const nzp_csi_rs_generator::config_t& config) override;

  // See interface for documentation.
  bool configure_resource_grid(const resource_grid_context& context, resource_grid& grid) override;

  // See interface for documentation.
  void finish_processing_pdus() override;

private:
  class pdsch_processor_notifier_wrapper : public pdsch_processor_notifier
  {
  public:
    pdsch_processor_notifier_wrapper(detail::downlink_processor_callback& callback_) : callback(callback_)
    {
      // Do nothing.
    }

    void on_finish_processing() override { callback.on_task_completion(); }

  private:
    detail::downlink_processor_callback& callback;
  };

  /// \brief Sends the resource grid and updates the processor state to allow configuring a new resource grid.
  void send_resource_grid() override;

  /// \brief Decrements the number of pending PDUs to be processed and tries to send the resource grid through the
  /// gateway.
  void on_task_completion() override;

  upper_phy_rg_gateway&                 gateway;
  resource_grid_context                 rg_context;
  resource_grid*                        current_grid;
  std::unique_ptr<pdcch_processor>      pdcch_proc;
  std::unique_ptr<pdsch_processor>      pdsch_proc;
  std::unique_ptr<ssb_processor>        ssb_proc;
  std::unique_ptr<nzp_csi_rs_generator> csi_rs_proc;
  task_executor&                        executor;
  srslog::basic_logger&                 logger;

  /// DL processor internal state.
  downlink_processor_single_executor_state state;

  /// PDSCH notifier wrapper.
  pdsch_processor_notifier_wrapper pdsch_notifier;

  /// Protects the internal state.
  // :TODO: remove me later
  mutable std::mutex mutex;
};
} // namespace srsran
