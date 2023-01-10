/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/support/resource_grid.h"
#include "srsgnb/ran/pci.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/ran/ssb_mapping.h"

namespace srsgnb {

/// Describes the SSB processor interface.
class ssb_processor
{
public:
  /// Defines the BCH payload size.
  static constexpr unsigned BCH_PAYLOAD_SIZE = 32;

  /// Describes the SS/PBCH Block PDU.
  struct pdu_t {
    /// Current slot context.
    slot_point slot;
    /// Physical Cell identifier.
    pci_t phys_cell_id;
    /// PSS power level allocation in dB, relative to SSS.
    float beta_pss;
    /// SSB opportunity index in a burst.
    unsigned ssb_idx;
    /// Maximum number of SS/PBCH block candidates in a 5ms burst, described in TS38.213 Section 4.1.
    unsigned L_max;
    /// Higher layer parameter \e subCarrierSpacingCommon as per TS38.331 Section 6.2.2 - MIB.
    subcarrier_spacing common_scs;
    /// \brief Alignment offset between the resource grid and the SS/PBCH block.
    /// \sa ssb_subcarrier_offset for more details.
    ssb_subcarrier_offset subcarrier_offset;
    /// \brief Start of the SS/PBCH block relative to Point A in PRB.
    /// \sa ssb_offset_to_pointA for more details.
    ssb_offset_to_pointA offset_to_pointA;
    /// SS/PBCH pattern case.
    ssb_pattern_case pattern_case;
    /// \brief PBCH payload.
    /// The first 24 bits correspond to the packed MIB, while the last 8 bits are:
    /// - SFN (4 bit),
    /// - half-radio frame (1 bit), and
    /// - SS/PBCH block index (3 bit) for L_max=64, otherwise
    /// - SS/PBCH subcarrier offset (1 bit) and 2 padding bits.
    /// \remark Bit order is specified in 3GPP TS38.212 Section 7.1.1.
    std::array<uint8_t, BCH_PAYLOAD_SIZE> bch_payload;
    /// Port indexes to map the SS/PBCH transmission.
    static_vector<uint8_t, MAX_PORTS> ports;
  };

  /// Default destructor.
  virtual ~ssb_processor() = default;

  /// \brief Processes the SS/PBCH Block PDU and writes the resultant signal in the given grid.
  /// \param[out] grid Destination resource grid.
  /// \param[in]  pdu Properties to generate the SSB message.
  virtual void process(resource_grid_writer& grid, const pdu_t& pdu) = 0;
};

/// \brief Describes the SSB processor validator interface.
class ssb_pdu_validator
{
public:
  /// Default destructor.
  virtual ~ssb_pdu_validator() = default;

  /// \brief Validates SSB processor configuration parameters.
  /// \return True if the parameters contained in \c pdu are supported, false otherwise.
  virtual bool is_valid(const ssb_processor::pdu_t& pdu) const = 0;
};

} // namespace srsgnb
