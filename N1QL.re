= N1QLクエリ

Couchbase ServerのN1QLクエリについて、SQLとの差分を中心に解説します(SQL標準と重複する部分を含めた網羅的な解説であることは意図されていません)。


== 基本構造

N1QLは、JSONデータを構成する文字列、数値、真偽値を（直接的に、つまり文字列としてエスケープされた形ではなく）含むことができます。以下に具体的な例を見てみます。

=== コンストラクションオペレーター

コンストラクションオペレーターと呼ばれる次の記号をクエリ内で直接用いることができます。

@<tt>{{} @<tt>{\}} @<tt>{:} @<tt>{[} @<tt>{]}

=== ネステッドオペレーター

ネステッドオペレーターと呼ばれる以下の表現を利用することができます。

 * ドットノーテーション （@<tt>{.}）:
ネストされたオブジェクト（サブドキュメント）へのパスをドット記号（@<tt>{.}）を使って表現できます。

 * 配列表現（インデックス、スライス）:配列へのアクセスに、インデックス（添字）や、スライス（範囲による指定）を利用することができます。

=== エスケープ文字

ユーザー定義語句（バケット名、フィールド名）は、「`」(バッククォート)を使ってエスケープすることによって、予約語との衝突を回避することができます。
Couchbase Serverでは、「-」は、予約語に含まれるため、「-」を含むバケット名を利用する場合、下記のようにバケット名をエスケープする必要があります。

//emlist{
SELECT ... FROM `travel-sample`
//}

== ドキュメントキーの利用

Couchbase Serverでは、クエリ内でドキュメントキーを使うことによって、効率的にクエリを実行することが可能です。

====[column]ドキュメントキーが利用できる場合のアプローチ
Couchbase Serverでは、ドキュメントキーが判明している場合には、ドキュメントキーを指定してドキュメントを取り出すことが最も効果的です。
ドキュメントキーが利用できる場合、必ずしもクエリを用いて(Queryサービスを介して)ドキュメントへアクセスする必要はなく、Dataサービスに対して直接リクエストすることで性能を最適化することが可能です。
一方、サブクエリを伴うような複雑なクエリの場合などのように、クエリの中でドキュメントキーを効果的に利用できる場面があります。

====[/column]

=== USE KEYS

//emlist{
SELECT *
FROM `travel-sample`.inventory.airport
USE KEYS "airport_1254";
//}

//emlist{
SELECT *
FROM `travel-sample`.inventory.airport
USE KEYS ["airport_1254","airport_1255"];
//}

@<tt>{USE KEYS}のみを利用してクエリを行う場合、インデックスを作成する必要がありません。



=== META().id

上記の@<tt>{USE KEYS}を利用したクエリは、下記のように @<tt>{META().id}を使って書き直すことができます。

//emlist{
SELECT *
FROM `travel-sample`.inventory.airport
WHERE META().id IN ["airport_1254","airport_1255"];
//}

@<tt>{META().id} を利用してクエリを行う場合、@<tt>{META().id}を用いたインデックスが作成されている必要があります。

@<tt>{USE KEYS}の構文では、後続する鉤括弧の中のドキュメントキーは完全一致が想定されていますが、@<tt>{META().id} を利用する場合、N1QLクエリの一部として、@<tt>{LIKE}と組み合わせることができ部分一致検索を行うことができる他、比較演算子による大小比較など、クエリの名で用いる際により自由度が高い表現と言えます。

また、検索条件としてのみではなく、 @<tt>{SELECT META().id FROM ... WHERE ...}のように、条件に一致するドキュメントキーを検索するために用いることもできます。

上記のクエリ中の@<tt>{META().id} の部分は、下記のようなキースペース(の別名)を含んだ表現(@<tt>{META(a).id} )を用いることができます。このクエリのように、クエリの中でキースペースが一意に決まっている場合、指定は必須ではありません。複数のキースペースが混在している中で、ドキュメントのキーを用いる（取り出す）場合には、キースペースを指定した表現を用います。

//emlist{
SELECT *
FROM `travel-sample`.inventory.airport a
WHERE META(a).id IN ["airport_1254","airport_1255"];
//}


== JSONから値のみを取得: RAW | ELEMENT | VALUE

@<tt>{RAW},@<tt>{ELEMENT},@<tt>{VALUE}, これらは全て同義語です（以下では、@<tt>{RAW}を用います）。

N1QLで検索された結果は、JSONデータとして、フィールド名と値のペアとなることが基本ですが、@<tt>{RAW} キーワードを @<tt>{SELECT}句と共に用いることで、フィールドの値のみを取り出すことができます。

比較のため、まず@<tt>{RAW} を用いない場合を見てみます。

//emlist{
SELECT city
FROM `travel-sample`.inventory.airport
ORDER BY city LIMIT 5;
//}

上記クエリの結果は、以下のようにJSONデータ（フィールド名と値のペア）の配列になります。

//emlist{
[
  {
    "city": "Abbeville"
  },
  {
    "city": "Aberdeen"
  },
  {
    "city": "Aberdeen"
  },
  {
    "city": "Aberdeen"
  },
  {
    "city": "Abilene"
  }
]
//}

次に、@<tt>{RAW} を用いた場合を見ていきます。


//emlist{
SELECT RAW city
FROM `travel-sample`.inventory.airport
ORDER BY city LIMIT 5;
//}

結果は、値の配列になります。

//emlist{
[
  "Abbeville",
  "Aberdeen",
  "Aberdeen",
  "Aberdeen",
  "Abilene"
]
//}

最後に、@<tt>{DISTINCT} と組み合わせた場合を見ます。

//emlist{
SELECT DISTINCT RAW city
FROM `travel-sample`.inventory.airport
ORDER BY city LIMIT 5;
//}

結果は、値の重複しない配列になります。

//emlist{
[
  "Abbeville",
  "Aberdeen",
  "Abilene",
  "Adak Island",
  "Addison"
]
//}


== 配列からデータを取得

@<tt>{ARRAY} または @<tt>{FIRST} から始まり、@<tt>{END} で基本単位となります。@<tt>{IN}または @<tt>{WITHIN} と @<tt>{FOR}により、コレクションの要素へのループを表現しています。

この構文を利用して、ドキュメントに含まれる配列が要素として、オブジェクト（サブドキュメント）を持っている時、そのオブジェクトのフィールドを配列として取り出すことができます。

//emlist{
( ARRAY | FIRST ) var1 FOR var1 ( IN | WITHIN ) expr1　END
//}

=== ARRAY

配列の要素全体を扱います（配列の要素に対して @<tt>{map} 操作を実行します）。

=== FIRST

@<tt>{ARRAY} を @<tt>{FIRST}に置き換えることで、配列のはじめの要素のみを扱います。

=== WHEN

@<tt>{WHEN} 句により、条件指定を付け加えることが可能です。配列の要素に対して@<tt>{filter} 操作を実行します。

//emlist{
( ARRAY | FIRST ) var1 FOR var1 ( IN | WITHIN ) expr1　[ ( WHEN cond1 [ AND cond2 ] ) ] END
//}

下記の通り、上記説明した要素の組み合わせにより複雑なステートメント用いることができます。

//emlist{
( ARRAY | FIRST ) var1 FOR var1 ( IN | WITHIN ) expr1　
[ ,var2 ( IN | WITHIN ) expr2 ]*
[ ( WHEN cond1 [ AND cond2 ] ) ] END
//}

== 配列を検索条件に利用

@<tt>{ANY}　または @<tt>{EVERY} から始まり、@<tt>{END}までで基本単位となります。@<tt>{SATISFIES} で条件を指定します。

//emlist{
( ANY | EVERY ) var1 ( IN | WITHIN ) expr1
[ , var2 ( IN | WITHIN ) expr2 ]*
SATISFIES condition END
//}

この構文は、配列から条件が真となった要素を取り出しているのではないことに注意してください。
条件が真となった際に、その配列が含まれているドキュメント(の指定されたフィールド)をクライアントに返します。
これは、SQLの WHERE条件が検索に使ったカラムを取り出すものではないのと同様です。

=== ANY

配列に条件で指定した要素が一つでも含まれる場合、真となります。

//emlist{
SELECT *
FROM retail.east.order o
WHERE ANY item IN o.lineItems SATISFIES
item.count >= 5 END
//}

=== EVERY

配列の全ての要素が、指定した条件に一致する場合、真となります。

//emlist{
SELECT *
FROM retail.east.order o
WHERE EVERY item IN o.lineItems SATISFIES
item.count >= 5 END
//}

=== IN | WITHIN

@<tt>{IN} 句が、指定された配列のトップレベルの要素を検索するのに対して、@<tt>{WITHIN} 句は、現在の配列とその子、および子孫を含めて検索対象とします。

== ドキュメントキーによる結合: NEST

@<tt>{NEST} は、外部の子ドキュメントを親の下に埋め込む特別なタイプの @<tt>{JOIN}です。
ドキュメント・キーを介した参照関係でデータをモデル化した際に用いる事ができます。@<tt>{ON KEYS}により、対象とするドキュメントのキーを指定することができます。


以下の2つのコレクションとドキュメントに対するクエリについて考えてみます(データの構造および内容は、あくまでこの箇所での説明のためのものであり、他の箇所と同じでないことにご注意ください)。

コレクション：@<tt>{retail.east.order}

//emlist{
{
"order_id":1234,
"customer_id":"34567",
"total_price":"65.5",
"lineitems":["o11","o12","o13"]
}
//}

コレクション：@<tt>{retail.east.lineItem}

//emlist{
{
"lineitem_id":o11,
"item_id":"789",
"qty":"3",
"itemprice":"5.99",
"base_price":"17.97",
"tax":"0.75",
"total_price":"18.22"
}
//}

//emlist{
{
"lineitem_id":o12,
"item_id":"234",
"qty":"5",
"itemprice":"10.00",
"base_price":"50.00",
"tax":"0.75",
"total_price":"50.75"
}
//}

//emlist{
SELECT ordr.order_id,
ARRAY {“item_id”: l.item_id, “quantity”:l.qty} FOR l IN line END as items
FROM retailsample.east.order ordr
NEST retailsample.east.lineItem line
ON KEYS ordr.lineitems
//}

結果は、下記のように、一つのドキュメントになります。

//emlist{
[
{
"items":[
    {"item_id":"789", "qty":"3"\},
    {"item_id":"234", "qty":"5"\}
  ];
  "order_id":"1234"
},
{
  "items":[
    {"item_id":"899", "qty":"8"\},
    {"item_id":"651", "qty":"2"\}
  ];
  "order_id":"9812"
}
]
//}

== サブドキュメントのフラット化: UNNEST

@<tt>{UNNEST}は、ネストされたオブジェクトを最上位ドキュメントとして表示するために用います。

下記のようなネストされた構造を持つデータを想定します（受注明細データを中に含む受注伝票データ）。

//emlist{
{
"ordId": "ORDER-0001",
"status": "Shipped",
"items": [
  {
  "prodId": "AAA-222",
  "qty": 1
  },
  {
  "prodId": "BBB-333",
  "qty": 2
  },
  {
  "prodId": "CCC-444",
  "qty": 3
  }
]
}
//}

@<tt>{UNNEST}は、このようなネストされた（サブドキュメントを含む）ドキュメントへのクエリの結果を下記のようなテーブル構造として利用したい時に使われます。

//table{
ordId	Status	prodId	qty
----------------------------
ORDER-0001	Shipped	AAA-222	1
ORDER-0001	Shipped	BBB-333	2
ORDER-0001	Shipped	CCC-444	3
//}


クエリでは、@<tt>{UNNEST}をサブドキュメントに対して指定し、@<tt>{as}で別名をつけたものを @<tt>{SELECT}句の中で使用します。

//emlist{
SELECT ord.ordId, ord.status, item.* FROM retail.east.order ord UNNEST items as item
//}

下記のようなフラットな構造のJSONが取り出されます。

//emlist{
[
{ “ordId”: “ORDER-0001”, “status”: “Shipped”, “prodId”: “AAA-222”, “qty”: 1 },
{ “ordId”: “ORDER-0001”, “status”: “Shipped”, “prodId”: “BBB-333”, “qty”: 2 },
{ “ordId”: “ORDER-0001”, “status”: “Shipped”, “prodId”: “CCC-444”, “qty”: 3 }
]
//}

上記のデータは、先述のテーブル形式データとの比較のために整形しています。
実際には、フィールドの出現順はこの通りではないことにご注意ください。JSONオブジェクトのフィールドはその登場順序に意味を持ちません。


== データ型


SQL利用者にとって、「NULL」という予約語や、@<tt>{IS [NOT] NULL}という比較演算は、馴染みがあるところです。
NULLは、データが存在しないことである、と言ったりします。より正確には、値が存在しない状態を表しています。ここで、データと値の違いはどこにあり、なぜ後者がより正確なのでしょうか？「データ」という表現は曖昧に取れる一方、「値」という言葉には、数学でいうところのXの値という使い方に見られるように（もちろん、プログラミングでいうところの、変数の値、でもいいわけですが）、単にそれ自体で存在しているデータを超えた含意があります。データベースのコンテクストでいえば、テーブルスキーマにおけるカラムは、一定のデータ型を持ちますが、数値型であれば0、文字列型であれば空文字（長さ0の文字）のようなデータ型固有の表現とは別に、値そのものが存在していない状態がNULLで表現されます。
JSONにおいても、「フィールドの値が未定義」であることを意味する「null」という予約語が定義されています。一方、JSONのように、データ自体にデータ構造の情報が含まれる（データとは別にスキーマが存在していない）、データにあっては、「フィールドが未定義」の場合を考慮する必要があります。

JSONデータに対して、N1QLによるクエリを実行した場合、他のドキュメントには存在するフィールドが、一部のドキュメントには存在しないことがあり得ます（RDB/SQLでは、テーブル内のすべてのレコードが同一のスキーマに従うため、このような状況が起こりません）。
N1QLには、そのようなギャップを埋めるため、@<tt>{MISSING}というキーワード（データ型）が用意されています。以下、N1QLにおけるデータ型としての、　@<tt>{NULL}と@<tt>{MISSING}の定義を確認した後に、N1QLにおける、存在しない値に対する比較演算について見ていきます。


=== MISSING


N1QLにおいて@<tt>{MISSING}は、JSONドキュメントでフィールド（名前と値のペア）が存在していない（欠落している）ことを表します。
N1QLにおけるSELECT文で、検索結果として返されるフィールドが存在しない（MISSINGである）データがある場合、（クエリ処理において、内部的にはMISSINGはリテラル式で扱われますが）、最終的な結果の配列中では、（JSONデータにおける未定義の値である）NULLに変換されます。


=== NULL


N1QLは、キーワードNULLを使用して空の値を表します。データ挿入、更新時に、フィールドの値をNULLにすることができます。


NULL値は、ゼロ除算や間違ったタイプの引数の受け渡しなど、特定の操作によっても生成され得ます。

NULLでは大文字と小文字が区別されません。たとえば、null、NULL、Null、およびnUllはすべて同等です。

Couchbase公式ドキュメント Data Types MISSING@<fn>{datatypes}

//footnote[datatypes][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/datatypes.html#datatypes]

== 比較演算


@<tt>{IS [NOT] NULL|MISSING|VALUED}演算子を使用すると、データセット内の属性の存在（または不在）に基づいて条件を指定できます。

これらは、@<tt>{IS [NOT] NULL}と@<tt>{IS [NOT] MISSING}は、データ型の定義から想像される通りの挙動となります。具体的なクエリと結果の例を見るのが、理解に役立つでしょう。


=== IS NULL

//emlist[][sql]{
SELECT fname, children
    FROM tutorial.sample.person
       WHERE children IS NULL
//}

//emlist[][json]{
{
  "results": [
    {
      "children": null,
      "fname": "Fred"
    }
  ]
}
//}

=== IS MISSING

//emlist[][sql]{
SELECT fname
   FROM tutorial.sample.person
      WHERE children IS MISSING
//}

//emlist[][json]{
{
  "results": [
    {
      "fname": "Harry"
    },
    {
      "fname": "Jane"
    }
  ]
}
//}

=== IS VALUED

@<tt>{NULL}と@<tt>{MISSING}は、JSONデータ上明らかに異なっており、区別する方法が必要である一方、アプリケーションにおける検索条件としてはこれらを区別する必要がない場合が考えられます。その際に@<tt>{AND}や@<tt>{OR}を使って複数の検索条件を並置する代わりに、N1QLでは、@<tt>{IS [NOT] VALUED}という比較演算が利用可能です。

 * @<tt>{IS VALUED}は、値が'MISSING'でも'NULL'でもない場合、TRUE（真）になります。
 * @<tt>{IS NOT VALUED}は、値が'MISSING'あるいは'NULL'である場合、TRUE（真）になります。


=== 参考情報

Couchbase公式ドキュメント Comparison Operators@<fn>{comparisonops}


== SQLと比べた場合の制約

 * @<tt>{FULL [OUTER] JOIN} はサポートされていません
 * @<tt>{CROSS JOIN} はサポートされていません
 * @<tt>{RIGHT [OUTER] JOIN} は、JOIN連結の最初の (または唯一の)クエリである必要があります


====[column]エディションによる差異

クエリに関連した、コミュニティエディションには含まれない、エンタープライズエディションの機能として、以下があります。

 * ウィンドウ関数
 * FLEXインデックス（クエリからの全文検索インデックス利用）
 * コストベースオプティマイザ（CBO）
 * 無制限のクエリ同時実行
 * N1QLアグリゲートプッシュダウン
 * N1QLリクエストの監査
 * クエリのモニタリング

====[/column]

== 結合

Couchbase ServerのN1QLで利用することのできる、以下の3つの結合のタイプについて解説します。
 * @<tt>{ANSI JOIN}
 * @<tt>{Lookup JOIN}
 * @<tt>{Index JOIN}

下の二つのJOINでは、混同しやすいCouchbase Server独自のキーワードが用いられます。それらは、@<tt>{ON KEYS}と @<tt>{ON KEY ... FOR} です。

ここでは、これらの用法について、以下のデータモデルを用いて、例示による整理を行います。

 * 「ルート」と「エアライン(航空会社)」という、二つの種類のデータモデルがある
 * 「ルート」モデルのデータ構造は、「エアライン」への参照（複数）を含む。参照は、「エアライン」ドキュメントのキーと一致している。

=== ANSI JOIN

ANSI JOINでドキュメントキーを用いた結合を行うためには、@<tt>{META().id}を利用し、適切なインデックスが作成されている必要があります。 


//emlist{
lhs-expr
JOIN rhs-keyspace
ON any join condition
//}


//emlist{
SELECT *
FROM `travel-sample`.inventory.route r
JOIN `travel-sample`.inventory.airline a
ON r.airlineid = META(a).id
//}

=== Lookup JOIN

@<tt>{ON KEYS}句を用います。ドキュメントキーを用いた結合を行うために、インデックスを利用する必要はありません。 


//emlist{
lhs-expr
JOIN rhs-keyspace
ON KEYS lhs-expr.foreign-key
//}



//emlist{
SELECT *
FROM `travel-sample`.inventory.route r
JOIN `travel-sample`.inventory.airline
ON KEYS r.airlineid
//}

=== Index JOIN

@<tt>{ON KEY ... FOR}句を用います。下記のクエリ例では、Lookup JOINのケースと@<tt>{FROM ... JOIN ...}で指定されているキースペース（コレクション）の順序が異なっていることに注目ください。 
Index JOINでは、適切なインデックスが作成されている必要があります。


//emlist{
lhs-keyspace
JOIN rhs-keyspace
ON KEY rhs-keyspace.idx_key
FOR lhs-keyspace
//}



//emlist{
SELECT *
FROM `travel-sample`.inventory.airline a
JOIN `travel-sample`.inventory.route r
ON KEY r.airlineid
FOR a
//}


=== 参考情報

Couchbase公式ドキュメント JOIN Clause@<fn>{63c6596bad48de18597b283a703bfc2f}

== レンジスキャン

=== 分散データベース一般のレンジスキャン

分散データベースにおいては、一般的に（キーを用いた）レンジスキャンを効果的に行うためには、論理的に関連性のあるキーを設計するのみではなく、あらかじめそのレンジに含まれるデータが、同じ、あるいは近い場所（ノード、リージョン、パーティション）に存在する様に、（物理）設計を行う必要があります。

例えば、MongoDBでは、シャーディングの方法として、ハッシュとレンジのいずれかを要件に応じ選択することになります（この選択が物理配置に影響します）。HBaseでは、データはRowKeyで（物理的に）ソートされます。 

=== Couchbase Serverの特殊性

Couchbase Serverは、N1QLというSQLをJSONのために拡張したクエリ言語で、検索を行うことができます。その際、重要なことは、Couchbase Serverの中心的なアーキテクチャーはKVS(キーバリューストア)であり、そのレベルでは、バリューとしてJSONデータを格納しているのに過ぎないため、検索を行うための前提として、インデックスの定義・構築が行われている必要があります。

そもそも、スキーマレスなデータストアに対して、どうやって検索が可能なのか(全件サーチするのでなければ)？という視点からは、これはむしろ納得できるものとして受け止められるかと思います。

ただし、ここで留意が必要なのは、KVSのキーを使った検索（つまりキーを特定したアクセスではなく、ある範囲のキーを指定してデータを一度に取得するという要件）であっても、事前にインデックスの作成が欠かせない、というところです。
このことをよりよく理解するために、以下の二つの背景を押させておくことが重要です。

 * Couchbase　Serverにおいては、クエリおよびインデックスサービスは、（KVSである）データサービスとは、アーキテクチャー上完全に独立している（利用する要件がない場合は、無効にすることができ、そのために潜在的にリソースが消費されることはない）。

 * Dataサービスのキーのシャーディング方法には、ハッシュが用いられており、ユーザによって変更する余地を残していない。（CouchbaseServerは、メモリファーストアーキテクチャーと、クラスターマップを用いたクライアントからのノードへの直接のアクセスによって、データをチャンクで取り出すよりも、高い性能を実現している）

=== N1QLによるレンジスキャン

下記のようなインデックスを作成します。

//emlist{
CREATE INDEX range_index ON MyFirstBucket(META().id);
//}

ここで、@<tt>{META().id} が、KVSのキー（ドキュメントID）に対応します。

@<tt>{MyFirstBucket} は、インデックスを作りたいバケットです。

インデックス作成のための構文は、@<tt>{CREATE INDEX [インデックス名] ON [キースペース名]([ドキュメントのフィールド名])}
というものです。バケット名の後のカッコ内に通常JSONドキュメントのフィールド名を指定しますが、ここではドキュメントの内容ではなく、ドキュメントが格納される際のキー/IDをインデックスとして利用するため @<tt>{META().id} という特殊なキーワードを用いています。

ここではキースペースの指定に@<tt>{MyFirstBucket} というバケット名のみを指定していますが、この場合、そのバケットの@<tt>{_default}スコープの@<tt>{_default}コレクションを指定したことになります。  


下記の様な、キーにレンジを指定した検索が可能になります。

//emlist{
SELECT * from MyFirstBucket where Meta().id BETWEEN 'ABC:001' AND 'ABC:999';
//}

また、下記の様な、キーの一部を指定した曖昧検索も可能になります。

//emlist{
SELECT * from MyFirstBucket where Meta().id LIKE 'ABC:%';
//}

=== DataサービスAPIによるレンジスキャン

Couchbase Serverで「キー」のみによるレンジスキャンを行うケースとして、下記の様なDataサービスへのAPI呼び出しとして実現することが考えられます。
（ここでは、Javaを用いたコード例を示しています）

同期型の例。

//emlist{
protected List<Object> get() {
  for (String key : keys) {
    GetResult result = collection.get(String.valueOf(key));
    listResults.add(result);
  }
  return listResults;
}
//}

非同期の例。

//emlist{
protected List<Object>get() {
  ReactiveCollection reactiveCollection = collection.reactive();

  Flux<Object> resultFlux = Flux.fromArray(keys.toArray())
    .flatMap( k -> reactiveCollection.get(String.valueOf(k)));

  List<Object> listResults = resultFlux.collectList().block();
  return listResults;
}
//}

上記のコードは、コード外で、`id` のリストを作成し、`keys`リストとして与えられることを想定しています。

//footnote[63c6596bad48de18597b283a703bfc2f][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/join.html#index-join-clause]


//footnote[comparisonops][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/comparisonops.html]

== ユーザー定義関数

Couchbase ServerのN1QLでは、ユーザーが関数を定義して、組み込み関数と同じように、任意の式で呼び出すことができます。

====[column]エディションによる差異
ユーザー定義関数は、エンタープライズエディションでのみ利用することができます。

====[/column]

ユーザー定義関数には次の2つの種類があります。

 * @<strong>{インライン関数}は、サブクエリを含むN1QL式を使用して定義されます。クエリを簡素化するために、複雑な式や、何度も用いられる式に名前を付けて定義し、再利用することができます。
 * @<strong>{外部関数}は、N1QLではなく、プログラミング言語を使用して定義されます。この機能により、N1QL式を使用して定義するのが困難または不可能な関数を作成することができます。（構文上は、他の言語の利用も想定される書式が用いられていますが、現時点で）サポートされている言語はJavaScriptのみです。

ユーザー定義関数には次の2つの呼び出し方法があります。

 * @<strong>{N1QL}の任意の式中で、組み込み関数と同じように、呼び出すことができます。ただし、Couchbase Serverのユーザー定義関数は、単純なインライン関数を超えた高い自由度を持ち、関数の中でドキュメントの変更などを行うことができる反面、このような「副作用」を持つ、ユーザー定義関数をN1QL式の中から呼び出した場合には、エラーが発生することに注意が必要です。
 * @<strong>{EXECUTE FUNCTIONステートメント}を使って、ユーザー定義関数を単体で直接実行することが可能です。ユーザー定義関数をテストする際に用いることができる他、@<tt>{EXECUTE FUNCTION}ステートメントでは副作用のある関数を実行することが可能なため、RDBMSにおけるストアドプロシージャのような使い方ができます(なお、Couchbase Serverでは、RDBMSにおけるトリガに類似する機能としてEventingサービスを利用することができます)。

====[column]エディションによる差異
Eventingサービスは、エンタープライズエディションでのみ利用することができます。

====[/column]

=== 参考情報

Couchbase公式ドキュメント User-Defined Functions@<fn>{4fec4dc4c082b58a3a0c563887d0169d}

Couchbase公式ドキュメント CREATE FUNCTION@<fn>{9691b9034b80d65250e6f45a580041f0}

Couchbase公式ドキュメント EXECUTE FUNCTION@<fn>{973a71b323540fcd67ec61262da9d1de}


//footnote[4fec4dc4c082b58a3a0c563887d0169d][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/userfun.html]

//footnote[9691b9034b80d65250e6f45a580041f0][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/createfunction.html]

//footnote[973a71b323540fcd67ec61262da9d1de][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/execfunction.html]


