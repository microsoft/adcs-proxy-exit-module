param (
  [Parameter(Mandatory=$true)]
  [string]$Operation,

  [Parameter(Mandatory=$false)]
  [string]$SubjectKeyIdentifier,

  [Parameter(Mandatory=$false)]
  [string]$SerialNumber,

  [Parameter(Mandatory=$true)]
  [string]$RawCertPath
)

# Cause a timeout in the exit module.
Start-Sleep 11

if ($Operation -eq 'certissued') {
  $cert = [System.Security.Cryptography.X509Certificates.X509Certificate2]::new($RawCertPath)
  $cert | fl > "$RawCertPath.txt"
}