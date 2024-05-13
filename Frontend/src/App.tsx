import { useState, useEffect, ChangeEvent } from "react";
import axios from "axios";
import "./App.css";

interface DailyConsumption {
  perangkat_id: string;
  waktu_diterima: string;
  daily_consump: number;
}

interface MonthlyConsumption {
  perangkat_id: string;
  MM: number;
  YY: number;
  month_consump: number;
  cumul_consump: number;
}

interface StatPerangkat {
  perangkat_id: string;
  stat_komunikasi: number;
  stat_casing: number;
  waktu_casing: string;
}

interface ForceData {
  perangkat_id: string;
  waktu_diterima: string;
  force_consump: number;
}

// Define a type for the table configuration
type TableConfig = {
  [key: string]: {
    rows: number;
    columns: number;
  };
};

const SmartWaterMeterApp = () => {
  // State for the Konsumsi Bulanan and harian table
  const [bulanTahunIn, setBulanTahunIn] = useState("");
  const [bulanTahun, setBulanTahun] = useState("");
  const [bulan, setBulan] = useState(0);
  const [tahun, setTahun] = useState(0);
  const [fdPer, setFDPer] = useState("");
  const [fdData, setFDData] = useState<ForceData[]>([]);
  const [dailyData, setDailyData] = useState<DailyConsumption[]>([]);
  const [monthlyData, setMonthlyData] = useState<MonthlyConsumption[]>([]);
  const [statData, setStatData] = useState<StatPerangkat[]>([]);
  const api =
    "https://asia-southeast2-mystic-curve-420406.cloudfunctions.net/function-api";

  // Function to fetch Konsumsi Bulanan data from the server
  const fetchKonsumsiBulananData = async () => {
    try {
      // You need to replace the URL and API endpoint with your actual backend API endpoint
      const response = await axios.get(`${api}/month/${bulan}/${tahun}`);
      setMonthlyData(response.data); // Assuming the response is an array of data
    } catch (error) {
      console.error("Error fetching Konsumsi Bulanan data", error);
    }
  };

  // Function to fetch Konsumsi Harian data from the server
  const fetchKonsumsiHarianData = async () => {
    try {
      const response = await axios.get(`${api}/daily`); // Replace with your API endpoint
      setDailyData(response.data);
    } catch (error) {
      console.error("Error fetching Konsumsi Harian data", error);
    }
  };

  // Function to fetch Konsumsi Harian data from the server
  const fetchStatPerangkat = async () => {
    try {
      const response = await axios.get(`${api}/stat`); // Replace with your API endpoint
      setStatData(response.data);
    } catch (error) {
      console.error("Error fetching Konsumsi Harian data", error);
    }
  };

  // Function to fetch Konsumsi Harian data from the server
  const fetchForceData = async () => {
    try {
      const response = await axios.get(`${api}/force/${fdPer}`); // Replace with your API endpoint
      if (response.data.length === 0) {
        console.error("Invalid Format");
      } else {
        setFDData(response.data);
      }
    } catch (error) {
      console.error("Error fetching force data", error);
    }
  };

  // Fetch data on component mount
  useEffect(() => {
    fetchKonsumsiHarianData();
    fetchStatPerangkat();
  }, []);

  const tableConfig: TableConfig = {
    konsumsiHarian: { rows: 6, columns: 3 },
    konsumsiBulanan: { rows: 3, columns: 4 },
    deviceStatus: { rows: 3, columns: 4 },
  };

  // Function to format the timestamp in 'Asia/Jakarta' time zone
  const formatTimestamp = (timestamp: string) => {
    const dateObj = new Date(timestamp);
    return dateObj.toLocaleString("en-US", { timeZone: "Asia/Jakarta" });
  };

  // Function to generate table cells
  const generateTableCells = (apiData: any[]) => {
    const cells = [];
    const rowCells = [];
    const cellData = 0 < apiData.length ? apiData[0] : null;
    rowCells.push(
      <td key={`force-PerID-col1`}>
        {cellData && (
          <>
            <div>{cellData.perangkat_id}</div>
          </>
        )}
      </td>
    );
    rowCells.push(
      <td key={`force-timestamp-col2`}>
        {cellData && (
          <>
            <div>
              {cellData.waktu_diterima &&
                formatTimestamp(cellData.waktu_diterima)}
            </div>
          </>
        )}
      </td>
    );
    rowCells.push(
      <td key={`force-fconsump-col3`}>
        {cellData && (
          <>
            <div>{cellData.force_consump}</div>
          </>
        )}
      </td>
    );
    rowCells.push(
      <td key={`force-statcasing-col3`}>
        {cellData && (
          <>
            <div>{cellData.stat_casing}</div>
          </>
        )}
      </td>
    );
    cells.push(<tr key={"ForceData"}>{rowCells}</tr>);

    return cells;
  };

  // Function to generate table cells with data from API response
  const generateTableCells2 = (tableType: string, apiData: any[]) => {
    const rows = tableConfig[tableType].rows;
    const cells = [];

    for (let i = 0; i < rows; i++) {
      const rowCells = [];
      const dataIndex = i;
      const cellData = dataIndex < apiData.length ? apiData[dataIndex] : null;
      rowCells.push(
        <td key={`harian-timeSt-${i}-col1`}>
          {cellData && (
            <>
              <div>
                {cellData.waktu_diterima &&
                  formatTimestamp(cellData.waktu_diterima)}
              </div>
            </>
          )}
        </td>
      );
      rowCells.push(
        <td key={`harian-PerID-${i}-col2`}>
          {cellData && (
            <>
              <div>{cellData.perangkat_ID}</div>
            </>
          )}
        </td>
      );
      rowCells.push(
        <td key={`konsumsiHari-${i}-col3`}>
          {cellData && (
            <>
              <div>{cellData.daily_consump}</div>
            </>
          )}
        </td>
      );

      cells.push(<tr key={`harian-${i}-row`}>{rowCells}</tr>);
    }

    return cells;
  };

  // Function to generate table cells with data from API response
  const generateTableCells3 = (tableType: string, apiData: any[]) => {
    const rows = tableConfig[tableType].rows;
    const cells = [];

    for (let i = 0; i < rows; i++) {
      const rowCells = [];
      const dataIndex = i;
      const cellData = dataIndex < apiData.length ? apiData[dataIndex] : null;
      rowCells.push(
        <td key={`bulan/tahun-${i}-col1`}>
          {cellData && (
            <>
              <div>{bulanTahun}</div>
            </>
          )}
        </td>
      );
      rowCells.push(
        <td key={`bulan-perangkat-${i}-col2`}>
          {cellData && (
            <>
              <div>{cellData.perangkat_id}</div>
            </>
          )}
        </td>
      );
      rowCells.push(
        <td key={`konsumsiBulanan-${i}-col3`}>
          {cellData && (
            <>
              <div>{cellData.month_consump}</div>
            </>
          )}
        </td>
      );
      rowCells.push(
        <td key={`konsumsiKumul-${i}-col4`}>
          {cellData && (
            <>
              <div>{cellData.cumul_consump}</div>
            </>
          )}
        </td>
      );

      cells.push(<tr key={`Bulanan-${i}-row`}>{rowCells}</tr>);
    }

    return cells;
  };

  // Function to generate table cells with data from API response
  const generateTableCells4 = (tableType: string, apiData: any[]) => {
    const rows = tableConfig[tableType].rows;
    const cells = [];

    for (let i = 0; i < rows; i++) {
      const rowCells = [];
      const dataIndex = i;
      const cellData = dataIndex < apiData.length ? apiData[dataIndex] : null;
      rowCells.push(
        <td key={`stat-id-${i}`}>
          {cellData && (
            <>
              <div>{cellData.perangkat_id}</div>
            </>
          )}
        </td>
      );
      rowCells.push(
        <td key={`stat-kom-${i}`}>
          {cellData && (
            <>
              <div>{cellData.stat_komunikasi}</div>
            </>
          )}
        </td>
      );
      rowCells.push(
        <td key={`stat-cas-${i}`}>
          {cellData && (
            <>
              <div>{cellData.stat_casing}</div>
            </>
          )}
        </td>
      );

      rowCells.push(
        <td key={`waktu-cas-${i}`}>
          {cellData && (
            <>
              <div>
                {cellData.waktu_casing &&
                  formatTimestamp(cellData.waktu_casing)}
              </div>
            </>
          )}
        </td>
      );

      cells.push(<tr key={`statPerangkat-${i}`}>{rowCells}</tr>);
    }

    return cells;
  };

  const handleInputBTChange = (e: ChangeEvent<HTMLInputElement>) => {
    setBulanTahunIn(e.target.value);
  };

  const validateInputFormat = () => {
    return bulanTahunIn && /^\d{1,2}\/\d{4}$/.test(bulanTahunIn);
  };

  const handleOK_BT_ButtonClick = () => {
    if (validateInputFormat()) {
      const [inputBulan, inputTahun] = bulanTahunIn.split("/");
      setBulan(parseInt(inputBulan, 10));
      setTahun(parseInt(inputTahun, 10));
      setBulanTahun(bulanTahunIn);
      fetchKonsumsiBulananData();
    } else {
      console.error("Invalid input format");
    }
  };

  const handleInputFDChange = (e: ChangeEvent<HTMLInputElement>) => {
    setFDPer(e.target.value);
  };

  const handleOK_FD_ButtonClick = () => {
    fetchForceData();
  };

  // Render the component
  return (
    <div className="app-container">
      {/* Part 1 */}
      <div className="part1">
        <div className="title-container">
          <h1>
            SMART WATER METER
            <br />
            TA232401006
          </h1>
        </div>
        <hr />
      </div>
      {/* 2.1 Konsumsi Harian Log Table */}
      {/* Part 2 */}
      <div className="part2">
        <div className="force-data-send">
          <h2>Force Data Send</h2>
          <input
            type="text"
            placeholder="Enter 'KK_PPPPP'"
            value={fdPer}
            onChange={handleInputFDChange}
          />
          <button onClick={handleOK_FD_ButtonClick}>OK</button>
          <table className="force-table">
            <thead>
              <tr>
                <th>Device ID</th>
                <th>Timestamp</th>
                <th>Konsumsi (L)</th>
                <th>Casing</th>
              </tr>
            </thead>
            <tbody>{generateTableCells(fdData)}</tbody>
          </table>
        </div>
        <div className="konsumsi-harian-log-table">
          <h2>Konsumsi Harian Log</h2>
          <table className="custom-table">
            <thead>
              <tr>
                <th>Timestamp</th>
                <th>Device ID</th>
                <th>Konsumsi air harian (L)</th>
              </tr>
            </thead>
            <tbody>{generateTableCells2("konsumsiHarian", dailyData)}</tbody>
          </table>
        </div>
      </div>

      {/* 2.2 Konsumsi Bulanan Log */}
      <div className="part3">
        <h2>Konsumsi Bulanan Log</h2>
        <input
          type="text"
          placeholder="Enter MM/YYYY"
          value={bulanTahunIn}
          onChange={handleInputBTChange}
        />
        <button onClick={handleOK_BT_ButtonClick}>OK</button>
        <table className="custom-table">
          <thead>
            <tr>
              <th>Bulan/Tahun</th>
              <th>Device ID</th>
              <th>Konsumsi bulanan (L)</th>
              <th>Total konsumsi air (L)</th>
            </tr>
          </thead>
          <tbody>{generateTableCells3("konsumsiBulanan", monthlyData)}</tbody>
        </table>
      </div>

      {/* 2.3 Device Status */}
      <div className="part4">
        <h2>Device Status</h2>
        <table className="custom-table">
          <thead>
            <tr>
              <th>Device ID</th>
              <th>Status Komunikasi</th>
              <th>Status Casing</th>
              <th>Timestamp Casing</th>
            </tr>
          </thead>
          <tbody>{generateTableCells4("deviceStatus", statData)}</tbody>
        </table>
      </div>
    </div>
  );
};

export default SmartWaterMeterApp;
