/*!
 * @file      apps_common.c
 *
 * @brief     Common functions shared by the examples
 *
 * @copyright
 * The Clear BSD License
 * Copyright Semtech Corporation 2022. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the disclaimer
 * below) provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Semtech corporation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH CORPORATION BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "apps_common.h"
#include "apps_utilities.h"
#include "printers/sx126x_str.h"

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE MACROS-----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE CONSTANTS -------------------------------------------------------
 */

static sx126x_mod_params_lora_t lora_mod_params = {
    .sf   = LORA_SPREADING_FACTOR,
    .bw   = LORA_BANDWIDTH,
    .cr   = LORA_CODING_RATE,
    .ldro = 0,  // Will be initialized during radio init
};

const sx126x_pkt_params_lora_t lora_pkt_params = {
    .preamble_len_in_symb = LORA_PREAMBLE_LENGTH,
    .header_type          = LORA_PKT_LEN_MODE,
    .pld_len_in_bytes     = PAYLOAD_LENGTH,
    .crc_is_on            = LORA_CRC,
    .invert_iq_is_on      = LORA_IQ,
};

const sx126x_mod_params_gfsk_t gfsk_mod_params = {
    .br_in_bps    = FSK_BITRATE,
    .fdev_in_hz   = FSK_FDEV,
    .pulse_shape  = FSK_PULSE_SHAPE,
    .bw_dsb_param = FSK_BANDWIDTH,
};

const sx126x_pkt_params_gfsk_t gfsk_pkt_params = {
    .preamble_len_in_bits  = FSK_PREAMBLE_LENGTH,
    .preamble_detector     = FSK_PREAMBLE_DETECTOR,
    .sync_word_len_in_bits = FSK_SYNCWORD_LENGTH,
    .address_filtering     = FSK_ADDRESS_FILTERING,
    .header_type           = FSK_HEADER_TYPE,
    .pld_len_in_bytes      = PAYLOAD_LENGTH,
    .crc_type              = FSK_CRC_TYPE,
    .dc_free               = FSK_DC_FREE,
};

static const sx126x_mod_params_bpsk_t bpsk_mod_params = {
    .br_in_bps   = BPSK_BITRATE_IN_BPS,
    .pulse_shape = SX126X_DBPSK_PULSE_SHAPE,
};

static sx126x_pkt_params_bpsk_t bpsk_pkt_params = {
    .pld_len_in_bytes = 0,  // Will be initialized in radio init
    .ramp_up_delay    = 0,
    .ramp_down_delay  = 0,
    .pld_len_in_bits  = 0,  // Will be initialized in radio init
};

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE TYPES -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE VARIABLES -------------------------------------------------------
 */

//static volatile bool irq_fired = false;

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DECLARATION -------------------------------------------
 */
void radio_on_dio_irq( void* context );
void on_tx_done( void ) __attribute__( ( weak ) );
void on_rx_done( void ) __attribute__( ( weak ) );
void on_preamble_detected( void ) __attribute__( ( weak ) );
void on_syncword_valid( void ) __attribute__( ( weak ) );
void on_header_valid( ) __attribute__( ( weak ) );
void on_header_error( void ) __attribute__( ( weak ) );
void on_crc_error( void ) __attribute__( ( weak ) );
void on_rx_timeout( void ) __attribute__( ( weak ) );
void on_rx_error( void ) __attribute__( ( weak ) );
void on_cad_done_undetected( void ) __attribute__( ( weak ) );
void on_cad_done_detected( void ) __attribute__( ( weak ) );
void on_fhss_hop_done( void ) __attribute__( ( weak ) );

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC VARIABLES --------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS DEFINITION ---------------------------------------------
 */

