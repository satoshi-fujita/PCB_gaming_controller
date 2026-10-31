# 物理ボタン実機テスト追加 設計（design.md）

対象: `.steering/20260926-add-button-hardware-test/`
参照元（承認済み）: `.steering/20260926-add-button-hardware-test/requirements.md`

## 1. 実装アプローチ

PlatformIO Unit Testing（テストフレームワーク: Unity）で、AtomS3実機上で動くテストを1本追加する。

- テストは `setup()` の中で順番に実行し、各テストの冒頭で画面とシリアルに操作指示を出す
- 人の操作は「制限時間内に期待した状態になるまでポーリングして待つ」方式で待ち受け、時間切れならFAILにする
- `InputManager` を実際に使い、アプリ本体と同じ初期化手順（`M5.begin()` → `InputManager::begin()`）でテストする

### 1.1 `src/` をテストから使うための対応

`src/InputManager.cpp` 等をテストで使うため、`platformio.ini` に `test_build_src = yes` を追加する。
このままでは `src/main.cpp` の `setup()`/`loop()` とテスト側の `setup()`/`loop()` が重複してリンクエラーになる。
PlatformIOはテストビルド時にマクロ `PIO_UNIT_TESTING` を定義するため、`main.cpp` 全体を `#ifndef PIO_UNIT_TESTING` で囲み、テスト時だけ除外する。通常の `pio run` には影響しない。

### 1.2 暫定対応（右タッチ=OK）の扱い

`InputManager::okPressed()` は右タッチでもtrueになる。ボタンによる検出であることを区別するため、OKのエッジを検出した瞬間に `digitalRead(G38) == HIGH` であることも併せて確認する。
テスト中はタッチパッドに触れないよう、画面に注意書きを表示する。

## 2. 変更するコンポーネント

| ファイル | 変更内容 |
|---|---|
| `test/test_buttons/test_main.cpp` | 新規。テスト本体 |
| `platformio.ini` | `test_build_src = yes` を追加 |
| `src/main.cpp` | 全体を `#ifndef PIO_UNIT_TESTING` 〜 `#endif` で囲む |
| `docs/development-guidelines.md` | 「4. テスト規約」を更新し、実機テスト（`test/`）の利用と実行方法を追記 |
| `docs/repository-structure.md` | `test/` の説明を「現状未使用」「スコープ外」から、実機テストの配置先に更新 |

## 3. テストケース

ピン番号はテスト側でも定数として持つ（`InputManager` のピン定数はprivateのため）。制限時間は操作待ち1回あたり10秒とする。

| No | テスト名 | 操作指示 | 判定内容 |
|---|---|---|---|
| 1 | `test_idle_pins_low` | 何も触らない | 1秒間サンプリングし、G38/G39とも常にLOW |
| 2 | `test_ok_button` | OKを押す→離す | 押下待ち中に `cancelPressed()` が一度もtrueにならない。`okPressed()` がtrueになった瞬間にG38がHIGH、G39がLOW。押してから離すまでの間、`okPressed()` がtrueになるのは1回だけ。離した後にG38がLOWに戻る |
| 3 | `test_cancel_button` | キャンセルを押す→離す | No.2のOKとキャンセルを入れ替えた内容 |
| 4 | `test_idle_no_false_edges` | 何も触らない | 3秒間 `update()` を回し、`okPressed()`/`cancelPressed()` とも一度もtrueにならず、G38/G39とも常にLOW |

- No.2/3の「1回だけ」の判定で、デバウンスが効いてチャタリングによる多重検出がないことを確認する
- No.2/3の「もう一方が反応しない」の判定で、配線の取り違え・短絡がないことを確認する
- 失敗時は `TEST_ASSERT_*_MESSAGE` で、ボタン名と観点（例: `"OK: timeout waiting for press"`, `"OK: CANCEL edge detected while pressing OK"`）を出力する

## 4. 画面・シリアル表示

- 画面: `M5.Lcd` で黒背景に指示文（例: `PRESS OK`, `RELEASE OK`, `DON'T TOUCH`）とテスト番号を表示する。テスト用の最小限の表示のため `GUI` クラスは使わない
- シリアル: 同じ指示文を `Serial.println()` で出力する（Unityの結果出力と同じシリアル）
- 全テスト終了後、画面に `DONE` を表示する

## 5. 影響範囲の分析

- アプリ本体: `main.cpp` にプリプロセッサのガードを追加するだけで、`pio run` のビルド結果は変わらない
- `InputManager` 他の `src/` のクラス: 変更なし
- `pio test` 実行時は `src/` 全体（GUI・GameStateMachine等）もリンクされるが、テストからは `InputManager` と `SettingsStore`（感度しきい値の取得）だけを使う

## 6. 実機テストで判明した配線とファーム変更（2026-09-26追記）

実機テスト実行時、ボタンを押していないのに `PRESS OK` が進む挙動を確認。ピン電圧を測定した結果:

- OFF（未押下）: GND基準で 0.3V、ON（押下）: 0V → **どちらもLOW**。ピンは一度もHIGHにならない。

つまりボタン配線は **active-low（押すとGNDに落ちる）** だったが、ファームは `INPUT_PULLDOWN`＋「HIGHで押下」の **active-high** 前提だったため、物理ボタンは一切反応していなかった（`PRESS OK` が進んだのは右タッチ=OK暫定対応＋タッチのドリフトによる偽エッジ）。

対応（ユーザー承認済み: ファーム側をactive-lowに）:

- `src/InputManager.cpp` の `begin()`: `INPUT_PULLDOWN`/`GPIO_PULLDOWN_ONLY` → `INPUT_PULLUP`/`GPIO_PULLUP_ONLY`、ボタン初期状態の判定を `== HIGH` → `== LOW` に変更
- `src/InputManager.cpp` の `readDebounced()`: `raw = (digitalRead(pin) == HIGH)` → `== LOW`
- `test/test_buttons/test_main.cpp`: ピン判定を active-low（押下=LOW）に修正。さらに、右タッチ=OK暫定対応によるタッチ偽エッジに惑わされないよう、押下検出を「InputManagerのエッジ報告 かつ 対象ピンが押下レベル」の複合条件にし、ボタン独立性の確認をピンレベル（`digitalRead`）で行うよう変更
