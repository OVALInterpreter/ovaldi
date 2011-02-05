#!/bin/bash
#
#
#****************************************************************************************#
# Copyright (c) 2002-2011, The MITRE Corporation
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright notice, this list
#       of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright notice, this 
#       list of conditions and the following disclaimer in the documentation and/or other
#       materials provided with the distribution.
#     * Neither the name of The MITRE Corporation nor the names of its contributors may be
#       used to endorse or promote products derived from this software without specific 
#       prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
# SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
# OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
# TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#****************************************************************************************#

#
# This wrapper script runs the OVAL Interpreter with good arguments.
#

#
# Test to make sure the user has either supplied their own OVAL
# Definitions document or that one exists in the default location.
#
if [ $# == 1 ]; then
  DEFS="$1"
elif [ -f /usr/share/ovaldi/definitions.xml ]; then
  DEFS=/usr/share/ovaldi/definitions.xml
else
  echo "Error: no definitions file specified and default definitions"
  echo "(/usr/share/ovaldi/definitions.xml) do not exist."
  echo "usage: $0 [definitions.xml]"
  exit 1
fi

#
# Get a date of the start of the run, so we can date-stamp the results
# files.
#

DATE=`/bin/date +%F--%T`

#
# Next, run the OVAL Interpreter pointing it to the locations of the
# files it uses.
#

cd /var/log/ovaldi
/usr/sbin/ovaldi -d /var/log/ovaldi/system-characteristics-$DATE.xml \
     -r /var/log/ovaldi/results-$DATE.xml \
     -a /usr/share/ovaldi \
     -x /var/log/ovaldi/results-$DATE.html \
     -o $DEFS \
     -m \

echo ...
echo The OVAL System Characteristics and OVAL Results documents are stored in /var/log/ovaldi.
echo ...

