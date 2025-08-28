param([string]$serviceName = "NumberService")
sc.exe stop $serviceName
Start-Sleep -Seconds 2
sc.exe delete $serviceName
