# ここでは、Lit-langのコードを書くときに気をつけるべきことを書いていきます。

## 変数
### 配列
- *配列型の変数* を使うときは注意が必要かもしれません。例えば:
```rb
for i in 0...10
    a = new 10 int
    # something
end
```
- 上記のようなコードを書くと、変数``a``が毎回宣言されることになってしまい、GCにもパフォーマンス的にも悪いです。なので:
```rb
a = []:int
for i in 0...10
    a = new 10 int
    # something!
end
```
- のようにすると、いいです。

### 参照
- 現在、Lit-langでは *参照* をサポートしていません。なので：
```rb
def f(ref a)
    a = 10
end
a = 1
f a
puts a # hope '10', but '1'
```
- のようなコードは思った通りには動きません。エラーになるでしょう。

## GC
- 現在、Lit-langには **Mark&Sweep** のGCがスクラッチから実装してあります。 ですが、まだバグが多いので、バグかな？と思ったらコードを眺めることをおすすめします:) (waiting for Pull Request!)
