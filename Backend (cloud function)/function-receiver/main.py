# Use "runtime: Python 3.12" and "Entry Point: main"
import mysql.connector as sql
from mysql.connector import pooling
from datetime import datetime, timedelta
import json
import struct
import requests
import pytz

# Set the timezone to wit (western indonesia time) and utc
utcTime = pytz.utc
witTime = pytz.timezone("Asia/Jakarta")

# MySQL server
db_config = {
    "host": "34.101.221.13",
    "port": 3306,
    "user": "root",
    "password": "Ta0206sukses!",
    "database": "ta232402006",
}

# Initialize connection pool
pool = pooling.MySQLConnectionPool(pool_name="pool",
                                    pool_size=5,
                                    **db_config)

def downlink():
     # Define the URL
    urlSRE = "https://platform.antares.id:8443/~/antares-cse/antares-id/3-b400/3-b400-5"

    urlLaptop = "https://platform.antares.id:8443/~/antares-cse/antares-id/b400-2/b400-2-1"

    urlTA = "https://platform.antares.id:8443/~/antares-cse/antares-id/ta232401006/TA232401006"


    # Define the headers
    headersSRE = {
        "X-M2M-Origin": "01a7518e229c54e1:c2847ca70b63b9db",
        "Content-Type": "application/json;ty=4",
        "Accept": "application/json"
    }

    headersLaptop = {
        "X-M2M-Origin": "19a57048c46bf00f:067c5c7a2e794456",
        "Content-Type": "application/json;ty=4",
        "Accept": "application/json"
    }

    # Define the headers
    headersTA = {
        "X-M2M-Origin": "66916b012afd3f49:174a8e2203266a52",
        "Content-Type": "application/json;ty=4",
        "Accept": "application/json"
    }
    
    # Calculate the current time in seconds then to hex
    # Get the current time in ICT timezone
    current_timeD = datetime.now(witTime)
    downlinkData = current_timeD.hour * 3600 + current_timeD.minute * 60 + current_timeD.second
    print(downlinkData)
    hex_time = f"{downlinkData:08x}"  # Remove the '0x' prefix

    dataDownlink = {
        "m2m:cin": {
            "con": "{\"type\":\"downlink\", \"data\":\"" + hex_time + "\"}"
        }
    }

    response_downlink = requests.post(urlTA, headers=headersTA, json=dataDownlink)

    print(f"Status Code: {response_downlink.status_code}")
    print(f"Response Body: {response_downlink.json()}")