void apps_common_sx126x_print_config( void )
{
    printf( "Common RF parameters:\n" );
    printf( "   Packet type     = %s\n", sx126x_pkt_type_to_str( PACKET_TYPE ) );
    printf( "   RF Freq         = %d Hz\n", RF_FREQ_IN_HZ );
    printf( "   Output power    = %d dBm\n", TX_OUTPUT_POWER_DBM );
    printf( "   Fallback mode   = %s\n", sx126x_fallback_modes_to_str( FALLBACK_MODE ) );
    printf( "   Rx boost mode %sactivated\n", ( ENABLE_RX_BOOST_MODE ) ? "" : "de" );
    printf( "\n" );
    if( PACKET_TYPE == SX126X_PKT_TYPE_LORA )
    {
        printf( "LoRa modulation parameters:\n" );
        printf( "   Spreading Factor = %s\n", sx126x_lora_sf_to_str( LORA_SPREADING_FACTOR ) );
        printf( "   Bandwidth        = %s\n", sx126x_lora_bw_to_str( LORA_BANDWIDTH ) );
        printf( "   Coding rate      = %s\n", sx126x_lora_cr_to_str( LORA_CODING_RATE ) );
        printf( "\n" );
        printf( "LoRa packet parameters:\n" );
        printf( "   Preamble length  = %d symbol(s)\n", LORA_PREAMBLE_LENGTH );
        printf( "   Header mode      = %s\n", sx126x_lora_pkt_len_modes_to_str( LORA_PKT_LEN_MODE ) );
        printf( "   Payload length   = %d\n", PAYLOAD_LENGTH );
        printf( "\n" );
        printf( "LoRa sync word = 0x%02x\n", LORA_SYNCWORD );
    }
    if( PACKET_TYPE == SX126X_PKT_TYPE_GFSK )
    {
        printf( "FSK modulation parameters:\n" );
        printf( "    Freq Deviation    = %e Hz\n", FSK_FDEV );
        printf( "    Bitrate           = %e bps\n", FSK_BITRATE );
        printf( "    Pulse shape       = %s\n", sx126x_gfsk_pulse_shape_to_str( FSK_PULSE_SHAPE ) );
        printf( "    Bandwidth         = %s\n", sx126x_gfsk_bw_to_str( FSK_BANDWIDTH ) );
        printf( "FSK packet parameters:\n" );
        printf( "    Preamble length   = %d bits\n", FSK_PREAMBLE_LENGTH );
        printf( "    Preamble detector = %s\n",
                            sx126x_gfsk_preamble_detector_to_str( FSK_PREAMBLE_DETECTOR ) );
        printf( "    Syncword length   = %d bits\n", FSK_SYNCWORD_LENGTH );
        printf( "    Address filtering = %s\n",
                            sx126x_gfsk_address_filtering_to_str( FSK_ADDRESS_FILTERING ) );
        printf( "    Header type       = %s\n", sx126x_gfsk_pkt_len_modes_to_str( FSK_HEADER_TYPE ) );
        printf( "    Payload length    = %d\n", PAYLOAD_LENGTH );
        printf( "    CRC type          = %s\n", sx126x_gfsk_crc_types_to_str( FSK_CRC_TYPE ) );
        printf( "    DC free           = %s\n", sx126x_gfsk_dc_free_to_str( FSK_DC_FREE ) );
    }
}

void apps_common_sx126x_init( const void* context )
{
    ASSERT_SX126X_RC( sx126x_reset( ( void* ) context ) );
    ASSERT_SX126X_RC( sx126x_init_retention_list( ( void* ) context ) );
}

void apps_common_sx126x_radio_init( const void* context )
{
    apps_common_sx126x_print_config( );

    ASSERT_SX126X_RC( sx126x_set_standby( context, SX126X_STANDBY_CFG_RC ) );
    ASSERT_SX126X_RC( sx126x_set_pkt_type( context, PACKET_TYPE ) );
    ASSERT_SX126X_RC( sx126x_set_rf_freq( context, RF_FREQ_IN_HZ ) );

    ASSERT_SX126X_RC( sx126x_set_rx_tx_fallback_mode( context, FALLBACK_MODE ) );
    ASSERT_SX126X_RC( sx126x_cfg_rx_boosted( context, ENABLE_RX_BOOST_MODE ) );

    if( PACKET_TYPE == SX126X_PKT_TYPE_LORA )
    {
        lora_mod_params.ldro = apps_common_compute_lora_ldro( LORA_SPREADING_FACTOR, LORA_BANDWIDTH );
        ASSERT_SX126X_RC( sx126x_set_lora_mod_params( context, &lora_mod_params ) );
        ASSERT_SX126X_RC( sx126x_set_lora_pkt_params( context, &lora_pkt_params ) );
        ASSERT_SX126X_RC( sx126x_set_lora_sync_word( context, LORA_SYNCWORD ) );
    }
    else if( PACKET_TYPE == SX126X_PKT_TYPE_GFSK )
    {
        ASSERT_SX126X_RC( sx126x_set_gfsk_mod_params( context, &gfsk_mod_params ) );
        ASSERT_SX126X_RC( sx126x_set_gfsk_pkt_params( context, &gfsk_pkt_params ) );
        ASSERT_SX126X_RC( sx126x_set_gfsk_sync_word( context, gfsk_sync_word, sizeof( gfsk_sync_word ) ) );

        if( FSK_DC_FREE != SX126X_GFSK_DC_FREE_OFF )
        {
            ASSERT_SX126X_RC( sx126x_set_gfsk_whitening_seed( context, FSK_WHITENING_SEED ) );
        }

        if( FSK_CRC_TYPE != SX126X_GFSK_CRC_OFF )
        {
            ASSERT_SX126X_RC( sx126x_set_gfsk_crc_seed( context, ( uint16_t ) FSK_CRC_SEED ) );
            ASSERT_SX126X_RC( sx126x_set_gfsk_crc_polynomial( context, ( uint16_t ) FSK_CRC_POLYNOMIAL ) );
        }
        if( FSK_ADDRESS_FILTERING != SX126X_GFSK_ADDRESS_FILTERING_DISABLE )
        {
            ASSERT_SX126X_RC( sx126x_set_gfsk_pkt_address( context, FSK_NODE_ADDRESS, FSK_BROADCAST_ADDRESS ) );
        }
    }
}

