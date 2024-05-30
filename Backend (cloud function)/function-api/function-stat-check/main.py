# use "Runtime : Python 3.12" & "Entry point : main"
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

def find_devices_missing_data():
    try:
        # Connect to MySQL server
        connection = sql.connect(**db_config)
        if connection.is_connected():
            # Create a cursor object
            cursor = connection.cursor()

            # Get yesterday's date
            yesterday_date = (datetime.now() - timedelta(days=1)).strftime('%Y-%m-%d')

            # SQL query to find devices that didn't send daily_consumption data for yesterday
            query = """
            SELECT DISTINCT p.perangkat_id
            FROM perangkat p
            LEFT JOIN konsumsi_harian kh ON p.perangkat_id = kh.perangkat_id
            WHERE kh.waktu_diterima IS NULL OR DATE(kh.waktu_diterima) != %s
            """

            # Execute the query
            cursor.execute(query, (yesterday_date,))
            
            # Fetch the result
            missing_devices = cursor.fetchall()

            # Close the cursor
            cursor.close()

            return missing_devices

    except Exception as e:
        print(f"Error: {e}")
        return None
    finally:
        if connection.is_connected():
            cursor.close()
            connection.close()
            print("MySQL connection closed")

def main(request):
    try:
        # Connect to Mysql Server
        connection = sql.connect(**db_config)
        if connection.is_connected():
            print("Connected to MySQL server")

            missing_devices = find_devices_missing_data()
            if missing_devices:
                print("Devices missing data for yesterday:")
                data = missing_devices[0]
                print((data[0],))
                for device in missing_devices:
                    print(device[0])
                update_query = """
                UPDATE stat_perangkat SET stat_komunikasi = 0
                WHERE perangkat_id = %s
                """
                cursor = connection.cursor() # Create a cursor object
                
                for device in missing_devices:
                    update_data = (device[0],)
                    cursor.execute(update_query,update_data)
                    print(update_data)
                    
                connection.commit() # Commit the changes
                cursor.close() # Close the cursor after use
                return "Stat komunikasi has been updated", 200
            else:
                print("Failed to retrieve missing devices.")
                return "All communication works", 200
    except sql.Error as err:
        print(f"Error: {err}")
        return f"Error:{err}", 500 # Error in query
    
    finally:
        if connection.is_connected():
            cursor.close()
            connection.close()
            print("MySQL connection closed")