def update_data_cumulative(new_cumul_consump, perangkat_id_, tamper_flag_):
    try:
        # Acquire connection from the pool
        connection = pool.get_connection()

        if connection.is_connected():
            print("Connected to MySQL server")

            # Create a buffered cursor
            cursor = connection.cursor(buffered=True)

            ## Validate perangkat_id
            select_query = f"SELECT cumul_consump FROM konsumsi_kumulatif WHERE Perangkat_id = '{perangkat_id_}'"
            cursor.execute(select_query)
            result = cursor.fetchone()

            if result is None:
                print(f"No entry found for perangkat_id {perangkat_id_} in konsumsi_kumulatif table. Skipping update.")
                return  # Exit the function if no entry found

            # Do Downlink
            downlink()

            prev_cumul = result[0] if result[0] is not None else 0
            daily_consump = new_cumul_consump - prev_cumul

            ## Check for data entries in the last 1 minute for the same perangkat_id
            one_min_ago = datetime.now(utcTime) - timedelta(minutes=1)
            check_query = """
                SELECT COUNT(*)
                FROM konsumsi_harian
                WHERE Perangkat_id = %s AND waktu_diterima >= %s
            """
            cursor.execute(check_query, (perangkat_id_, one_min_ago))
            data_count = cursor.fetchone()[0]

            if data_count > 0:
                print(f"There are {data_count} data entries for perangkat_id {perangkat_id_} in the last 1 minute. No update will be performed.")
                return  # Exit the function if there are data entries in the last 1 minute

            ## Check for data entries from 10:30:00 yesterday to 00:29:59 today
            today_date = datetime.now(utcTime).date()
            start_time = datetime.combine(today_date, datetime.min.time()).replace(hour=10, minute=30, second=0, microsecond=0) - timedelta(days=1)
            end_time = datetime.combine(today_date, datetime.min.time()).replace(hour=0, minute=29, second=59, microsecond=0)
            check_query = """
                SELECT daily_consump
                FROM konsumsi_harian
                WHERE Perangkat_id = %s AND waktu_diterima BETWEEN %s AND %s
            """
            cursor.execute(check_query, (perangkat_id_, start_time, end_time))
            data = cursor.fetchone()

            if data:
                # There is a data entry in the specified time range
                current_daily_consump = data[0]
                new_daily_consump = current_daily_consump + daily_consump
                update_query = """
                    UPDATE konsumsi_harian 
                    SET daily_consump = %s, waktu_diterima = %s 
                    WHERE Perangkat_id = %s AND waktu_diterima BETWEEN %s AND %s
                """
                cursor.execute(update_query, (new_daily_consump, datetime.now(utcTime), perangkat_id_, start_time, end_time))
            else:
                # No data entries in the specified time range
                insert_query = """
                    INSERT INTO konsumsi_harian (Perangkat_id, waktu_diterima, daily_consump) 
                    VALUES (%s, %s, %s)
                """
                cursor.execute(insert_query, (perangkat_id_, datetime.now(utcTime), daily_consump))

            # Update the cumulative consumption in konsumsi_kumulatif table
            update_query = "UPDATE konsumsi_kumulatif SET cumul_consump = %s WHERE Perangkat_id = %s"
            cursor.execute(update_query, (new_cumul_consump, perangkat_id_))

            # Check the tamper flag and update stat_perangkat table
            if tamper_flag_ == 1:
                update_query = "UPDATE stat_perangkat SET stat_casing = %s, waktu_casing = %s WHERE Perangkat_id = %s"
                cursor.execute(update_query, (tamper_flag_, datetime.now(utcTime), perangkat_id_))
            else:
                update_query = "UPDATE stat_perangkat SET stat_casing = %s WHERE Perangkat_id = %s"
                cursor.execute(update_query, (tamper_flag_, perangkat_id_))

            # Get the current monthly consumption
            select_query = f"""
                SELECT month_consump
                FROM konsumsi_bulanan
                WHERE Perangkat_id = '{perangkat_id_}'
                AND MM = MONTH(NOW()) AND YY = YEAR(NOW());
            """
            cursor.execute(select_query)
            result = cursor.fetchone()

            if result is not None:
                newMonth = 0
                monthly_consump = daily_consump + result[0]
            else:
                # The month has changed
                newMonth = 1
                monthly_consump = daily_consump

            # Update the konsumsi_bulanan table
            if newMonth == 1:  # New month, insert new row
                insert_query = """
                    INSERT INTO konsumsi_bulanan (Perangkat_id, MM, YY, month_consump) 
                    VALUES (%s, %s, %s, %s)
                """
                cursor.execute(insert_query, (perangkat_id_, datetime.now(utcTime).month, datetime.now(utcTime).year, monthly_consump))
            else:
                update_query = """
                    UPDATE konsumsi_bulanan 
                    SET month_consump = %s 
                    WHERE Perangkat_id = %s AND MM = %s AND YY = %s
                """
                cursor.execute(update_query, (monthly_consump, perangkat_id_, datetime.now(utcTime).month, datetime.now(utcTime).year))

            # Commit the changes
            connection.commit()

    except sql.Error as err:
        print(f"Error: {err}")

    finally:
        if connection.is_connected():
            cursor.close()
            connection.close()
            print("MySQL connection closed")


def update_force_data(konsumsiAir, perangkat_id_, tamper_flag_):
    try:
        # Acquire connection from the pool
        connection = pool.get_connection()
        if connection.is_connected():
            print("Connected to MySQL server")

            # Create a cursor
            cursor = connection.cursor(buffered=True)
            
            ## Check the stat. Update waktu_casing only if flag_tamper = 1.
            if tamper_flag_ == 1:
                # Insert Data to the stat_perangkat table
                result_time = datetime.now(utcTime)
                update_query = "UPDATE stat_perangkat SET stat_casing = %s, waktu_casing = %s WHERE Perangkat_id = %s"
                update_data = (tamper_flag_,result_time,perangkat_id_)
                cursor.execute(update_query, update_data)
            else:
                update_query = "UPDATE stat_perangkat SET stat_casing = %s WHERE Perangkat_id = %s"
                update_data = (tamper_flag_, perangkat_id_)
                cursor.execute(update_query, update_data)

            # Insert Data to the force_data table
            result_time = datetime.now(utcTime)
            update_query = "UPDATE force_data SET force_consump = %s, waktu_diterima = %s, stat_casing = %s  WHERE Perangkat_id = %s"
            update_data = (konsumsiAir, result_time, tamper_flag_, perangkat_id_)
            cursor.execute(update_query, update_data)

            ## Get the current cumulative consumption
            select_query = f"SELECT cumul_consump FROM konsumsi_kumulatif WHERE Perangkat_id = '{perangkat_id_}'"
            # Execute the SELECT query
            cursor.execute(select_query)
            # Fetch the result
            result = cursor.fetchone()

            if result is None:
                print(f"No entry found for perangkat_id {perangkat_id_} in konsumsi_kumulatif table. Skipping update.")
                return  # Exit the function if no entry found

            else:
                downlink()

            # Commit the changes
            connection.commit()

    except sql.Error as err:
        print(f"Error: {err}")
    
    finally:
        if connection.is_connected():
            cursor.close()
            connection.close()
            print("MySQL connection closed")


