/*
 ******************************************************************
 * Subroutine HelmholtzJ
 * Solves poisson equation on rectangular grid assuming :
 * (1) Uniform discretization in each direction, and
 * (2) Dirichlect boundary conditions
 *
 * Jacobi method is used in this routine
 *
 * Input : n,m   Number of grid points in the X/Y directions
 *         dx,dy Grid spacing in the X/Y directions
 *         alpha Helmholtz eqn. coefficient
 *         omega Relaxation factor
 *         f(n,m) Right hand side function
 *         u(n,m) Dependent variable/Solution
 *         tolerance Tolerance for iterative solver
 *         maxit  Maximum number of iterations
 *
 * Output : u(n,m) - Solution
 *****************************************************************
 */
#include <mpi.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include "jacobi.h"

#include <scorep/SCOREP_User.h>

using namespace std;

#define U( j, i ) afU[ ( ( j ) - data.iRowFirst ) * data.iCols + ( i ) ]
#define F( j, i ) afF[ ( ( j ) - data.iRowFirst ) * data.iCols + ( i ) ]
#define UOLD( j, i ) uold[ ( ( j ) - data.iRowFirst ) * data.iCols + ( i ) ]

inline void
ExchangeJacobiMpiData( JacobiData&  data,
                       double*      uold,
                       MPI_Request* request,
                       MPI_Status*  status );

void
Jacobi( JacobiData& data )
{
    /*use local pointers for performance reasons*/
    double* afU, * afF;
    afU = data.afU;
    afF = data.afF;
    MPI_Request request[ 4 ];
    MPI_Status  status[ 4 ];

    SCOREP_USER_REGION_DEFINE( scorep_iteration );

    double* uold = new double[ data.iCols * ( data.iRowLast - data.iRowFirst + 1 ) ];

    if ( uold )
    {
        double ax       = 1.0 / ( data.fDx * data.fDx );    /* X-direction coef */
        double ay       = 1.0 / ( data.fDy * data.fDy );    /* Y_direction coef */
        double b        = -2.0 * ( ax + ay ) - data.fAlpha; /* Central coeff */
        double residual = 10.0 * data.fTolerance;

#ifdef SCOREP_POMP_USER
        #pragma pomp inst begin(loop)
#endif
        while ( residual > data.fTolerance && data.iIterCount < data.iIterMax )
        {
            SCOREP_USER_REGION_BEGIN( scorep_iteration, "ITERATION",
                                      SCOREP_USER_REGION_TYPE_DYNAMIC );

            residual = 0.0;

            /* copy new solution into old */
            ExchangeJacobiMpiData( data, uold, request, status );

            double fLRes;

            /* compute stencil, residual and update */
            for ( int j = data.iRowFirst + 1; j <= data.iRowLast - 1; j++ )
            {
                for ( int i = 1; i < data.iCols - 1; i++ )
                {
                    fLRes = ( ax * ( UOLD( j, i - 1 ) + UOLD( j, i + 1 ) )
                              + ay * ( UOLD( j - 1, i ) + UOLD( j + 1, i ) )
                              +  b * UOLD( j, i ) - F( j, i ) ) / b;

                    /* update solution */
                    U( j, i ) = UOLD( j, i ) - data.fRelax * fLRes;

                    /* accumulate residual error */
                    residual += fLRes * fLRes;
                }
            }
            double tmpResd = residual;
            MPI_Allreduce( &tmpResd, &residual, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );

            /* error check */
            data.iIterCount++;
            residual = sqrt( residual ) / ( data.iCols * data.iRows );

            SCOREP_USER_REGION_END( scorep_iteration );
        } /* while */

#ifdef SCOREP_POMP_USER
        #pragma pomp inst end(loop)
#endif

        data.fResidual = residual;
        delete[] uold;
    }
    else
    {
        cerr << "Error: cant allocate memory\n";
        Finish( data );
        exit( 1 );
    }
}

inline void
ExchangeJacobiMpiData( JacobiData& data, double* uold,
                       MPI_Request* request, MPI_Status* status )
{
    double*   afU           = data.afU;
    int       iReqCnt       = 0;
    const int iTagMoveLeft  = 10;
    const int iTagMoveRight = 11;

    if ( data.iMyRank != 0 )
    {
        /*  receive stripe mlo from left neighbour blocking */
        MPI_Irecv( &UOLD( data.iRowFirst, 0 ), data.iCols, MPI_DOUBLE,
                   data.iMyRank - 1, iTagMoveRight, MPI_COMM_WORLD,
                   &request[ iReqCnt ] );
        iReqCnt++;
    }

    if ( data.iMyRank != data.iNumProcs - 1 )
    {
        /* receive stripe mhi from right neighbour blocking */
        MPI_Irecv( &UOLD( data.iRowLast, 0 ), data.iCols, MPI_DOUBLE,
                   data.iMyRank + 1, iTagMoveLeft, MPI_COMM_WORLD,
                   &request[ iReqCnt ] );
        iReqCnt++;
    }

    if ( data.iMyRank != data.iNumProcs - 1 )
    {
        /* send stripe mhi-1 to right neighbour async */
        MPI_Isend( &U( data.iRowLast - 1, 0 ), data.iCols, MPI_DOUBLE,
                   data.iMyRank + 1, iTagMoveRight, MPI_COMM_WORLD,
                   &request[ iReqCnt ] );
        iReqCnt++;
    }

    if ( data.iMyRank != 0 )
    {
        /* send stripe mlo+1 to left neighbour async */
        MPI_Isend( &U( data.iRowFirst + 1, 0 ), data.iCols, MPI_DOUBLE,
                   data.iMyRank - 1, iTagMoveLeft, MPI_COMM_WORLD,
                   &request[ iReqCnt ] );
        iReqCnt++;
    }
    for ( int j = data.iRowFirst + 1; j <= data.iRowLast - 1; j++ )
    {
        for ( int i = 0; i < data.iCols; i++ )
        {
            UOLD( j, i ) = U( j, i );
        }
    }

    MPI_Waitall( iReqCnt, request, status );
}
