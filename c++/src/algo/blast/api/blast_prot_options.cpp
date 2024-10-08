/*  $Id: blast_prot_options.cpp 663400 2023-02-17 18:27:03Z fongah2 $
 * ===========================================================================
 *
 *                            PUBLIC DOMAIN NOTICE
 *               National Center for Biotechnology Information
 *
 *  This software/database is a "United States Government Work" under the
 *  terms of the United States Copyright Act.  It was written as part of
 *  the author's official duties as a United States Government employee and
 *  thus cannot be copyrighted.  This software/database is freely available
 *  to the public for use. The National Library of Medicine and the U.S.
 *  Government have not placed any restriction on its use or reproduction.
 *
 *  Although all reasonable efforts have been taken to ensure the accuracy
 *  and reliability of the software and data, the NLM and the U.S.
 *  Government do not and cannot warrant the performance or results that
 *  may be obtained by using this software or data. The NLM and the U.S.
 *  Government disclaim all warranties, express or implied, including
 *  warranties of performance, merchantability or fitness for any particular
 *  purpose.
 *
 *  Please cite the author in any work or product based on this material.
 *
 * ===========================================================================
 *
 * Authors:  Christiam Camacho
 *
 */

/// @file blast_prot_options.cpp
/// Implements the CBlastProteinOptionsHandle class.

#include <ncbi_pch.hpp>
#include <algo/blast/core/blast_encoding.h>
#include <algo/blast/api/blast_prot_options.hpp>
#include <objects/seqloc/Na_strand.hpp>
#include "blast_setup.hpp"


/** @addtogroup AlgoBlast
 *
 * @{
 */


BEGIN_NCBI_SCOPE
BEGIN_SCOPE(blast)

CBlastProteinOptionsHandle::CBlastProteinOptionsHandle(EAPILocality locality)
    : CBlastOptionsHandle(locality)
{
    SetDefaults();
    m_Opts->SetProgram(eBlastp);
}

void CBlastProteinOptionsHandle::SetWordSize(int ws) {

   	m_Opts->SetWordSize(ws);
   	switch (ws) {
   		case 3:
   		m_Opts->SetWordThreshold(BLAST_WORD_THRESHOLD_BLASTP);
   		break;
   		case 5:
   		m_Opts->SetWordThreshold(BLAST_WORD_THRESHOLD_BLASTP_FAST);
   		break;
   		case 6:
   		m_Opts->SetWordThreshold(BLAST_WORD_THRESHOLD_BLASTP_WD_SZ_6);
   		break;
   		case 7:
   		m_Opts->SetWordThreshold(BLAST_WORD_THRESHOLD_BLASTP_WD_SZ_7);
   		break;
   		default:
   		m_Opts->SetWordThreshold(BLAST_WORD_THRESHOLD_BLASTP);
   		break;
   	}

   	if (ws > 4) {
   		m_Opts->SetLookupTableType(eCompressedAaLookupTable);
   	}
   	else {
   		m_Opts->SetLookupTableType(eAaLookupTable);
   	}
}

void 
CBlastProteinOptionsHandle::SetLookupTableDefaults()
{
    m_Opts->SetLookupTableType(eAaLookupTable);
    SetWordSize(BLAST_WORDSIZE_PROT);
    SetWordThreshold(BLAST_WORD_THRESHOLD_BLASTP);
}

void
CBlastProteinOptionsHandle::SetQueryOptionDefaults()
{
    SetSegFiltering(true);
    m_Opts->SetStrandOption(objects::eNa_strand_unknown);
}

void
CBlastProteinOptionsHandle::SetInitialWordOptionsDefaults()
{
    SetXDropoff(BLAST_UNGAPPED_X_DROPOFF_PROT);
    SetWindowSize(BLAST_WINDOW_SIZE_PROT);
}

void
CBlastProteinOptionsHandle::SetGappedExtensionDefaults()
{
    SetGapXDropoff(BLAST_GAP_X_DROPOFF_PROT);
    SetGapXDropoffFinal(BLAST_GAP_X_DROPOFF_FINAL_PROT);
    SetGapTrigger(BLAST_GAP_TRIGGER_PROT);
    m_Opts->SetGapExtnAlgorithm(eDynProgScoreOnly);
    m_Opts->SetGapTracebackAlgorithm(eDynProgTbck);
}


void
CBlastProteinOptionsHandle::SetScoringOptionsDefaults()
{
    SetMatrixName(BLAST_DEFAULT_MATRIX); // BLOSUM62
    SetGapOpeningCost(BLAST_GAP_OPEN_PROT);
    SetGapExtensionCost(BLAST_GAP_EXTN_PROT);
    SetGappedMode();

    // set invalid values for options that are not applicable
    m_Opts->SetOutOfFrameMode(false);
    m_Opts->SetFrameShiftPenalty(INT2_MAX);
    //m_Opts->SetMatchReward(0);
    //m_Opts->SetMismatchPenalty(0);
}

void
CBlastProteinOptionsHandle::SetHitSavingOptionsDefaults()
{
    SetHitlistSize(500);
    SetEvalueThreshold(BLAST_EXPECT_VALUE);
    SetMinDiagSeparation(0);
    SetPercentIdentity(0);
    // set some default here, allow INT4MAX to mean infinity
    SetMaxNumHspPerSequence(0); 
    SetMaxHspsPerSubject(0);

    SetCutoffScore(0); // will be calculated based on evalue threshold,
    // effective lengths and Karlin-Altschul params in BLAST_Cutoffs_simple
    // and passed to the engine in the params structure
    SetQueryCovHspPerc(0);
}

void
CBlastProteinOptionsHandle::SetEffectiveLengthsOptionsDefaults()
{
    SetDbLength(0);
    SetDbSeqNum(0);
    SetEffectiveSearchSpace(0);
}

void
CBlastProteinOptionsHandle::SetSubjectSequenceOptionsDefaults()
{}

END_SCOPE(blast)
END_NCBI_SCOPE


/* @} */
