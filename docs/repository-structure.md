# リポジトリ構造定義書（repository-structure.md）

## 1. フォルダ・ファイル構成

```
theme32_kiban_meisi/
├── platformio.ini              # ビルド設定（board=m5stack-atoms3, framework=arduino）
├── CLAUDE.md                   # プロジェクトメモリ（ドキュメント運用ルール）
├── docs/                       # 永続的ドキュメント
│   ├── product-requirements.md
│   ├── functional-design.md
│   ├── architecture.md
│   ├── repository-structure.md
│   ├── development-guidelines.md   # (次work作成予定)
│   └── glossary.md                 # (次work作成予定)
├── .steering/                  # 作業単位ドキュメント
│   └── [YYYYMMDD]-[開発タイトル]/
│       ├── requirements.md
│       ├── design.md
│       └── tasklist.md
├── include/                     # プロジェクト共通ヘッダ（現状未使用、README のみ）
├── lib/                         # プロジェクト専用ライブラリ（現状未使用、README のみ）
├── src/                          # アプリ本体のソースコード（後述）
├── test/                         # PlatformIO Unit Testing（実機テスト）
│   └── test_buttons/             # 物理ボタン(G38/G39)の配線確認テスト
├── .vscode/                      # VS Code / PlatformIO IDE拡張の設定
└── .pio/                         # ビルド成果物・ダウンロード済みライブラリ（自動生成・Git管理対象外）
```

## 2. `src/` ディレクトリの役割

`docs/functional-design.md`のコンポーネント設計で定義したモジュールを、原則「1クラス＝1組の`.h`/`.cpp`」で配置する。

```
src/
├── main.cpp                # setup()/loop()。各コンポーネントの初期化とオーケストレーションのみを行う
├── GameState.h              # GameState enum の定義（状態一覧）
├── GameStateMachine.h/.cpp  # 状態遷移の管理（OK/キャンセル/衝突/時間経過をトリガーに遷移）
├── InputManager.h/.cpp      # タッチ4ch(G5-8)読み取り・キャリブレーション・しきい値判定、ボタン(G38/G39)読み取り・デバウンス
├── SettingsStore.h/.cpp     # しきい値レベル（弱/中/強）の保持・変更
├── DifficultyConfig.h/.cpp  # 難易度レベル1〜3ごとの自機サイズ・壁パラメータのテーブル
├── Player.h/.cpp            # 自機の座標・サイズの保持と移動
├── Wall.h                   # Wall構造体の定義（壁1本分のデータ）
├── WallManager.h/.cpp       # 壁の生成・スクロール・消去、開口/出現間隔の時間経過に伴う縮小
├── CollisionChecker.h/.cpp  # 自機と壁群の当たり判定
└── GUI.h/.cpp                # 既存クラスを拡張。GameStateごとの画面描画（Renderer）
```

- 既存の[main.cpp](../src/main.cpp) / [GUI.h](../src/GUI.h) / [GUI.cpp](../src/GUI.cpp)はこの構成の土台として維持・拡張する。
- `include/`と`lib/`は、PlatformIOの標準テンプレートとして用意されているが、本プロジェクトの規模では`src/`直下にヘッダ・実装をまとめる方針とし、当面は使用しない（README のみ残す）。

## 3. ファイル配置ルール

- クラス名とファイル名は一致させる（`PascalCase`、例: `WallManager.h` → `class WallManager`）。
- 複数の状態・画面にまたがって共有される型（`GameState`など）は、単独のヘッダ（`GameState.h`）に切り出し、循環参照を避ける。
- ロジックを持たないデータ保持のみのクラス（`SettingsStore`, `DifficultyConfig`）も、責務の見通しやすさを優先して同様に1クラス1ファイルとする。
- テストコードは、PlatformIOの規約に従い`test/`配下にテストケースごとのディレクトリ（`test_[対象]`）を作成し、その中に`test_main.cpp`を置く。テストから`src/`のクラスを使うため`platformio.ini`で`test_build_src = yes`とし、`src/main.cpp`は`#ifndef PIO_UNIT_TESTING`で囲んでテストビルドから除外する。
- `.pio/`はビルドごとに再生成されるため、リポジトリの構成管理上は無視してよい（`.gitignore`で除外済み）。

## 4. 作業単位ドキュメントとの対応

実装時は、本ドキュメントで定義した`src/`構成をもとに`.steering/[YYYYMMDD]-initial-implementation/tasklist.md`でファイル単位のタスクに分解する（例: 「`InputManager`を実装する」「`WallManager`を実装する」など）。
