import mysql.connector as sql
from datetime import datetime, timedelta

# MySQL connection initialization
db_config = {
    "host": "34.101.221.13",
    "port": 3306,
    "user": "root",
    "password": "Ta0206sukses!",
    "database": "ta232402006",
}

def find_missing_present_devices(connection):
    try:
        if connection.is_connected():
            # Create a cursor object
            cursor = connection.cursor()

            # Get yesterday's date
            today = datetime.now() + timedelta(hours=7)
            yesterday_start = (today - timedelta(days=1)).replace(hour=11, minute=0, second=0, microsecond=0)
            yesterday_end = (today - timedelta(days=1)).replace(hour=23, minute=59, second=59, microsecond=999999)
            print(f"Start: {yesterday_start}, End: {yesterday_end}")

            # SQL query to find devices that didn't send daily_consumption data for the specified time range yesterday
            query_missing = """
            SELECT DISTINCT p.perangkat_id
            FROM perangkat p
            LEFT JOIN konsumsi_harian kh ON p.perangkat_id = kh.perangkat_id
            AND kh.waktu_diterima BETWEEN %s AND %s
            WHERE kh.waktu_diterima IS NULL
            """

            query_present = """
            SELECT DISTINCT p.perangkat_id
            FROM perangkat p
            JOIN konsumsi_harian kh ON p.perangkat_id = kh.perangkat_id
            WHERE kh.waktu_diterima BETWEEN %s AND %s
            """

            # Execute the queries
            cursor.execute(query_missing, (yesterday_start, yesterday_end))
            missing_devices = cursor.fetchall()

            cursor.execute(query_present, (yesterday_start, yesterday_end))
            present_devices = cursor.fetchall()

            # Close the cursor
            cursor.close()

            return missing_devices, present_devices

    except Exception as e:
        print(f"Error finding missing and present devices: {e}")
        return None, None

def update_stat_komunikasi(devices_missing, devices_present, connection):
    try:
        if connection.is_connected():
            # Create a cursor object
            cursor = connection.cursor()

            # Update stat_komunikasi to 0 for missing devices
            if devices_missing:
                update_query_missing = """
                UPDATE stat_perangkat
                SET stat_komunikasi = %s
                WHERE perangkat_id = %s
                """
                update_data_missing = [(0, device[0]) for device in devices_missing]
                cursor.executemany(update_query_missing, update_data_missing)

            # Update stat_komunikasi to 1 for present devices
            if devices_present:
                update_query_present = """
                UPDATE stat_perangkat
                SET stat_komunikasi = %s
                WHERE perangkat_id = %s
                """
                update_data_present = [(1, device[0]) for device in devices_present]
                cursor.executemany(update_query_present, update_data_present)

            # Commit the changes
            connection.commit()

            # Close the cursor
            cursor.close()

    except Exception as e:
        print(f"Error in updating: {e}")

def main(request):
    try:
        # Connect to MySQL server
        connection = sql.connect(**db_config)

        # Get missing and present devices
        missing_devices, present_devices = find_missing_present_devices(connection)
        if missing_devices is not None and present_devices is not None:
            print("Devices missing data for yesterday:")
            for device in missing_devices:
                print(device[0])

            print("Devices present data for yesterday:")
            for device in present_devices:
                print(device[0])

            update_stat_komunikasi(missing_devices, present_devices, connection)

            return "Stat komunikasi has been updated", 200
        else:
            print("Failed to retrieve missing or present devices.")
            return "Error in retrieving device data", 200
    except Exception as e:
        print(f"Error in main program: {e}")
        return f"Error:{e}", 500  # Error in query
