# Notes for Backend implemented in cloud function

## Summary
### function-stat-check
This is the code that updates "status_komunikasi" or communication status in the "stat_perangkat" or device status table.
The function checks the device that are registered in "perangkat_id" or device identity on the previous table. 
This function is triggered by cloud scheduler that is set to run daily every 7:00 WIB.
### function-receiver
