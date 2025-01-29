# Operating systems
## 1st project
Create a bash script that can be used to analyse data from log files.
### How to use
`tradelog [FILTER] [COMMAND] [INPUT FILE]`
**Commands**
Only one can be used when running the script
- `list-tick`: Lists tickers
- `profit`: Lists profit from closed positions
- `pos`: Lists the value of currently held positions sorted in descending order of value
- `last-price`: Lists the last known prices for each ticker
- `hist-ord`: Prints a histogram of the number of transactions per ticker
- `graph-pos`: Prints a graph of the value of positions held by ticker
**Filters**
- `-a DATETIME`: Prints only records after this date
	- DATETIME is in format: YYYY-MM-DD HH:MM:SS
- `-b DATETIME`: Prints only records before this date
	- DATETIME is in format: YYYY-MM-DD HH:MM:SS
- `-t TICKER`: Prints records only for the given ticker. If used multiple times the ticker set is selected.
- `-w WIDTH`: Sets the width of the listing, filter cannot be used multiple times.
project - 15/15