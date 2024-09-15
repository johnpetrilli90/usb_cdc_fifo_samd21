@ECHO ON
del "pre-build_version_increment_log.txt"
gcc pre-build_version_increment.c -o pre-build_version_increment
pre-build_version_increment.exe >pre-build_version_increment_log.txt

