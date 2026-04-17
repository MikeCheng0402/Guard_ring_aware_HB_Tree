# Guard Ring Aware H-B Tree

本專案用於 FinFET placement，核心流程為：

1. 讀入 SPICE netlist
2. 視需要讀入 symmetry / proximity constraints
3. 進行 H-B Tree placement 與 simulated annealing
4. 輸出每個 transistor 的座標資訊
5. 後續可再轉成 `.tcl` 並匯入 LAKER 產生 layout

註記：相關資料與實際執行環境以 TSRI 的 `a74t16c1` 帳號內容為準。

## Repository Structure

- `main.cpp`: 程式進入點
- `src/`: C++ 實作
- `inc/`: 標頭檔
- `Spice_file/`: 測試用 SPICE、symmetry、proximity 範例
- `visualize_placement.py`: 將 placement 輸出轉成 PNG 圖檔
- `tests/`: Python 與 CLI 測試
- `Makefile`: 主要編譯與執行入口

## Build

建議直接使用 `Makefile`：

```bash
make
```

會產生執行檔：

```bash
./placement
```

若要手動編譯，可使用：

```bash
g++ -I ./inc -std=c++17 -O2 -Wall main.cpp src/*.cpp -o placement -lreadline -lcurses
```

## Run

### 1. 不含 constraints

```bash
./placement spice_file output_file
```

範例：

```bash
./placement Spice_file/test.sp out.txt
```

### 2. 含 symmetry 與 proximity constraints

```bash
./placement spice_file symmetry_file proximity_file output_file
```

範例：

```bash
./placement \
  Spice_file/opamp_right.sp \
  Spice_file/opamp_right_sym.txt \
  Spice_file/opamp_right_proximity.txt \
  opamp_right_out.txt
```

說明：

- `symmetry_file` 代表 symmetry constraints
- `proximity_file` 代表 proximity constraints
- 若未提供這兩個檔案，程式會視為沒有 symmetry / proximity constraints
- 輸出檔會包含每個 transistor 的座標資訊

## Makefile Shortcuts

直接編譯：

```bash
make
```

使用 `Makefile` 內建案例執行：

```bash
make run
```

清除編譯結果：

```bash
make clean
```

目前 `Makefile` 內建測試案例為 `opamp_right`，可在 `Makefile` 的 `CASE` 變數中修改。

## Output Visualization

若已經有 placement 輸出檔，可使用 Python 工具轉成 PNG：

```bash
python3 visualize_placement.py output_file
```

圖檔會輸出到：

```bash
picture/<output_file_stem>.png
```

此工具會：

- 解析 placement 結果
- 標示 guard ring 與 device
- 依 layout 尺寸自動調整輸出圖片大小

## Tests

Python 測試：

```bash
pytest
```

CLI 測試：

```bash
bash tests/cli_args_test.sh
```

## TCL / LAKER Flow

placement 結果產生後，可以再用其他工具轉成 `.tcl`。

舊版流程提到可使用 `changed program` 資料夾中的 `generator.exe` 產生 `.tcl`；更細節的使用方式可參考學長留下的相關說明文件。

產生 `.tcl` 後，可在 TSRI 伺服器的 `iPDK_SYNS` 環境中開啟 LAKER，並於 LAKER command window 執行：

```tcl
source XXXX.tcl
```

即可匯入 `.tcl` 並生成 layout。

若需要更多 LAKER TCL 功能，可參考 `lakerUserGuide` 中的 `tcl.pdf`。

## Current Limitations

目前這份資料夾中尚未包含以下功能的 implementation：

- transistor array
- guard ring merging
- guard ring splitting
