# $Id: CMakeLists.txt 680762 2024-03-27 13:50:01Z ivanov $

NCBI_add_subdirectory(
  unit_test_util readers blast lds2 data_loaders simple
  alnmgr cddalignview test manip cleanup format edit 
  huge_asn validator
  asniotest align seqmasks_io eutils
  align_format snputil uudutil variation writers pubseq_gateway
  logging import flatfile
)
