param(
    [string]$exePath = "build\NumberService.exe",
    [string]$serviceName = "NumService",
    [string]$displayName = "Number Store Service",
    [string]$account = "NT AUTHORITY\LocalService"
)

# Build the absolute path relative to this script's folder
$scriptFolder = $PSScriptRoot
$resolvedExePath = Join-Path $scriptFolder $exePath

# Verify executable exists
if (-not (Test-Path $resolvedExePath)) {
    Write-Error "Executable not found: $resolvedExePath"
    exit 1
}

# If service already exists, stop and delete it
$service = Get-Service -Name $serviceName -ErrorAction SilentlyContinue
if ($null -ne $service) {
    Write-Host "Service '$serviceName' already exists. Removing it..."
    try {
        if ($service.Status -eq 'Running') {
            Write-Host "Stopping service..."
            Stop-Service -Name $serviceName -Force -ErrorAction Stop
        }
    } catch {
        Write-Warning "Failed to stop service (might not be running): $_"
    }

    # Delete old service
    sc.exe delete $serviceName | Out-Null
    Start-Sleep -Seconds 2   # Give Windows time to unregister
}

# Create new service
$bin = "`"$resolvedExePath`""   # Quote path for safety

$cmd = "sc.exe create `"$serviceName`" binPath= $bin DisplayName= `"$displayName`" start= auto obj= `"$account`""
Write-Host "Running: $cmd"
Invoke-Expression $cmd

Write-Host "Service created. Start it with:"
Write-Host "sc start $serviceName"
