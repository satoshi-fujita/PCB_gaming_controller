# 技術仕様書（architecture.md）

## 1. テクノロジースタック

| 分類 | 内容 |
|---|---|
| ハードウェア | M5Stack AtomS3（ESP32-S3、内蔵LCD 128x128、静電容量式タッチ対応GPIO） |
| 外付け入力 | 物理タクトスイッチ×2（OK＝GPIO39、キャンセル＝GPIO38） |
| フレームワーク | Arduino（`framework = arduino`、ESP-IDF v4.4.7ベース） |
| ビルドシステム | PlatformIO（`platformio.ini` の `env:m5stack-atoms3`） |
| 主要ライブラリ | [M5Unified](https://github.com/m5stack/M5Unified) ^0.2.13（本体制御・入出力の統一API）、[M5GFX](https://github.com/m5stack/M5GFX) ^0.2.19（描画エンジン、M5Unifiedの依存先） |
| 言語 | C/C++（`gnu99` / `gnu++11`、コンパイラは`xtensa-esp32s3-elf-gcc/g++`） |
| 永続化 | なし（本アプリのスコープでは不要。将来ベストスコア保存等を追加する場合はArduino `Preferences`ライブラリ（NVS）を想定） |

## 2. 開発ツールと手法

- **IDE/エディタ**: VS Code + PlatformIO IDE拡張（`.vscode/`にプロジェクト設定済み）
- **ビルド/書き込み/シリアルモニタ**: `pio run` / `pio run -t upload` / `pio device monitor`（`monitor_speed = 115200`）
- **デバッグ出力**: `USBSerial`（既存`main.cpp`と同様、USB CDCシリアルでログ出力する）
- **バージョン管理**: 本プロジェクトは現時点でGitリポジトリ未初期化。今後Git管理する場合は`.pio/`（ビルド成果物）を`.gitignore`済みであることを確認する
- **ドキュメント運用**: `CLAUDE.md`に定義した「永続的ドキュメント（`docs/`）」「作業単位ドキュメント（`.steering/`）」の2層構成に従う
- **開発の進め方**: 本ドキュメント群（`docs/`）承認後、`.steering/[YYYYMMDD]-initial-implementation/`にrequirements.md／design.md／tasklist.mdを作成し、段階的に実装する

## 3. 技術的制約と要件

- **タッチ入力の特性**: ESP32-S3の`touchRead()`は静電容量値を返すアナログ的な値であり、個体差・湿度・周囲環境で基準値が変動する。起動時キャリブレーション（未接触状態の基準値記憶）と、しきい値3段階設定（弱・中・強）による吸収が必須（`docs/product-requirements.md` 8章、`docs/functional-design.md` データモデル参照）
- **GPIO制約**: タッチ4ch（G5/G6/G7/G8）は`touchRead()`対応ピンであることを前提とする。物理ボタン用のG38/G39は通常のデジタル入力として使用し、タッチ機能とは独立させる
- **画面解像度**: 128x128という限られた表示領域に、メニュー・ゲーム画面・操作説明を収める必要がある（`docs/functional-design.md`のワイヤフレーム参照）。フォントサイズ・レイアウトは可読性を優先し、詰め込みすぎない
- **メモリ制約**: ESP32-S3のRAM 327,680バイト、Flash 3,342,336バイト（現状のビルド確認では未実装状態でRAM 6.9%／Flash 14.3%使用）。壁オブジェクトの保持方法は固定長配列または軽量なリスト構造とし、動的メモリ確保（`new`/`malloc`の頻発）を避ける
- **状態管理**: `docs/functional-design.md`の`GameState`（TITLE/TUTORIAL/SETTINGS/DIFFICULTY_SELECT/PLAYING/GAME_OVER）を単一の状態機械で管理し、`loop()`内で現在状態に応じた更新・描画のみを行う構造とする
- **既存資産の扱い**: `main.cpp`のタッチ読み取り初期化ロジック、`GUI`クラスの構造は流用・拡張し、ゼロから作り直さない

## 4. パフォーマンス要件

- **入力〜反映の遅延**: タッチ入力から自機移動までの遅延が体感できるほど大きくならないこと。既存`main.cpp`の`delay(100)`ベースのループは、プレイ中の滑らかなスクロール表現には粗いため、プレイ中状態（`PLAYING`）ではより短い周期（目安: 30〜50ms程度）へ調整することを検討する。タイトル/設定など静的な画面では既存同様100ms程度でも問題ない
- **描画負荷**: `M5.Lcd.fillScreen()`による全画面再描画は既存コードと同様の方式を踏襲するが、壁オブジェクトが増えても1フレームの描画時間がプレイ体感を損なわない範囲に収まることを実機で確認する
- **難易度カーブ**: 壁の開口・出現間隔の縮小に下限値を設け、処理不能な頻度で壁が生成され続けないようにする（無限に壁オブジェクトが増加しないよう、画面外に出た壁は都度解放する）
- **電源投入から操作可能になるまでの時間**: 既存`main.cpp`にある「シリアル接続待ち（`while (!USBSerial)`）」は、USB未接続でも起動できるよう、ゲーム本編ではシリアル接続を必須の待機条件にしない（デバッグ出力の有無にかかわらず起動する）
