#!/usr/bin/env bash

# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to You under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -e -o pipefail

>2& echo "[INFO] Fetching Google zone..."
GOOGLE_ZONE=$(gcloud compute instances list --filter="name=$(hostname)" --format="value(zone)")

>2& echo "[INFO] Fetching metadata..."
REPOSITORY=$(gcloud compute instances describe "$(hostname)" \
                    --format="value(metadata[repository])" --quiet --zone="${GOOGLE_ZONE}")
PAT_VERSION=$(gcloud compute instances describe "$(hostname)" \
                     --format="value(metadata[pat_version])" --quiet --zone="${GOOGLE_ZONE}")

>2& echo "[INFO] Fetching PAT..."
GITHUB_PAT="$(gcloud secrets versions access ${PAT_VERSION} --secret=GITHUB_PAT)"

>2& echo "[INFO] Fetching GitHub token..."
TOKEN=$(curl -X POST --url "https://api.github.com/repos/${REPOSITORY}/actions/runners/registration-token" \
     -H "Authorization: Bearer ${GITHUB_PAT}" \
     -H "Accept: application/vnd.github.v3+json" | jq -r '.token')

cd /var/opt/gh-runner

>2& echo "[INFO] Stopping GitHub runner service..."
sudo ./svc.sh stop

>2& echo "[INFO] Uninstalling GitHub runner service..."
sudo ./svc.sh uninstall

>2& echo "[INFO] Removing instance from GitHub runners..."
sudo su github -c "./config.sh remove --token ${TOKEN}"
