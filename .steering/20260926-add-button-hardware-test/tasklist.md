# 物理ボタン実機テスト追加 タスクリスト（tasklist.md）

対象: `.steering/20260926-add-button-hardware-test/`
参照元（承認済み）: `requirements.md`, `design.md`

## 1. 実装タスク

- [x] T1: `platformio.ini` に `test_build_src = yes` を追加する
- [x] T2: `src/main.cpp` 全体を `#ifndef PIO_UNIT_TESTING` 〜 `#endif` で囲む
- [x] T3: `test/test_buttons/test_main.cpp` を作成する
  - [x] 画面・シリアルへの指示表示ヘルパー
  - [x] `test_idle_pins_low`
  - [x] `test_ok_button`
  - [x] `test_cancel_button`
  - [x] `test_idle_no_false_edges`
  - [x] `setup()` での `M5.begin()` → `InputManager::begin()` → `UNITY_BEGIN()` 〜 `UNITY_END()`
- [x] T4: `docs/development-guidelines.md`「4. テスト規約」を更新する
- [x] T5: `docs/repository-structure.md` の `test/` の説明を更新する
- [x] T6: 実機テストで配線がactive-lowと判明。`src/InputManager.cpp`をactive-low(INPUT_PULLUP/押下=LOW)に変更し、テストのピン判定も追従（design.md 6章）

## 2. 品質チェック

- [x] Q1: `pio run` でアプリ本体のビルドが成功する
- [x] Q2: `pio test -e m5stack-atoms3 --without-uploading --without-testing` でテストのビルドが成功する

## 3. 実機検証（ユーザー実施）

- [ ] V1: `pio test -e m5stack-atoms3` を実行し、画面の指示どおりに操作して全4テストがPASSする
- [ ] V2: OK押下待ちでボタンを押さずに放置し、`test_ok_button` がタイムアウトでFAILすること、メッセージで原因が分かることを確認する

## 4. 完了条件

- 上記T1〜T5、Q1〜Q2が完了している
- V1〜V2の実機検証結果をユーザーが確認している

## 5. 作業メモ

- Q2実施時、PATH上の`pio`（PIO Core 6.1.19, pypoetry venv）では`ModuleNotFoundError: No module named 'SCons.Tool.FortranCommon'`でテストビルドが失敗した。`~/.platformio/penv/Scripts/pio.exe`（PIO Core 6.2.0）では成功。PIO Coreが2つ混在していることによる環境問題で、コードの問題ではない
