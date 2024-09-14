# $Id: CMakeLists.txt 673644 2023-10-05 19:23:38Z ivanov $

NCBI_requires(Boost.Test.Included)
NCBI_add_app(seqdb_unit_test seqdb_lmdb_unit_test seqdb_tax4blast_unit_test)

