# Addkey.ps1
# Script to register VietK.dll and configure VietK IME on Windows 11

# Đường dẫn đến VietK.dll và MinGW runtime
$DllPath = "C:\Users\VJP-USER\CLionProjects\VietK\cmake-build-debug\VietK.dll"
$MinGWBin = "C:\msys64\mingw64\bin"

# CLSID của VietK
$CLSID = "{12345678-1234-1234-1234-567890ABCDEF}"

# Kiểm tra quyền admin
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Host "ERROR: Script must be run as Administrator. Right-click and select 'Run with PowerShell as Administrator'." -ForegroundColor Red
    pause
    exit 1
}

# Kiểm tra VietK.dll
if (-not (Test-Path $DllPath)) {
    Write-Host "ERROR: VietK.dll not found at $DllPath" -ForegroundColor Red
    Write-Host "Please ensure the DLL is built in the specified path." -ForegroundColor Red
    pause
    exit 1
}

# Copy MinGW runtime DLLs
Write-Host "Copying MinGW runtime DLLs..."
$runtimeDlls = @("libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll")
foreach ($dll in $runtimeDlls) {
    $src = Join-Path $MinGWBin $dll
    $dst = Join-Path (Split-Path $DllPath -Parent) $dll
    if (Test-Path $src) {
        Copy-Item $src $dst -Force
        Write-Host "Copied $dll to $(Split-Path $dst -Parent)" -ForegroundColor Green
    } else {
        Write-Host "WARNING: $dll not found in $MinGWBin" -ForegroundColor Yellow
    }
}

# Đăng ký VietK.dll
Write-Host "Registering VietK.dll..."
$regsvr32Output = Start-Process -FilePath "C:\Windows\System32\regsvr32.exe" -ArgumentList "/s `"$DllPath`"" -Wait -NoNewWindow -PassThru
if ($regsvr32Output.ExitCode -ne 0) {
    Write-Host "ERROR: Failed to register VietK.dll (Exit Code: $($regsvr32Output.ExitCode))." -ForegroundColor Red
    Write-Host "Possible reasons: Invalid DLL, missing dependencies, or insufficient permissions." -ForegroundColor Red
    Write-Host "Try running 'C:\Windows\System32\regsvr32.exe $DllPath' manually to see detailed error." -ForegroundColor Yellow
    pause
    exit 1
}
Write-Host "VietK.dll registered successfully." -ForegroundColor Green

# Tạo khóa TIP
Write-Host "Creating TIP Registry keys..."
New-Item -Path "HKLM:\SOFTWARE\Microsoft\CTF\TIP\$CLSID" -Force | Out-Null
Set-ItemProperty -Path "HKLM:\SOFTWARE\Microsoft\CTF\TIP\$CLSID" -Name "(Default)" -Value "VietK IME"
Set-ItemProperty -Path "HKLM:\SOFTWARE\Microsoft\CTF\TIP\$CLSID" -Name "Enable" -Value 1 -Type DWord
New-Item -Path "HKLM:\SOFTWARE\Microsoft\CTF\TIP\$CLSID\Category\{534C48C1-0607-4098-A521-4FC899C73E90}" -Force | Out-Null

# Tạo khóa LanguageProfile cho English US (0x00000409)
Write-Host "Creating LanguageProfile Registry keys..."
New-Item -Path "HKLM:\SOFTWARE\Microsoft\CTF\TIP\$CLSID\LanguageProfile\0x00000409" -Force | Out-Null
Set-ItemProperty -Path "HKLM:\SOFTWARE\Microsoft\CTF\TIP\$CLSID\LanguageProfile\0x00000409" -Name "(Default)" -Value "VietK IME"
Set-ItemProperty -Path "HKLM:\SOFTWARE\Microsoft\CTF\TIP\$CLSID\LanguageProfile\0x00000409" -Name "Description" -Value "VietK IME"
Set-ItemProperty -Path "HKLM:\SOFTWARE\Microsoft\CTF\TIP\$CLSID\LanguageProfile\0x00000409" -Name "Enable" -Value 1 -Type DWord

# Làm mới dịch vụ TSF
Write-Host "Refreshing TSF service..."
Stop-Service -Name "ctfmon" -Force -ErrorAction SilentlyContinue
Start-Service -Name "ctfmon" -ErrorAction SilentlyContinue

Write-Host "VietK IME has been registered and activated." -ForegroundColor Green
Write-Host "Check Settings > Time & Language > Language & Region > Preferred languages > English (United States) > Options > Keyboards."
Write-Host "If VietK IME does not appear, restart your PC or run 'control input.dll' to open keyboard settings."
pause
exit 0