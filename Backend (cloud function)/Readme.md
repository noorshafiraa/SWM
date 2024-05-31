# Notes for Backend implemented in cloud function

## Summary
### function-stat-check
his function is responsible for updating the 'status_komunikasi' or communication status in the 'stat_perangkat' or device status table. It retrieves the list of devices identified by their 'perangkat_id' or device identity. If a device is found to have not sent any data, its communication status is updated to 0; conversely, if a device is found to have sent data, its communication status is updated accordingly. This determination is made by querying the 'konsumsi_harian' or daily consumption table.

The function is triggered by a cloud scheduler set to run daily at 7:00 AM Western Indonesian Time (WIB). It examines device data from 6:00 PM to 6:59 AM WIB, covering the period when data should have been transmitted.
### function-receiver
