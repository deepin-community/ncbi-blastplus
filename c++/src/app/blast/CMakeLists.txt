# $Id: CMakeLists.txt 683932 2024-06-10 12:08:12Z ivanov $

NCBI_add_library(blast_app_util)
NCBI_add_app(
  blastp
  blastn
  blastx
  tblastn
  tblastx
  psiblast
  rpsblast
  rpstblastn
  blast_formatter
  deltablast
  seedtop
)

NCBI_project_watchers(camacho madden fongah2)