# Function to identify input
def identify_input(num):
    force_flag = (num >> 52) & 0x01
    if force_flag == 1:
        return 0 # Force Data
    elif (num>>47) > 0:
        return 1 # Periodic Data
    else:
        return 2 # Wrong data

    
def parse_periodic_data(data):    
    # Bitwise shifting to get individual fields
    kecamatan = (data >> 47) & 0x1F
    pelanggan = (data >> 33) & 0x3FFF
    tamper_flag = (data >> 32) & 0x01
    konsumsi_air_bytes = data & 0xFFFFFFFF

    # Interpret the konsumsi_air_bytes as a float
    konsumsi_air = struct.unpack('f', konsumsi_air_bytes.to_bytes(4, byteorder='little'))[0]

    # Format the fields
    formatted_kecamatan = f"{kecamatan:02}"
    formatted_pelanggan = f"{pelanggan:05}"

    # Construct perangkat_id
    perangkat_id = f"{formatted_kecamatan}_{formatted_pelanggan}"

    return {
        "kecamatan": kecamatan,
        "pelanggan": pelanggan,
        "tamper_flag": tamper_flag,
        "konsumsi_air": konsumsi_air,
        "perangkat_id": perangkat_id
    }

def parse_force_data(data):
    # Extract individual fields
    force_flag = (data >> 52) & 0x01
    kecamatan = (data >> 47) & 0x1F
    pelanggan = (data >> 33) & 0x3FFF
    tamper_flag = (data >> 32) & 0x01
    konsumsi_air_bytes = data & 0xFFFFFFFF

    # Interpret the konsumsi_air_bytes as a float
    konsumsi_air = struct.unpack('f', konsumsi_air_bytes.to_bytes(4, byteorder='little'))[0]

    # Format the fields
    formatted_kecamatan = f"{kecamatan:02}"
    formatted_pelanggan = f"{pelanggan:05}"

    # Construct perangkat_id
    perangkat_id = f"{formatted_kecamatan}_{formatted_pelanggan}"

    return {
        "kecamatan": kecamatan,
        "pelanggan": pelanggan,
        "tamper_flag": tamper_flag,
        "konsumsi_air": konsumsi_air,
        "perangkat_id": perangkat_id
    }

    
#Main Program
def main(request):
    # Access the request data
    request_data = request.get_json()

    try:
        # Extract the 'con' field from the request data
        con_content = json.loads(request_data.get('m2m:sgn', {}).get('m2m:nev', {}).get('m2m:rep', {}).get('m2m:cin', {}).get('con', None))
        
        if con_content is None:
            raise ValueError("No 'con' field found or it's None")

        # Extract the 'data' field from the 'con' content
        data_value = con_content.get('data', None)
        
        if data_value is None:
            raise ValueError("No 'data' field found in 'con' or it's None")
        
        # Attempt to convert 'data' to an integer
        input = int(data_value,16)
        print("Hex String value of 'data' in request: ",data_value)
        print("Integer value of 'data' in request:", input)
        
        # Proceed with further processing if necessary
        
    except (TypeError, ValueError) as e:
        print("The request data:", request_data)
        print("Error:", e)
        return "Input Structure not recognized", 200

    # Identify the input
    # input = 0b000001000000000000011 #example input kondisi casing
    # input = 0b100000100000000000001000000000000000000001111 #example input
    input_type = identify_input(input)
    print(f"Input Type: {input_type}")

    # Process the input
    if input_type == 0:
        # Process force data
        result = parse_force_data(input)
        print("Parsed Data:")
        print(result)
        update_force_data(result["konsumsi_air"], result["perangkat_id"], result["tamper_flag"])
        return "force data perangkat berhasil diproses!",200

    elif input_type == 1:       
        # Process periodic data
        result = parse_periodic_data(input)
        print("Parsed Data:")
        print(result)
        update_data_cumulative(result["konsumsi_air"], result["perangkat_id"], result["tamper_flag"])
        return "Periodic data perangkat berhasil diproses!",200
    else:
        print(input)
        return "Input not supported", 200
    else:
        print(input)
        return "Input not supported", 200
