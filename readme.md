# ogri

ogriは**文章のように書ける**ように設計された動的型付けプログラミング言語です。  
英文のような自然な構文を持ち、記号の入力をなるべく少なく、流れるようにプログラミングを楽しむことができます。

#### ⚠️注意事項(Limitations)
*!!現在まだGC(ガベージコレクタ)は未実装です。実行中にメモリの解放は行われないため、すべてのメモリはプロセス終了時にOSによって回収されます。 長時間の実行や、大量のメモリを消費する処理を行う際はご注意ください。*
## 💡特徴

* **Shiftキーの排除**: 構文はすべて小文字と記号(`,`, `.`)でかけるように構成されています。
    * *文字列リテラル(`"`)の入力時にShiftキーが必要です。*
    * *演算子(`==`, `!=`, `<`, `>`, `<=`, `>=`, `+`, `-`, `*`, `/`, `%`)はそのまま書くこともできます。*
* **英語のような文**: 全ての文はピリオド`.`で終わります。
* **スマートな代入**: 単数代入の `is` と、リスト化・複数代入の `are` を、英語の文法に合わせて使い分けます。

## 🚀インストール方法

### 前提条件
Cコンパイラ(clang/gcc), Bison, Flex が必要です。

### ビルドとインストール
1. リポジトリをクローンします。
```bash
git clone https://github.com/OgamiRikuto/ogri.git
cd ogri
```
2. インタープリターをビルドします。
```bash
make
```
3. インストール(任意): `ogri`コマンドをシステムに登録します(sudoとパスワードが必要です)。
    * `/usr/local/bin`に実行ファイルをコピーしています。
```bash
sudo make install
```



## 📖使い方
インストールした場合はターミナル上でどのディレクトリにいても
```bash
ogri
```
で実行できます。  
インストールしていない場合はリポジトリをクローンしたディレクトリにて
```bash
./ogri
```
で実行できます。

スクリプトファイルを指定して実行します。
```bash
ogri examples/main.ogri
```

## 🗒️構文ガイド
### 1. 変数と代入(`is`と`are`)
ogriでは、単一の値の代入と、リストや複数変数の扱いで`is`と`are`を使い分けます。
```
// 単一代入 (is)
name is "Alice".
age is 20.

// リスト作成 (are)
numbers are 1, 2, 3.
single_list are 10. // 値が一つでもリスト[10]が作られます。

// アンパック / 複数代入 (are)
x, y are 10, 20.    // x = 10, y = 20 と代入されます。
```

### 2. 出力とf文字列
出力には`say with`と`says with`を使います。f文字列`f"..."`の中では角括弧`[...]`を使って変数を埋め込めます。
```
say with "Hello World". // say は出力に改行を含みます。
says with "Hello ogri\n". // says は出力に改行を含みません。
say with f"My name is [name] and I am [age] years old.".
```

### 3. 制御構文
条件分岐(`when`/`otherwise`):
```
when age is larger than 18,
    say with "You are an adult.".
otherwise when age is same as 18,
    say with "Happy Birthday!".
otherwise,
    say with "You are a minor.".
```

繰り返し(`repeat`):
```
// Foreach ループ
colors are "Red", "Green", "Blue".
repeat color foreach colors that
    say with f"Color: [color].".

// Until ループ (条件が満たされるまで繰り返す)
count is 0.
repeat until count is larger than 5 that
    say with count.
    count is count plus 1.
```

### 4. 演算子
記号の代わりに単語を使うことができます。  
| ogri | 意味 | 一般的な記号 |
| --- | --- | --- |
| plus | 足す | `+` |
| minus | 引く | `-` |
| times | 掛ける | `*` |
| divided by | 割る | `/` |
| remainder | あまり | `%` |
| is same as | 等しい | `==` |
| is not same as | 等しくない | `!=` |
| is less than | 未満 | `<` |
| is larger than | より大きい | `>` |
| is not larger than | 以下 | `<=` |
| is not less than | 以上 | `>=` |

### 5. 関数定義と関数呼び出し
`define ... that`を使用して関数を定義します。  
関数の呼び出しには`with`を用います。
```
// 引数なしの関数定義
define greet that
    say with "Hello!".

greet with. // 引数がなくても with は必要です。

// 引数を指定する場合は using を使います。
define greet_with_name using name that
    say with f"Hello, [name]".

greet_with_name with "Bob".

gn is greet_with_name.  // withをつけないことでオブジェクトとして扱うことができます。
gn with "Bob".
```

### 6. その他
仕様の詳細は、[examples/manual.ogri](examples/manual.ogri) にて**実行可能なマニュアル**として用意しています。  
スライスや組み込み関数の使い方も網羅されているたぜひ一度実行して挙動を確認してみてください。

## 🧩サンプルコード: FizzBuzz
```
define fizzbuzz using limit that
    number is 1.

    repeat until number is larger than limit that
        is_fizz is number remainder 3 is same as 0.
        is_buzz is number remainder 5 is same as 0.

        when is_fizz and is_buzz,
            say with "FizzBuzz".
        otherwise when is_fizz,
            say with "Fizz".
        otherwise when is_buzz,
            say with "Buzz".
        otherwise,
            say with number.
        
        number is number plus 1.

fizzbuzz with 20.
```

## Syntax Diagram
本言語の構文定義(Syntac diagram / EBNF) は [syntax/diagram.md](syntax/diagram.md)で確認できます。

