# This script continuously monitors the BitLocker status of the F: drive.
# It clears the screen, displays a status message, shows BitLocker info, and repeats every 2 seconds.

while ($true) {
    cls # Clear the console screen for a fresh display each time

    # Display a message indicating what the script is doing
    Write-Host "Checking BitLocker status on C: drive..." -ForegroundColor Cyan

    # Show the current BitLocker status for the C: drive
    manage-bde -status C:

    # Wait for 1 seconds before repeating the check
    Start-Sleep -Seconds 1
}