#! /bin/sh

# X11-Basic erzeugen
#
(echo "Neue Installation von X11-Basic vom " ; date) >install.log 2>&1
echo "X11-Basic wird erzeugt..."
cd src
make distclean >>../install.log 2>&1

echo "*** X11-Basic Version " >>../install.log
grep "version=" configure >>../install.log 2>&1
(echo "*** System ist ein " ; uname )>>../install.log 2>&1
(echo "*** Vorhandene Dateien" ; ls -al )>>../install.log 2>&1
echo "===Running configure ..." | tee -a ../install.log | tr -d =
./configure >>../install.log 2>&1

# sucess ?
if [ $? -eq 0 ]
then 
  echo "===Trying to make xbasic ..." | tee -a ../install.log | tr -d =
  make >>../install.log 2>&1
  # sucess ?
  if [ $? -eq 0 ] 
  then
    echo "===Testing xbasic ..." | tee -a ../install.log | tr -d =
    ./xbasic -e "version" >>../install.log 2>&1
    # sucess ?
    if [ $? -eq 0 ]
    then    
      echo
      echo "===SUCESS, you may now start xbasic from the directory ./src" | tee -a ../install.log | tr -d =
      echo "===or install it in system by typing 'cd src ; make install' (need to be root)" | tee -a ../install.log | tr -d =
      echo
    else
      echo
      echo "===FAILURE, the tests have failed" | tee -a ../install.log | tr -d =
      echo "===xbasic has not been built properly !" | tee -a ../install.log | tr -d =
      echo
    fi
  else
    echo
    echo "===FAILURE, could not make xbasic !" | tee -a ../install.log | tr -d =
    echo
  fi
else
   echo
   echo "===FAILURE, could not configure X11-Basic" | tee -a ../install.log | tr -d =
   echo
fi

# append config.log to install.log
echo "===config.log:" >>../install.log
cat config.log >>../install.log 2>&1
