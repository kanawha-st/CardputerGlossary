# Cardputer Glossary

M5Stack Cardputer用の英単語学習装置です。SDカード上の日本語訳を見て英単語を思い出し、4キーで学習状態を更新します。

## 操作

| キー | 動作 |
|---|---|
| `;` | 英単語を表示（SHOW） |
| `,` | 習得済みにする（DONE） |
| `.` | 連続OKをリセットし、20件後ろへ移動（NG） |
| `/` | OK。3回未満なら末尾へ、3回連続なら習得済み |

## SDカード

FAT32でフォーマットしたSDカードに、`data/academic_vocab`フォルダをディレクトリごとコピーしてください。

```text
/academic_vocab/
└── words.csv
```

初回起動時に進捗は全語未学習として扱われます。操作後は同じフォルダへ`progress.csv`が自動作成されます。

`words.csv`はUTF-8のCSVです。列は`id,word,meaning`で、IDは重複しない整数にしてください。カンマを含む値はダブルクォートで囲めます。

## ビルド

PlatformIOでプロジェクトを開き、`m5stack-cardputer`環境をビルドしてCardputerへ書き込みます。SDカードの内容はPlatformIOのfilesystem uploadではなく、PCのカードリーダーでコピーします。