void apps_common_sx126x_irq_process( const void* context, bool irq_fired )
{
    if( irq_fired == true )
    {
				printf("got here\n");
        irq_fired = false;

        sx126x_irq_mask_t irq_regs;
        sx126x_get_and_clear_irq_status( context, &irq_regs );

        if( ( irq_regs & SX126X_IRQ_TX_DONE ) == SX126X_IRQ_TX_DONE )
        {
            printf( "Tx done\n" );
            on_tx_done( );
        }

        if( ( irq_regs & SX126X_IRQ_RX_DONE ) == SX126X_IRQ_RX_DONE )
        {
            printf( "Rx done\n" );
            ASSERT_SX126X_RC( sx126x_handle_rx_done( context ) );
            if( PACKET_TYPE == SX126X_PKT_TYPE_GFSK )
            {
                sx126x_pkt_status_gfsk_t pkt_status;
                sx126x_get_gfsk_pkt_status( context, &pkt_status );

                if( pkt_status.rx_status.crc_error == true )
                {
                    printf( "CRC error from packet status\n" );
                    // The CRC error call to on_crc_error is handled with SX126X_IRQ_CRC_ERROR
                }
                else if( pkt_status.rx_status.adrs_error == true )
                {
                    printf( "Address error from packet status\n" );
                    on_rx_error( );
                }
                else if( pkt_status.rx_status.length_error == true )
                {
                    printf( "Length error from packet status\n" );
                    on_rx_error( );
                }
                else
                {
                    on_rx_done( );
                }
            }
            else
            {
                on_rx_done( );
            }
        }

        if( ( irq_regs & SX126X_IRQ_PREAMBLE_DETECTED ) == SX126X_IRQ_PREAMBLE_DETECTED )
        {
            printf( "Preamble detected\n" );
            on_preamble_detected( );
        }

        if( ( irq_regs & SX126X_IRQ_SYNC_WORD_VALID ) == SX126X_IRQ_SYNC_WORD_VALID )
        {
            printf( "Syncword valid\n" );
            on_syncword_valid( );
        }

        if( ( irq_regs & SX126X_IRQ_HEADER_VALID ) == SX126X_IRQ_HEADER_VALID )
        {
            printf( "Header valid\n" );
            on_header_valid( );
        }

        if( ( irq_regs & SX126X_IRQ_HEADER_ERROR ) == SX126X_IRQ_HEADER_ERROR )
        {
            printf( "Header error\n" );
            on_header_error( );
        }

        if( ( irq_regs & SX126X_IRQ_CRC_ERROR ) == SX126X_IRQ_CRC_ERROR )
        {
            printf( "CRC error\n" );
            on_crc_error( );
        }

        if( ( irq_regs & SX126X_IRQ_CAD_DONE ) == SX126X_IRQ_CAD_DONE )
        {
            printf( "CAD done\n" );
            if( ( irq_regs & SX126X_IRQ_CAD_DETECTED ) == SX126X_IRQ_CAD_DETECTED )
            {
                printf( "Channel activity detected\n" );
                on_cad_done_detected( );
            }
            else
            {
                printf( "No channel activity detected\n" );
                on_cad_done_undetected( );
            }
        }

        if( ( irq_regs & SX126X_IRQ_TIMEOUT ) == SX126X_IRQ_TIMEOUT )
        {
            printf( "Rx timeout\n" );
            on_rx_timeout( );
        }

        if( ( irq_regs & SX126X_IRQ_LR_FHSS_HOP ) == SX126X_IRQ_LR_FHSS_HOP )
        {
            printf( "FHSS hop done\n" );
            on_fhss_hop_done( );
        }
    }
}

uint32_t get_time_on_air_in_ms( void )
{
    switch( PACKET_TYPE )
    {
    case SX126X_PKT_TYPE_LORA:
    {
        return sx126x_get_lora_time_on_air_in_ms( &lora_pkt_params, &lora_mod_params );
    }
    case SX126X_PKT_TYPE_GFSK:
    {
        return sx126x_get_gfsk_time_on_air_in_ms( &gfsk_pkt_params, &gfsk_mod_params );
    }
    case SX126X_PKT_TYPE_LR_FHSS:
    default:
    {
        return 0;
    }
    }
}

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DEFINITION --------------------------------------------
 */

//void radio_on_dio_irq( void* context )
//{
//    irq_fired = true;
//}
void on_tx_done( void )
{
    printf( "%s\n", __func__ );
}
void on_rx_done( void )
{
    printf( "%s\n", __func__ );
}
void on_preamble_detected( void )
{
    printf( "%s\n", __func__ );
}
void on_syncword_valid( void )
{
    printf( "%s\n", __func__ );
}
void on_header_valid( void )
{
    printf( "%s\n", __func__ );
}
void on_header_error( void )
{
    printf( "%s\n", __func__ );
}
void on_crc_error( void )
{
    printf( "%s\n", __func__ );
}
void on_rx_timeout( void )
{
    printf( "%s\n", __func__ );
}
void on_rx_error( void )
{
    printf( "%s\n", __func__ );
}
void on_cad_done_undetected( void )
{
    printf( "%s\n", __func__ );
}
void on_cad_done_detected( void )
{
    printf( "%s\n", __func__ );
}
void on_fhss_hop_done( void )
{
    printf( "%s\n", __func__ );
}

/* --- EOF ------------------------------------------------------------------ */
