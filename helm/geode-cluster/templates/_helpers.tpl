{{/*
Expand the name of the chart.
*/}}
{{- define "geode-cluster.name" -}}
{{- default .Chart.Name .Values.nameOverride | trunc 63 | trimSuffix "-" }}
{{- end }}

{{/*
Create a default fully qualified app name.
We truncate at 63 chars because some Kubernetes name fields are limited to this (by the DNS naming spec).
If release name contains chart name it will be used as a full name.
*/}}
{{- define "geode-cluster.fullname" -}}
{{- if .Values.fullnameOverride }}
{{- .Values.fullnameOverride | trunc 63 | trimSuffix "-" }}
{{- else }}
{{- $name := default .Chart.Name .Values.nameOverride }}
{{- if contains $name .Release.Name }}
{{- .Release.Name | trunc 63 | trimSuffix "-" }}
{{- else }}
{{- printf "%s-%s" .Release.Name $name | trunc 63 | trimSuffix "-" }}
{{- end }}
{{- end }}
{{- end }}

{{/*
Create chart name and version as used by the chart label.
*/}}
{{- define "geode-cluster.chart" -}}
{{- printf "%s-%s" .Chart.Name .Chart.Version | replace "+" "_" | trunc 63 | trimSuffix "-" }}
{{- end }}

{{/*
Common labels
*/}}
{{- define "geode-cluster.labels" -}}
helm.sh/chart: {{ include "geode-cluster.chart" . }}
{{ include "geode-cluster.selectorLabels" . }}
{{- if .Chart.AppVersion }}
app.kubernetes.io/version: {{ .Chart.AppVersion | quote }}
{{- end }}
app.kubernetes.io/managed-by: {{ .Release.Service }}
{{- end }}

{{/*
Selector labels
*/}}
{{- define "geode-cluster.selectorLabels" -}}
app.kubernetes.io/name: {{ include "geode-cluster.name" . }}
app.kubernetes.io/instance: {{ .Release.Name }}
{{- end }}

{{/*
Create the name of the service account to use
*/}}
{{- define "geode-cluster.serviceAccountName" -}}
{{- if .Values.serviceAccount.create }}
{{- default (include "geode-cluster.fullname" .) .Values.serviceAccount.name }}
{{- else }}
{{- default "default" .Values.serviceAccount.name }}
{{- end }}
{{- end }}

{{/*
Path for the Geode image
*/}}
{{- define "geode-cluster.cluster.imagePath" -}}
{{- .Values.images.geode.repository }}/{{ .Values.images.geode.imageName }}:{{ .Values.images.geode.tag }}
{{- end }}

{{/*
Cluster locator name
*/}}
{{- define "geode-cluster.locator.name" -}}
{{- default (include "geode-cluster.fullname" .) .Values.cluster.prefix }}-locator
{{- end }}

{{/*
Cluster server name
*/}}
{{- define "geode-cluster.server.name" -}}
{{- default (include "geode-cluster.fullname" .) .Values.cluster.prefix }}-server
{{- end }}