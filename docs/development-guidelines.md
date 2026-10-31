# 開発ガイドライン（development-guidelines.md）

## 1. コーディング規約

- **インデント**: スペース4つ。タブは使用しない（既存`main.cpp`に合わせる。`GUI.cpp`は現状スペース2の箇所があるため、触る際はスペース4に揃える）
- **中括弧**: 開き括弧は行末に置く（K&Rスタイル。既存`main.cpp`/`GUI.cpp`と同じ）
- **ヘッダガード**: `#pragma once`を使用する（既存`GUI.h`を踏襲）
- **1ファイル1クラス**: `docs/repository-structure.md`のとおり、クラスとファイルを1:1対応させる
- **マジックナンバー禁止**: 壁の開口サイズ、しきい値、色コードなどの数値は名前付き定数（`constexpr`または`#define`）にする。既存`GUI.cpp`の`250000`のような直値は、該当箇所を触る際に定数化する
- **責務の分離**: 入力読み取り（`InputManager`）／ゲームロジック（`GameStateMachine`・`Player`・`WallManager`・`CollisionChecker`）／描画（`GUI`）の役割を混在させない（`docs/architecture.md`の制約に対応）
- **グローバル変数**: 新規コンポーネントでは極力使わず、各クラスが自身の状態を保持する設計にする。ただし`main.cpp`内でのコンポーネントのインスタンス保持（既存`GUI gui;`と同様の形）は許容する

## 2. 命名規則

| 対象 | 規則 | 例 |
|---|---|---|
| クラス名 | PascalCase | `WallManager`, `InputManager` |
| ファイル名 | クラス名と一致 | `WallManager.h` / `WallManager.cpp` |
| メソッド名 | camelCase | `drawGUI()`, `updateWalls()` |
| メンバ変数・ローカル変数 | camelCase | `touchValue`, `gapHeight` |
| 定数・マクロ | UPPER_SNAKE_CASE | `TOUCH_PIN1`, `WALL_GAP_MIN` |
| enum型・値 | 型はPascalCase、値はUPPER_SNAKE_CASE | `enum class GameState { TITLE, PLAYING, GAME_OVER }` |
| GPIOピン定義 | 既存`main.cpp`の`TOUCH_PIN1`等の命名に倣う | `BUTTON_PIN_OK`, `BUTTON_PIN_CANCEL` |

## 3. スタイリング規約（画面・UI）

- 画面解像度は128x128固定を前提にレイアウトする（`docs/functional-design.md`のワイヤフレームを正とする）
- 色はM5GFXが提供する`TFT_*`定数（`TFT_BLACK`, `TFT_RED`等）を使用し、独自のRGB値をハードコードしない
- テキストは詰め込みすぎず、フォントサイズは実機での視認性を確認しながら調整する
- 状態（`GameState`）ごとに描画関数を分け、1メソッドに複数画面分の描画ロジックを混在させない（`GUI`の`draw〜()`群を参照）

## 4. テスト規約

- 本プロジェクトは実機（AtomS3）上での動作確認を基本とする
- ハードウェアの配線・入力の確認など、実機でしか確認できない項目はPlatformIO Unit Testing（Unity）の実機テストとして`test/`配下に置く（配置ルールは`docs/repository-structure.md`参照）
  - 実行: `pio test -e m5stack-atoms3`（ビルドのみ確認する場合は`--without-uploading --without-testing`を付ける）
  - 人の操作が必要なテストは、画面とシリアルに操作指示を出し、制限時間内に期待した状態にならなければFAILとする
  - 失敗時に原因が分かるよう、`TEST_ASSERT_*_MESSAGE`で対象と観点をメッセージに含める
- PC上で動くnative環境のテストは現時点でスコープ外とする
- 新機能を追加した際は、最低限`pio run`でのビルド成功を確認する
- 可能な範囲で、衝突判定や難易度パラメータ計算などのゲームロジックはハードウェア（`M5.Lcd`や`touchRead()`）に直接依存しない形で実装し、将来的な自動テスト導入に備える
- 実機確認の観点（例: 「タッチ4方向それぞれで自機が動くか」「壁に当たるとゲームオーバーになるか」「難易度ごとに壁の狭まり方が変わるか」）は、`.steering/`配下のtasklistに検証項目として記載する

## 5. Git規約

- 本リポジトリは現時点でGit未初期化。Git管理を開始する場合は、機能・ドキュメント単位でコミットを分割する（例: 「docs: product-requirements.mdを追加」「feat: InputManagerを実装」等）
- コミットメッセージは日本語で構わないが、変更の「なぜ」が分かるように書く
- `.pio/`（ビルド成果物・ダウンロード済みライブラリ）はコミット対象外とする（`.gitignore`で除外）
