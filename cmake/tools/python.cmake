FIND_PACKAGE (Python3 QUIET COMPONENTS Interpreter)
IF(NOT Python3_FOUND)
    IF(WIN32)
        MESSAGE("DOWNLOADING PYTHON")
        FILE( DOWNLOAD
                https://www.python.org/ftp/python/3.7.2/python-3.7.2-amd64-webinstall.exe
                ${CMAKE_CURRENT_BINARY_DIR}/python-3.7.2-amd64-webinstall.exe
                SHOW_PROGRESS
                STATUS DOWNLOADED)

        EXECUTE_PROCESS(
                COMMAND
                ${CMAKE_CURRENT_BINARY_DIR}/python-3.7.2-amd64-webinstall.exe \\
                PrependPath=1 Include_test=0 InstallAllUsers=0 Include_launcher=0 \\
                InstallLauncherAllUsers=0 SimpleInstall=1 \\
                SimpleInstallDescription="Installs a local version of python and adds to the path"
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                OUTPUT_QUIET
                ERROR_QUIET)

        FILE( REMOVE
                ${CMAKE_CURRENT_BINARY_DIR}/python-3.7.2-amd64-webinstall.exe )

        MESSAGE("YOU MAY NEED TO RELOG IN ORDER FOR THE PATH TO BE UPDATED")
    ELSE()
        MESSAGE("PLEASE INSTALL PYTHON TO ALLOW BUILD SCRIPTS TO FUNCTION CORRECTLY")
    ENDIF()
ENDIF()
