/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef PDCCH_
#define PDCCH_

#include "srslte/config.h"
#include "srslte/common/phy_common.h"
#include "srslte/mimo/precoding.h"
#include "srslte/mimo/layermap.h"
#include "srslte/modem/mod.h"
#include "srslte/modem/demod_soft.h"
#include "srslte/scrambling/scrambling.h"
#include "srslte/fec/rm_conv.h"
#include "srslte/fec/convcoder.h"
#include "srslte/fec/viterbi.h"
#include "srslte/fec/crc.h"
#include "srslte/phch/dci.h"
#include "srslte/phch/regs.h"




typedef enum SRSLTE_API {
  SEARCH_UE, SEARCH_COMMON
} srslte_pdcch_search_mode_t;


/* PDCCH object */
typedef struct SRSLTE_API {
  srslte_cell_t cell;
  uint32_t nof_regs;
  uint32_t nof_cce;
  uint32_t max_bits;

  srslte_regs_t *regs;

  /* buffers */
  cf_t *ce[SRSLTE_MAX_PORTS];
  cf_t *symbols[SRSLTE_MAX_PORTS];
  cf_t *x[SRSLTE_MAX_PORTS];
  cf_t *d;
  uint8_t *e;
  float rm_f[3 * (DCI_MAX_BITS + 16)];
  float *llr;

  /* tx & rx objects */
  srslte_modem_table_t mod;
  srslte_demod_soft_t demod;
  srslte_sequence_t seq[SRSLTE_NSUBFRAMES_X_FRAME];
  srslte_viterbi_t decoder;
  srslte_crc_t crc;
  srslte_precoding_t precoding; 

} srslte_pdcch_t;

SRSLTE_API int srslte_pdcch_init(srslte_pdcch_t *q, 
                                 srslte_regs_t *regs, 
                                 srslte_cell_t cell);

SRSLTE_API void srslte_pdcch_free(srslte_pdcch_t *q);


/* Encoding function */
SRSLTE_API int srslte_pdcch_encode(srslte_pdcch_t *q, 
                                   srslte_dci_msg_t *msg,
                                   srslte_dci_location_t location,
                                   uint16_t rnti,
                                   cf_t *sf_symbols[SRSLTE_MAX_PORTS],
                                   uint32_t nsubframe, 
                                   uint32_t cfi);

/* Decoding functions: Extract the LLRs and save them in the srslte_pdcch_t object */
SRSLTE_API int srslte_pdcch_extract_llr(srslte_pdcch_t *q, 
                                        cf_t *sf_symbols, 
                                        cf_t *ce[SRSLTE_MAX_PORTS],
                                        float noise_estimate, 
                                        uint32_t nsubframe, 
                                        uint32_t cfi);

/* Decoding functions: Try to decode a DCI message after calling srslte_pdcch_extract_llr */
SRSLTE_API int srslte_pdcch_decode_msg(srslte_pdcch_t *q, 
                                       srslte_dci_msg_t *msg, 
                                       srslte_dci_location_t *location,
                                       srslte_dci_format_t format,
                                       uint16_t *srslte_crc_rem);

/* Function for generation of UE-specific search space DCI locations */
SRSLTE_API uint32_t srslte_pdcch_ue_locations(srslte_pdcch_t *q, 
                                              srslte_dci_location_t *locations, 
                                              uint32_t max_locations,
                                              uint32_t nsubframe, 
                                              uint32_t cfi,
                                              uint16_t rnti);

/* Function for generation of common search space DCI locations */
SRSLTE_API uint32_t srslte_pdcch_common_locations(srslte_pdcch_t *q, 
                                                  srslte_dci_location_t *locations, 
                                                  uint32_t max_locations,
                                                  uint32_t cfi);

#endif