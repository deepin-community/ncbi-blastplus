/*  $Id:
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
 * Authors: Amelia Fong
 *
 */

/** @file tblastn_node.cpp
 * tblastn node api
 */

#include <ncbi_pch.hpp>
#include <corelib/ncbiapp.hpp>
#include <algo/blast/api/local_blast.hpp>
#include <algo/blast/api/psiblast.hpp>
#include <algo/blast/api/remote_blast.hpp>
#include <algo/blast/blastinput/blast_fasta_input.hpp>
#include <algo/blast/api/objmgr_query_data.hpp>
#include <algo/blast/format/blast_format.hpp>
#include "blast_app_util.hpp"
#include "tblastn_node.hpp"

#ifndef SKIP_DOXYGEN_PROCESSING
USING_NCBI_SCOPE;
USING_SCOPE(blast);
USING_SCOPE(objects);
#endif

CTblastnNode::CTblastnNode (int node_num, const CNcbiArguments & ncbi_args, const CArgs& args,
		                      CBlastAppDiagHandler & bah, string & input,
                              int query_index, int num_queries,  CBlastNodeMailbox * mailbox):
                              CBlastNode(node_num, ncbi_args, args, bah, query_index, num_queries, mailbox), m_Input(kEmptyStr)
{
	m_Input.swap(input);
	m_CmdLineArgs.Reset(new CTblastnNodeArgs(m_Input));
	SetState(eInitialized);
	SendMsg(CBlastNodeMsg::eRunRequest, (void*) this);
}

int CTblastnNode::GetBlastResults(CNcbiOstream & os)
{
	if(GetState() == eDone) {
		if (m_CmdLineArgs->GetOutputStrStream().rdbuf()->in_avail() > 0) {
			os << m_CmdLineArgs->GetOutputStrStream().rdbuf();
		}
		return GetStatus();
	}
	return -1;
}

CTblastnNode::~CTblastnNode()
{
	m_CmdLineArgs.Reset();
}

