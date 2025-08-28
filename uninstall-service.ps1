param([string]$serviceName = "NumService")
sc.exe stop $serviceName
Start-Sleep -Seconds 2
sc.exe delete $serviceName
