# 初回実装 要求内容（requirements.md）

対象: `.steering/20260802-initial-implementation/`
参照元（承認済み）: `docs/product-requirements.md`, `docs/functional-design.md`, `docs/architecture.md`, `docs/repository-structure.md`, `docs/development-guidelines.md`, `docs/glossary.md`

## 1. 変更・追加する機能の説明

現状の`main.cpp`/`GUI.cpp`/`GUI.h`（タッチ4chの値を円の大きさとして表示するだけの動作確認プログラム）を土台に、`docs/functional-design.md`で設計した「イライラ棒」ゲームを一から実装する。今回の初回実装で作る範囲は以下の通り（`docs/product-requirements.md`のスコープ全体に相当）。

1. `InputManager`: タッチ4ch（G5/G6/G7/G8）読み取り・起動時キャリブレーション・しきい値判定、物理ボタン2個（G38=OK, G39=キャンセル）のデジタル読み取り・デバウンス
2. `GameStateMachine`: `GameState`（TITLE/TUTORIAL/SETTINGS/DIFFICULTY_SELECT/PLAYING/GAME_OVER）の保持と遷移
3. `SettingsStore`: しきい値（感度）3段階（弱・中・強）の保持・変更
4. `DifficultyConfig`: 難易度レベル1〜3ごとの自機サイズ・壁パラメータのテーブル
5. `Player`: 自機の座標・サイズの保持と移動（画面左側基準、十字キー入力で上下左右に微調整）
6. `Wall` / `WallManager`: 壁の生成（開口位置・サイズ）、右→左スクロール、画面外での消去、経過時間に応じた開口・出現間隔の縮小
7. `CollisionChecker`: 自機と壁群の当たり判定
8. `GUI`（Renderer拡張）: 6画面（タイトル/チュートリアル/設定/難易度選択/プレイ中/ゲームオーバー）の描画
9. `main.cpp`: 上記コンポーネントを初期化し、`GameStateMachine`を軸に`loop()`を組み立てるオーケストレーション

## 2. ユーザーストーリー（今回実装で満たすもの）

`docs/product-requirements.md`「6. ユーザーストーリー」に記載の全項目を対象とする。抜粋:

- 電源を入れたらタイトル画面が表示され、「チュートリアル」「ゲーム開始」「設定」から選べる
- タイトル画面を見ただけで十字キー・OK・キャンセルボタンの役割が分かる
- チュートリアルで自機の動かし方と壁接触＝ゲームオーバーを体験的に理解できる
- 設定画面でタッチ値のしきい値（感度、3段階）を選べる
- 「ゲーム開始」選択後に難易度（レベル1〜3）を選び、自機サイズ・難易度に反映される
- 十字キーで自機を上下左右に細かく動かして壁を避けられる
- 壁に触れたら失敗（ゲームオーバー）とわかり、生存時間が確認できる
- 失敗してもOKボタンで即リトライ、プレイ中・各画面からキャンセルでタイトルに戻れる

## 3. 受け入れ条件

`docs/product-requirements.md`「7. 受け入れ条件」の全項目を満たすこと。加えて実装時の技術的な受け入れ条件として以下を追加する。

- [ ] `pio run`でビルドが成功する
- [ ] 実機（AtomS3）に書き込み、タイトル→チュートリアル/設定/難易度選択→プレイ→ゲームオーバー→リスタート／タイトル復帰の一連の流れが動作する
- [ ] `docs/repository-structure.md`で定義したファイル構成（1クラス1組の.h/.cpp）に沿っている
- [ ] `docs/development-guidelines.md`の命名規則・コーディング規約に沿っている

## 4. 制約事項

- `docs/architecture.md`の技術的制約（タッチのアナログ特性・個体差対応、128x128画面制約、メモリ制約、状態機械での一元管理）に従う
- 永続保存（ベストスコア、設定値、難易度選択の記憶など）は今回のスコープ外とする（`docs/product-requirements.md`のスコープ外リストのとおり）。電源OFFで全てデフォルトに戻る前提で実装する
- `docs/product-requirements.md`「10. 未決事項」に列挙された数値パラメータ（しきい値3段階の具体値、難易度レベルごとの自機サイズ・壁パラメータ、壁スクロール速度、自機可動範囲、当たり判定形状、ボタン配線の論理レベル、チュートリアルの進行内容、タイトル画面操作説明の表現方法）は、次に作成する`design.md`で具体的な数値・方式として確定する