void *
CTblastnNode::Main()
{
    int status = BLAST_EXIT_SUCCESS;
    CBlastAppDiagHandler & bah = GetDiagHandler();
	SetDiagPostPrefix(GetNodeIdStr().c_str());

    SetState(eRunning);
    SetDataLoaderPrefix();
	try {

        /*** Get the BLAST options ***/
        const CArgs& args = GetArgs();

        CRef<CBlastOptionsHandle> opts_hndl;
        if(RecoverSearchStrategy(args, m_CmdLineArgs)){
        	opts_hndl.Reset(&*m_CmdLineArgs->SetOptionsForSavedStrategy(args));
        }
        else {
        	opts_hndl.Reset(&*m_CmdLineArgs->SetOptions(args));
        }
        const CBlastOptions& opt = opts_hndl->GetOptions();
        CRef<CQueryOptionsArgs> query_opts = 
            m_CmdLineArgs->GetQueryOptionsArgs();

        /*** Initialize the database/subject ***/
        CRef<CBlastDatabaseArgs> db_args(m_CmdLineArgs->GetBlastDatabaseArgs());
        CRef<CLocalDbAdapter> db_adapter;
        CRef<CScope> scope(new CScope(*CObjectManager::GetInstance()));
        InitializeSubject(db_args, opts_hndl, m_CmdLineArgs->ExecuteRemotely(), db_adapter, scope);
        _ASSERT(db_adapter && scope);

        /*** Get the query sequence(s) or PSSM (these two options are mutually
         * exclusive) ***/
        CRef<CPssmWithParameters> pssm = m_CmdLineArgs->GetInputPssm();
        CRef<CBlastFastaInputSource> fasta;
        CRef<CBlastInput> input;
        SDataLoaderConfig dlconfig =
            InitializeQueryDataLoaderConfiguration(query_opts->QueryIsProtein(),
                                                   db_adapter);

        CRef<CBlastQueryVector> query;
        CRef<IQueryFactory> query_factory;

        if (pssm.Empty()) {
            CBlastInputSourceConfig iconfig(dlconfig, query_opts->GetStrand(),
                                         query_opts->UseLowercaseMasks(),
                                         query_opts->GetParseDeflines(),
                                         query_opts->GetRange());
            if(m_Input.empty()){
               	ERR_POST(Warning << "Query is Empty!");
               	return BLAST_EXIT_SUCCESS;
            }
            fasta.Reset(new CBlastFastaInputSource(m_CmdLineArgs->GetInputStream(), iconfig));
            input.Reset(new CBlastInput(&*fasta,
                                        m_CmdLineArgs->GetQueryBatchSize()));
        } else {
            _ASSERT(pssm->HasQuery());
            _ASSERT(pssm->GetQuery().IsSeq());
            query.Reset(new CBlastQueryVector());
            scope->AddTopLevelSeqEntry(pssm->SetQuery());

            CRef<CSeq_loc> sl(new CSeq_loc());
            sl->SetWhole().Assign(*pssm->GetQuery().GetSeq().GetFirstId());

            CRef<CBlastSearchQuery> q(new CBlastSearchQuery(*sl, *scope));
            query->AddQuery(q);
            _ASSERT(query.NotEmpty());
        }

        /*** Get the formatting options ***/
        CRef<CFormattingArgs> fmt_args(m_CmdLineArgs->GetFormattingArgs());
        bool isArchiveFormat = fmt_args->ArchiveFormatRequested(args);
        if(!isArchiveFormat) {
        	bah.DoNotSaveMessages();
        }
        CBlastFormat formatter(opt, *db_adapter,
                               fmt_args->GetFormattedOutputChoice(),
                               query_opts->GetParseDeflines(),
                               m_CmdLineArgs->GetOutputStream(),
                               fmt_args->GetNumDescriptions(),
                               fmt_args->GetNumAlignments(),
                               *scope,
                               opt.GetMatrixName(),
                               fmt_args->ShowGis(),
                               fmt_args->DisplayHtmlOutput(),
                               opt.GetQueryGeneticCode(),
                               opt.GetDbGeneticCode(),
                               opt.GetSumStatisticsMode(),
                               m_CmdLineArgs->ExecuteRemotely(),
                               db_adapter->GetFilteringAlgorithm(),
                               fmt_args->GetCustomOutputFormatSpec(),
                               false, false, NULL, NULL,
                               GetCmdlineArgs(GetArguments()),
			       GetSubjectFile(args));

        formatter.SetQueryRange(query_opts->GetRange());
        formatter.SetLineLength(fmt_args->GetLineLength());
        formatter.SetHitsSortOption(fmt_args->GetHitsSortOption());
        formatter.SetHspsSortOption(fmt_args->GetHspsSortOption());
        formatter.SetCustomDelimiter(fmt_args->GetCustomDelimiter());
        if(UseXInclude(*fmt_args, args[kArgOutput].AsString())) {
        	formatter.SetBaseFile(args[kArgOutput].AsString() +  "_" + NStr::IntToString(GetNodeNum()));
        }
        formatter.PrintProlog();

        /*** Process the input ***/
        CRef<CSearchResultSet> results;
        if (pssm.Empty()) {
            for (; !input->End(); formatter.ResetScopeHistory(), QueryBatchCleanup()) {

                query =  input->GetNextSeqBatch(*scope);
                query_factory.Reset(new CObjMgr_QueryFactory(*query));

                SaveSearchStrategy(args, m_CmdLineArgs, query_factory, opts_hndl);

                if (m_CmdLineArgs->ExecuteRemotely()) {
                    CRef<CRemoteBlast> rmt_blast = 
                        InitializeRemoteBlast(query_factory, db_args, opts_hndl,
                              m_CmdLineArgs->ProduceDebugRemoteOutput(),
                              m_CmdLineArgs->GetClientId());
                    results = rmt_blast->GetResultSet();
                } else {
                    CLocalBlast lcl_blast(query_factory, opts_hndl, db_adapter);
                    lcl_blast.SetNumberOfThreads(1);
                    results = lcl_blast.Run();
                }

                if (fmt_args->ArchiveFormatRequested(args)) {
                    formatter.WriteArchive(*query_factory, *opts_hndl, *results, 0, bah.GetMessages());
                    bah.ResetMessages();
                } else {
                    BlastFormatter_PreFetchSequenceData(*results, scope,
                    		                            fmt_args->GetFormattedOutputChoice());
                    ITERATE(CSearchResultSet, result, *results) {
                        formatter.PrintOneResultSet(**result, query);
                    }
                }
            }
            SetQueriesLength(input->GetTotalLengthProcessed());

        } else {
            SaveSearchStrategy(args, m_CmdLineArgs, query_factory, opts_hndl, pssm);
            
            if (m_CmdLineArgs->ExecuteRemotely()) {

                CRef<CRemoteBlast> rmt_psiblast =
                    InitializeRemoteBlast(query_factory, db_args, opts_hndl,
                               m_CmdLineArgs->ProduceDebugRemoteOutput(),
                               m_CmdLineArgs->GetClientId(), pssm);
     
                results = rmt_psiblast->GetResultSet();
            
            } else {

                CRef<CPSIBlastOptionsHandle> psi_opts
                           (dynamic_cast <CPSIBlastOptionsHandle *> (&*opts_hndl));
                _ASSERT(psi_opts.NotEmpty());

                CRef<CPsiBlast> psiblast(new CPsiBlast(pssm, db_adapter, psi_opts));
                psiblast->SetNumberOfThreads(1);

                results = psiblast->Run(); 
            }

            if (isArchiveFormat) {
                formatter.WriteArchive(*query_factory, *opts_hndl, *results, 0, bah.GetMessages());
                bah.ResetMessages();
            } else {
                BlastFormatter_PreFetchSequenceData(*results, scope,
                			                        fmt_args->GetFormattedOutputChoice());
                ITERATE(CSearchResultSet, result, *results) {
                    formatter.PrintOneResultSet(**result, query);
                }
            }
        }

        formatter.PrintEpilog(opt);

        if (m_CmdLineArgs->ProduceDebugOutput()) {
            opts_hndl->GetOptions().DebugDumpText(NcbiCerr, "BLAST options", 1);
        }

    } CATCH_ALL(status)

	SetStatus(status);
	if (status == BLAST_EXIT_SUCCESS) {
		SetState(eDone);
		SendMsg(CBlastNodeMsg::ePostResult, (void *) this);

	}
	else {
		SetState(eError);
		SendMsg(CBlastNodeMsg::eErrorExit, (void *) this);
	}

    return NULL;
}

