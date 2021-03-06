/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 */

#include <config.h>

#include "SCOREP_Sampling.h"

#include <SCOREP_Definitions.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>
#include <UTILS_Error.h>

#include <papi.h>

/** Definition handle if this interrupt generator */
static SCOREP_InterruptGeneratorHandle papi_interrupt_generator = SCOREP_INVALID_INTERRUPT_GENERATOR;

/* *********************************************************************
 * Helper functions
 **********************************************************************/

/**
 * Handler of the overflow signal generated by a PAPI event set.
 *
 * @param eventSet          PAPI event set
 * @param address           Program counter address at the time of the overflow
 * @param overflowVector    Indicating which hardware counter(s) caused the overflow
 * @param context           Context at the time of overflow (machine specific)
 */
static void
papi_signal_handler( int       eventSet,
                     void*     address,
                     long long overflowVector,
                     void*     context )
{
    bool outside = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    SCOREP_ENTER_SIGNAL_CONTEXT();

    if ( outside && scorep_sampling_is_enabled() )
    {
        SCOREP_Sample( papi_interrupt_generator, context );
    }

    SCOREP_EXIT_SIGNAL_CONTEXT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


/* *********************************************************************
 * Signal handler functions
 **********************************************************************/

static void
initialize_interrupt_generator( scorep_sampling_interrupt_generator_definition definition )
{
    if ( !PAPI_is_initialized() )
    {
        if ( PAPI_library_init( PAPI_VER_CURRENT ) != PAPI_VER_CURRENT )
        {
            UTILS_WARNING( "Failed PAPI_Init." );
        }
    }

    papi_interrupt_generator =
        SCOREP_Definitions_NewInterruptGenerator( definition.event,
                                                  SCOREP_INTERRUPT_GENERATOR_MODE_COUNT,
                                                  SCOREP_METRIC_BASE_DECIMAL,
                                                  0,
                                                  definition.period );
}

/** @brief  This function can be used to determine identifier of recent location.
 *
 *  @return It returns the identifier of recent location.
 */
static unsigned long
get_location_id( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    /* Get the thread id from the measurement system */
    SCOREP_Location* data = SCOREP_Location_GetCurrentCPULocation();
    UTILS_ASSERT( data != NULL );

    unsigned long result = SCOREP_Location_GetId( data );

    SCOREP_IN_MEASUREMENT_DECREMENT();

    return result;
}

static void
create_interrupt_generator( scorep_sampling_single_location_data*          samplingData,
                            scorep_sampling_interrupt_generator_definition definition )
{
    if ( !samplingData->papi_eventset )
    {
        if ( PAPI_thread_init( get_location_id ) != PAPI_OK )
        {
            UTILS_WARNING( "PAPI thread initialization failed" );
            return;
        }

        /* Create the EventSet */
        samplingData->papi_eventset = PAPI_NULL;
        if ( PAPI_create_eventset( &( samplingData->papi_eventset ) ) != PAPI_OK )
        {
            UTILS_WARNING( "Error while creating event set for PAPI interrupt generator" );
            return;
        }

        if ( PAPI_event_name_to_code( definition.event, &( samplingData->papi_code ) ) != PAPI_OK )
        {
            UTILS_WARNING( "Error while getting PAPI code for %s", definition.event );
            return;
        }
        if ( definition.event )
        {
            /* Add Total Instructions Executed to our EventSet */
            if ( PAPI_add_event( samplingData->papi_eventset, samplingData->papi_code ) != PAPI_OK )
            {
                UTILS_WARNING( "Error while adding event to PAPI interrupt generator" );
                return;
            }
        }

        /*
         * Set up the event set to begin registering overflows.
         * If an overflow is registered the handler function
         * (papi_signal_handler) will be invoked.
         */
        int retval = PAPI_overflow( samplingData->papi_eventset,
                                    samplingData->papi_code,
                                    definition.period, 0,
                                    papi_signal_handler );
        if ( retval != PAPI_OK )
        {
            UTILS_WARNING( "Error while initializing PAPI interrupt generator" );
            return;
        }

        /* older PAPI need this, newer PAPI return error, so: ignore the error */
        PAPI_start( samplingData->papi_eventset );
    }
}

static void
enable_interrupt_generator( scorep_sampling_single_location_data* samplingData,
                            uint64_t                              samplingInterval )
{
    /*
     * Set up the event set to begin registering overflows.
     * If an overflow is registered the handler function
     * (papi_signal_handler) will be invoked.
     */
    int retval = PAPI_overflow( samplingData->papi_eventset,
                                samplingData->papi_code,
                                samplingInterval, 0,
                                papi_signal_handler );
    if ( retval != PAPI_OK )
    {
        UTILS_WARNING( "Error while enabling PAPI Interrupt generator" );
    }

    /* older PAPI need this, newer PAPI return error, so: ignore the error */
    PAPI_start( samplingData->papi_eventset );
}

static void
disable_interrupt_generator( scorep_sampling_single_location_data* samplingData )
{
    long long ignore;
    int       retval;

    /* older PAPI need this, newer PAPI return error, so: ignore the error */
    PAPI_stop( samplingData->papi_eventset, &ignore );

    if ( ( retval = PAPI_overflow( samplingData->papi_eventset, samplingData->papi_code, 0, 0, NULL ) ) != PAPI_OK )
    {
        UTILS_WARNING( "Error while disabling PAPI Interrupt generator (%i)", retval );
    }
}


const scorep_sampling_interrupt_generator scorep_sampling_interrupt_generator_papi =
{
    .initialize_interrupt_generator = initialize_interrupt_generator,
    .create_interrupt_generator     = create_interrupt_generator,
    .enable_interrupt_generator     = enable_interrupt_generator,
    .disable_interrupt_generator    = disable_interrupt_generator,
    .finalize_interrupt_generator   = NULL
};
