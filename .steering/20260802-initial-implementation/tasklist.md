# 初回実装 タスクリスト（tasklist.md）

`design.md`の実装アプローチに沿って、下位コンポーネントから積み上げる順に実装する。各タスクは`pio run`でのビルド成功を最低条件とし、可能な範囲で実機での動作確認を行う。

## タスク一覧

- [x] 1. `GameState.h` を作成し、`GameState` enum（TITLE/TUTORIAL/SETTINGS/DIFFICULTY_SELECT/PLAYING/GAME_OVER）を定義する
- [x] 2. `InputManager.h`/`.cpp` を作成する
  - 既存`main.cpp`のタッチ読み取り・キャリブレーション処理を移設する
  - `SettingsStore`から受け取ったしきい値（delta）を使い、上下左右4方向の「入力あり/なし」を判定する
  - GPIO38（OK）/GPIO39（キャンセル）を`INPUT_PULLUP`で読み取り、デバウンス処理を行う
  - 完了条件: シリアル出力で4方向＋OK/キャンセルの入力状態が正しく確認できる → **実機未検証**（後述）
- [x] 3. `SettingsStore.h`/`.cpp` を作成する（弱=30000/中=15000/強=6000の3値を保持、デフォルト=中）
- [x] 4. `DifficultyConfig.h`/`.cpp` を作成する（`design.md` 3.4節の表の値をレベル1〜3のテーブルとして実装）
- [x] 5. `Player.h`/`.cpp` を作成する（座標・サイズの保持、`InputManager`の判定結果に応じた移動、可動範囲x=12〜32/y=サイズ〜128-サイズのクランプ）
- [x] 6. `Wall.h`（構造体）と`WallManager.h`/`.cpp` を作成する
  - 壁の生成（開口位置をランダムに決定、開口サイズ・出現間隔は`DifficultyConfig`＋経過時間で決定）
  - 固定速度（2px/tick）でのスクロールと画面外での消去
  - 完了条件: 壁が右から出現し左へスクロールし、時間経過で開口・出現間隔が狭くなることをログまたは実機表示で確認できる → **実機未検証**（後述）
- [x] 7. `CollisionChecker.h`/`.cpp` を作成する（`Player`のバウンディングボックスと`WallManager`が保持する`Wall`群との矩形衝突判定）
- [x] 8. `GUI.h`/`.cpp` を拡張する
  - 既存の円描画デモコードを削除する
  - `drawTitle()`（メニュー3項目＋操作説明テキスト）
  - `drawTutorial(step)`（ステップ1: 自機のみ、ステップ2: サンプル壁＋自機）
  - `drawSettings(currentSensitivity)`
  - `drawDifficultySelect(currentLevel)`
  - `drawPlaying(player, walls, survivalTimeMs)`
  - `drawGameOver(survivalTimeMs)`
  - 完了条件: `docs/functional-design.md`のワイヤフレーム6画面それぞれに対応する描画関数が実装され、実機で表示確認できる → **実機未検証**（後述）。操作説明・メニュー等の文言は日本語フォント未導入のため暫定的に英語表記
- [x] 9. `GameStateMachine.h`/`.cpp` を作成する
  - 状態ごとの入力処理（十字キー/OK/キャンセル）と遷移ロジックを実装する（`docs/functional-design.md`の画面遷移図のとおり）
  - `PLAYING`中は`Player`・`WallManager`・`CollisionChecker`を毎ループ更新し、衝突時に`GAME_OVER`へ遷移する
- [x] 10. `main.cpp` を書き換える
  - `M5.begin()`後、`InputManager`・`GameStateMachine`・`GUI`を初期化する
  - `loop()`を「入力取得→`GameStateMachine`更新→`GUI`描画」の3ステップに整理する
  - プレイ中のtick周期を30ms、それ以外の画面は100ms程度にする（`design.md` 3.3節）
  - 既存の`while (!USBSerial) { delay(10); }`を削除し、USB未接続でも起動できるようにした
- [ ] 11. 実機ビルド・書き込みを行い、`requirements.md`「3. 受け入れ条件」を1項目ずつ確認する
  - `pio run`によるビルドは成功済み（RAM 6.8% / Flash 14.0%）。ただし本セッションではAtomS3実機が未接続のため書き込み・実機動作確認は未実施
- [ ] 12. 実機プレイテストの結果を踏まえ、`DifficultyConfig`の数値（開口・出現間隔・自機サイズ）およびしきい値3段階の数値を調整する

## 完了条件（このステアリング作業全体）

- `docs/product-requirements.md`「7. 受け入れ条件」の全項目を実機で満たしている
- `pio run`でのビルドが成功する
- `docs/repository-structure.md`のファイル構成・`docs/development-guidelines.md`の規約に沿っている
