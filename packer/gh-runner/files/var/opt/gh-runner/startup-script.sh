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

cd /var/opt/gh-runner

>2& echo "[INFO] Setting up permissions..."
sudo chown -R github:github .

>2& echo "[INFO] Downloading runner..."
GH_RUNNER_URL=$(curl -X GET --url "https://api.github.com/repos/${REPOSITORY}/actions/runners/downloads" \
     -H "Authorization: Bearer ${GITHUB_PAT}" \
     -H "Accept: application/vnd.github.v3+json" \
     -H "Content-Type: application/json" --fail | \
jq -r -c '.[] | select( .os == "linux" ) | select( .architecture == "x64" ) | .download_url')

sudo su github -c "curl -o actions-runner-linux-x64.tar.gz -L ${GH_RUNNER_URL}"
sudo su github -c "tar xzf ./actions-runner-linux-x64.tar.gz"
sudo su github -c "rm -f actions-runner-linux-x64.tar.gz"
unset GH_RUNNER_URL

>2& echo "[INFO] Fetching GitHub token..."
TOKEN=$(curl -X POST --url "https://api.github.com/repos/${REPOSITORY}/actions/runners/registration-token" \
     -H "Authorization: Bearer ${GITHUB_PAT}" \
     -H "Accept: application/vnd.github.v3+json" | jq -r '.token')

[[ "$(hostname)" =~ -([0-9a-f]{40}) ]] && GH_RUNNER_LABEL="${BASH_REMATCH[1]}"

>2& echo "[INFO] Setting up GitHub runner..."
sudo su github -c "./config.sh --url https://github.com/${REPOSITORY} --labels ${GH_RUNNER_LABEL} --token ${TOKEN} --unattended"

>2& echo "[INFO] Installing GitHub runner service..."
sudo ./svc.sh install github

>2& echo "[INFO] Starting GitHub runner service..."
sudo ./svc.sh start
