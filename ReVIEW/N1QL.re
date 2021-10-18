= N1QLクエリ

Couchbase ServerのN1QLクエリについて、SQLとの差分を中心に解説します（SQL標準と重複する部分を含めた網羅的な解説であることは意図されていません）。

詳細については、N1QL言語リファレンス(N1QL Language Reference@<fn>{n1ql-language-reference})を参照してください。

//footnote[n1ql-language-reference][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/index.html]

== 基本構造

N1QLは、JSONデータと同じ表現を（直接的に、つまり文字列としてエスケープされた形ではなく）含むことができます。

=== コンストラクションオペレーター

コンストラクションオペレーターと呼ばれる次の記号をクエリ内で直接用いることができます。

//blankline

@<tt>{{} @<tt>{\}} @<tt>{:} @<tt>{[} @<tt>{]}

=== ネステッドオペレーター

ネステッドオペレーターと呼ばれる以下の表現を利用することができます。

//blankline

 * @<strong>{ドットノーテーション}: ネストされたオブジェクト（サブドキュメント）へのパスをドット記号（@<tt>{.}）を使って表現できます。

 * @<strong>{配列表現}: 配列へのアクセスに、インデックス（添字）や、スライス（範囲による指定）を利用することができます。

=== エスケープ文字

N1QLクエリ中に現れるバケット名などのユーザー定義語句をバッククォート(@<tt>{`})を使ってエスケープすることによって、予約語との衝突を回避することができます。

N1QLでは、「@<tt>{-}」は、予約語に含まれるため、「@<tt>{-}」を含むユーザー定義語句を利用する場合、エスケープする必要があります。


=== クエリコンテキスト

N1QLでスコープとコレクションを活用するために、@<strong>{クエリコンテキスト}を用いることができます。

これにより、SQL/RDB経験者にとって、より馴染みやすいクエリ表現を用いることができます。
「コンテキスト」という用語自体、SQL/RDB経験者にとってよく親しまれているものだと思いますが、N1QLでも同じ意味合いで使われています。

まず、コンテキストを使用しないクエリについて見てみます。

//emlist{
SELECT * FROM `travel-sample`.inventory.airline LIMIT 1;
//}


このクエリでは、@<tt>{travel-sample}バケットの@<tt>{inventory}スコープにある@<tt>{airline}コレクションからデータを取得しています。N1QLでは、このような表現が可能です。

一方、SQL/RDB経験者が通常、予期するクエリは、以下のようなものでしょう。

//emlist{
SELECT * FROM airline LIMIT 1;
//}

クエリコンテキストに、@<tt>{travel-sample}バケットの@<tt>{inventory}スコープを設定することによって、上記の表現が成立します。

Couchbase Serverのクエリ操作用のコマンドラインインターフェースでは、下記のように設定することができます。

//cmd{
cbq> \SET -query_context   "`travel-sample`.inventory";
//}

Webコンソールのクエリワークベンチでクエリを実行する場合には、リストボックスからバケット名とスコープ名を選択して、クエリコンテキストを設定することができます。

SDKでは、@<tt>{Scope}クラスの@<tt>{query}メソッドを利用します。また、@<tt>{Bucket}クラスの@<tt>{query}メソッドを用いて、コンテキストを使用しないクエリの実行も可能です。

Javaのサンプルコードを示します。

//emlist{
Bucket bucket = cluster.bucket("travel-sample");
Scope scope = bucket.scope("inventory");

QueryResult result = 
  scope.query("SELECT * FROM airline WHERE country = $country",
    queryOptions().parameters(JsonObject.create().put("country", "France")));
//}

以下で用いるN1QLクエリのサンプルでは、クエリコンテキストを指定しない表現を用いています。


== ドキュメントキーの利用

ドキュメントキーの利用は、標準SQLには見られない、Couchbase ServerのN1QLに固有の特徴です。

Couchbase Serverでは、ドキュメントはドキュメントキーによって一意であるため、利用できる場合には、ドキュメントキーを使うことが最も効果的です。

====[column]ドキュメントキーが利用できる場合のアプローチ

ドキュメントキーが利用できる場合、必ずしもN1QLクエリを用いて(Queryサービスを介して)ドキュメントへアクセスする必要はなく、Dataサービスに対して直接リクエストすることで性能を最適化することが可能です。
一方、サブクエリを伴うような複雑なクエリの場合などのように、クエリの中でドキュメントキーを効果的に利用できる場面があります。

====[/column]

=== USE KEYS

下記のように、@<tt>{USE KEYS}句に、ドキュメントキーを指定して、クエリを実行することができます。

//emlist{
SELECT *
FROM `travel-sample`.inventory.airport
USE KEYS "airport_1254";
//}

ドキュメントキーは、配列として複数同時に与えることもできます。

//emlist{
SELECT *
FROM `travel-sample`.inventory.airport
USE KEYS ["airport_1254","airport_1255"];
//}

@<tt>{USE KEYS}のみを利用してクエリを実行する場合、インデックスを作成する必要はありません。



=== META().id

上記の@<tt>{USE KEYS}を利用したクエリは、下記のように @<tt>{META().id}を使って書き直すことができます。

//emlist{
SELECT *
FROM `travel-sample`.inventory.airport
WHERE META().id IN ["airport_1254","airport_1255"];
//}

@<tt>{META().id}を利用してクエリを行う場合、@<tt>{META().id}を指定したインデックスが作成されている必要があります。

@<tt>{USE KEYS}の構文では、ドキュメントキーの完全一致が必須ですが、@<tt>{META().id} を利用する場合、WHERE句の条件の一部として用いることができます。

たとえば、下記のように@<tt>{LIKE}と組み合わせて部分一致検索を行うことができます。

//emlist{
SELECT * FROM MyFirstBucket WHERE META().id LIKE "ABC:%";
//}


また、検索条件としてのみではなく、次のように、条件に一致するドキュメントキーを検索するために用いることもできます。

//emlist{
SELECT META().id FROM ... WHERE ...;
//}

このように、@<tt>{META().id}は構文として成立している限り自由に使うことができるため、N1QL固有の@<tt>{USE KEYS}句よりも自由度が高いといえます。
@<tt>{USE KEYS}句では必要ない、インデックスの作成が必要になるという違いも、ここから自然に理解されるのではないでしょうか。

なお、@<tt>{META()} という表現に含まれる括弧は、下記のように、キースペース(または、その別名)を含んだ表現を用いるために存在します。
ただし、これまで紹介した例のように、クエリの中でキースペースが一意に決まっている場合には、省略することができます。
複数のキースペースが混在しているクエリの中で用いる場合には、キースペースを指定した表現を行います。

//emlist{
SELECT *
FROM `travel-sample`.inventory.airport a
WHERE META(a).id IN ["airport_1254","airport_1255"];
//}


== フィールドから値のみを取り出す

=== RAW | ELEMENT | VALUE

@<tt>{RAW}、@<tt>{ELEMENT}、@<tt>{VALUE}、これらは全て同義語です（以下では、@<tt>{RAW}を用います）。

N1QLの実行結果は通常、フィールド名とその値を含むJSONデータですが、@<tt>{RAW} キーワード用いることで、値のみを取り出すことができます。

比較のため、まず@<tt>{RAW} を用いない場合を見てみます。

//emlist{
SELECT city
FROM `travel-sample`.inventory.airport
ORDER BY city LIMIT 5;
//}

上記クエリの結果は、以下のように名前と値のペアからなるJSONデータの配列になります。

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

結果は、値のみの配列になります。

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


== 複数の値をまとめて扱う

SQL経験者にとって、@<tt>{IN}句を使って、複数の値を指定することは、目新しいものではないでしょう。
ここでは、N1QLにおける@<tt>{IN}と@<tt>{WITHIN}について解説します。

=== IN

N1QLにおける@<tt>{IN}句は、SQLの@<tt>{IN}句に近い働きをします。
@<tt>{IN}句は、配列のようなフラットなデータシーケンスを対象とします。

//emlist{
SELECT * FROM `travel-sample`.inventory.airline AS t
WHERE country IN ["United Kingdom", "France"];
//}

=== WITHIN

N1QLでは、検索条件に複数の値を指定する際に、@<tt>{WITHIN}句を使って、JSONデータのネストした構造を対象とすることができます。

JSONデータに対する条件指定において、@<tt>{IN}は、トップレベルの要素を対象とするのに対して、@<tt>{WITHIN}は、その子、および子孫を含めて対象とします。

具体的に見ていきます。
たとえば、以下のクエリでは、@<tt>{WITHIN}句に(別名を介して)@<tt>{hotel}コレクションを指定しています。

//emlist{
SELECT * FROM `travel-sample`.inventory.hotel AS t 
WHERE "Walton Wolf" WITHIN t;
//}

以下は結果の例です。

//emlist{
[
  {
    "t": {
      "id": 10851,
      "name": "Tantallon House B&B",
      "address": "Gilsland, CA8 7DA",
      "country": "United Kingdom",
      "reviews": [
        {
          "author": "Walton Wolf",
          "content": "Myself and a mate stayed here the last week of April 2010. ...",
          "date": "2014-06-07 03:54:50 +0300",
          "ratings": {
            "Cleanliness": 4,
            "Location": 4,
            "Overall": 4,
            "Rooms": 2,
            "Service": 3,
            "Sleep Quality": 4,
            "Value": 3
          }
        },
        {
          "author": "Catharine Funk",
          "content": "My first trip to new york city was a disaster because of this hotel. ...",
          "date": "2012-01-02 21:29:27 +0300",
          "ratings": {
            ...
          }
        },
        {
          ...
        }
      ],
      ...
    }
  }
]
//}

検索結果として、ひとつのドキュメントが該当しています。トップレベルの@<tt>{reviews}フィールドは、値を配列として持っており、その要素はオブジェクトです。
配列の先頭要素のオブジェクトの@<tt>{author}フィールドの値が、検索条件に用いられた@<tt>{"Walton Wolf"}であることがわかります。


== 配列へのアクセス

N1QLで配列へのアクセスに用いられる構文は、@<tt>{ARRAY}または @<tt>{FIRST}から始まり、@<tt>{END}で終わります。

下記に基本構文を示します(簡単な構文からはじめ、徐々に複雑な構文を紹介していきます)。

この構文を利用して、配列の要素に対して条件指定を行って要素を抽出したり、配列が要素として値ではなくオブジェクトを持っているとき、そのオブジェクトのフィールドを配列として取り出すことができます。

//emlist{
( ARRAY | FIRST ) var1 FOR var1 ( IN | WITHIN ) expr1 END
//}

=== ARRAY

配列の要素全体を扱います（プログラミング言語の文脈でいうところの、シーケンスに対するmap操作にあたります）。

=== FIRST

@<tt>{ARRAY} を @<tt>{FIRST}に置き換えることで、配列のはじめの要素のみを扱うことになります。


=== WHEN

下記構文のように、@<tt>{WHEN} 句を使って、条件指定を付け加えることが可能です(配列の要素に対してfilter操作を実行します)。

//emlist{
( ARRAY | FIRST ) var1 FOR var1 ( IN | WITHIN ) expr1 [ ( WHEN cond1 [ AND cond2 ] ) ] END
//}



以下にクエリとその結果のサンプルを示します(構文の理解を容易にするため、クエリ内でデータを定義しています)。

//emlist{
SELECT ARRAY v FOR v IN [1, 2, 3, 4, 5] WHEN v > 2 END AS res;
//}

//emlist{
[
  {
    "res": [
      3,
      4,
      5
    ]
  }
]
//}

下記のように、配列要素がオブジェクトの場合、フィールドに対して条件指定を行ったり、特定のフィールドを取り出すことができます。

//emlist{
SELECT ARRAY v.flight FOR v IN schedule 
WHEN v.utc > "19:00" AND v.day = 5 END AS fri_evening_flights
FROM `travel-sample`.inventory.route
LIMIT 5;
//}

最後に、これまで説明した構文を踏まえ、配列を複数(@<tt>{var1}、@<tt>{var2})指定した、下記のような表現を用いることができます。

//emlist{
( ARRAY | FIRST ) var1 FOR var1 ( IN | WITHIN ) expr1 [ ,var2 ( IN | WITHIN ) expr2 ]* [ ( WHEN cond1 [ AND cond2 ] ) ] END
//}

== 配列を検索条件に利用

配列を検索条件の中で用いる際の構文は、@<tt>{ANY}または @<tt>{EVERY} から始まり、@<tt>{END}で終わります。@<tt>{SATISFIES} で条件を指定します。

//emlist{
( ANY | EVERY ) var1 ( IN | WITHIN ) expr1 [ , var2 ( IN | WITHIN ) expr2 ]* SATISFIES condition END
//}

条件が真となった際に、その配列を持つドキュメントが、検索結果に含まれます。

=== ANY

配列に条件で指定した要素がひとつでも含まれる場合、真となります。

下記に利用例を示します。

//emlist{
SELECT *
FROM retail.east.order o
WHERE ANY item IN o.lineItems SATISFIES
item.count >= 5 END;
//}

=== EVERY

配列の全ての要素が指定した条件に一致する場合、真となります。

下記は、上記例のクエリの@<tt>{ANY}を@<tt>{EVERY}に変更しただけですが、条件に一致するドキュメントの範囲が狭まる(可能性が高い)ことは理解しやすいと思います。

//emlist{
SELECT *
FROM retail.east.order o
WHERE EVERY item IN o.lineItems SATISFIES
item.count >= 5 END;
//}



== サブドキュメントの階層操作

=== NEST

@<tt>{NEST}は、外部のドキュメントを、子ドキュメントとして親ドキュメントの中に(サブドキュメントとして)埋め込みます。
結合の特別なタイプであるといった方が、理解しやすいかもしれません(ここで登場する、@<tt>{ON KEYS}句については、@<tt>{NEST}を使わない結合でも利用します)。

具体的な利用法として、関連のあるデータをドキュメントキーを介した参照関係を用いて、複数のドキュメントとしてモデル化した際に活用する事ができます。
それらのドキュメントを結合するために、@<tt>{ON KEYS}を用いて、結合対象とするドキュメントのキーを指定します。


以下の２種類のデータに対するクエリについて考えてみます。
リレーショナルモデルの解説では、よく知られた例である「受注伝票」と「受注明細」データが表現されています。

まず、「受注伝票」を表すデータ(@<tt>{retail.east.order}コレクション)のサンプルを示します。

//emlist{
{
"order_id":1234,
"customer_id":"34567",
"total_price":"65.5",
"lineitems":["o11","o12"]
}
//}

次に、「受注明細」を表すデータ(@<tt>{retail.east.lineItem}コレクション)のサンプルを示します。
ドキュメントキーは、@<tt>{lineitem_id}の値と一致していると考えてください。

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

下記に、これらのデータを結合するクエリを示します。

//emlist{
SELECT ordr.order_id,
ARRAY {“item_id”: l.item_id, “qty”:l.qty} FOR l IN line END AS items
FROM retailsample.east.order ordr
NEST retailsample.east.lineItem line
ON KEYS ordr.lineitems;
//}

結果は、以下のようになります。

//emlist{
[
{
"items":[
    {"item_id":"789", "qty":"3"},
    {"item_id":"234", "qty":"5"}
  ],
  "order_id":"1234"
}
]
//}

=== UNNEST

@<tt>{UNNEST}は、ネストされたオブジェクト(サブドキュメント)をドキュメントのトップレベルに表示するために用います。

下記のようなネストされた構造を持つデータを想定します。

先ほどとは逆に、受注伝票データが受注明細データを含む構造になっています。

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

@<tt>{UNNEST}は、このようなネストされた（サブドキュメントを含む）ドキュメントへのクエリの結果を下記のようなテーブル構造として利用したいときに使われます。

//table{
ordId	status	prodId	qty
----------------------------
ORDER-0001	Shipped	AAA-222	1
ORDER-0001	Shipped	BBB-333	2
ORDER-0001	Shipped	CCC-444	3
//}


下記のように、サブドキュメントに対して@<tt>{UNNEST}を指定し、@<tt>{AS}で別名をつけたものを@<tt>{SELECT}句の中で使用します。

//emlist{
SELECT ord.ordId, ord.status, item.* 
FROM retail.east.order ord 
UNNEST items AS item;
//}

これにより、下記のようなフラットな構造のJSONが取り出されます。

//emlist{
[
{ “ordId”: “ORDER-0001”, “status”: “Shipped”, “prodId”: “AAA-222”, “qty”: 1 },
{ “ordId”: “ORDER-0001”, “status”: “Shipped”, “prodId”: “BBB-333”, “qty”: 2 },
{ “ordId”: “ORDER-0001”, “status”: “Shipped”, “prodId”: “CCC-444”, “qty”: 3 }
]
//}

なお、上記のデータは、先述のテーブル形式データとの比較のために整形しています。
実際には、フィールドの出現順はこの通りではないことにご注意ください。JSONオブジェクトのフィールドはその登場順序に意味を持ちません。


== データ型

ここでは、Couchbase ServerのN1QL固有のデータ型(Data Types@<fn>{datatypes})について理解するために、標準SQLに含まれる関連性の高いデータ型とあわせて解説します。

SQL利用者にとって、「NULL」という予約語や、@<tt>{IS [NOT] NULL}という比較演算は、馴染みのあるものでしょう。
NULLは、データが存在しないことである、といったりします。より正確には、値が存在しない状態を表しています。ここで、データと値の違いはどこにあり、なぜ後者がより正確なのでしょうか？「データ」という表現は曖昧に取れる一方、「値」という言葉には、数学でいうところのXの値という使い方に見られるように（もちろん、プログラミングでいうところの、変数の値、でもいいわけですが）、単にそれ自体で存在しているデータを超えた含意があります。
リレーショナルデータベースでは、カラムは一定のデータ型を用いて定義されますが、数値型であれば0、文字列型であれば空文字（長さ0の文字）のようなデータ型固有の表現とは別に、値そのものが存在していない状態がNULLで表現されます。
JSONにおいても、「フィールドの値が未定義」であることを意味する「null」という予約語が定義されています。一方、JSONのように、データ自体にデータ構造の情報が含まれる（データとは別にスキーマが存在していない）データにあっては、「フィールドが未定義」のケースを考慮する必要があります。

JSONデータに対して、N1QLによるクエリを実行した場合、他のドキュメントには存在するフィールドが、一部のドキュメントには存在しないことがありえます（RDB/SQLでは、テーブル内のすべてのレコードが同一のスキーマに従うため、このような状況が起こりません）。
N1QLには、そのようなギャップを埋めるため、@<tt>{MISSING}というキーワード（データ型）が用意されています。

以下、N1QLにおけるデータ型としての、@<tt>{NULL}と@<tt>{MISSING}の定義を確認した後に、N1QLにおける、存在しない値に対する比較演算について見ていきます。


=== MISSING


N1QLにおいて@<tt>{MISSING}は、JSONドキュメントの中にそのフィールド（名前と値のペア）が存在していない（欠落している）ことを表します。


=== NULL


N1QLは、キーワード@<tt>{NULL}を使用して空の値を表します。データ挿入、更新時に、フィールドの値をNULLにすることができます。

なお、N1QLのNULLでは大文字と小文字が区別されません。たとえば、@<tt>{null}、@<tt>{NULL}、@<tt>{Null}、および@<tt>{nUll}はすべて同等です。


//footnote[datatypes][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/datatypes.html#datatypes]

== 比較演算

ここでは、Couchbase ServerのN1QL固有の比較演算子(Comparison Operators@<fn>{comparisonops})について見ていきます。

@<tt>{IS [NOT] NULL | MISSING | VALUED}演算子を使用すると、データセット内の値や属性の存在（または不在）に基づいて条件を指定できます。

@<tt>{IS [NOT] NULL}と@<tt>{IS [NOT] MISSING}は、データ型の定義から想像される通りの挙動となります。
具体的なクエリと結果の例を見るのが、理解に役立つでしょう。


=== IS NULL

//emlist{
SELECT fname, children
FROM tutorial.sample.person
WHERE children IS NULL;
//}

//emlist{
[
  {
    "children": null,
    "fname": "Fred"
  }
]
//}

=== IS MISSING

//emlist{
SELECT fname, children
FROM tutorial.sample.person
WHERE children IS MISSING;
//}

//emlist{
[
  {
    "fname": "Harry"
  },
  {
    "fname": "Jane"
  }
]
//}

=== IS VALUED

@<tt>{NULL}と@<tt>{MISSING}は、JSONデータ上明らかに異なっており、区別する方法が必要である一方、アプリケーションにおける検索条件としては、これらを区別する必要がない場合が考えられます。その際に@<tt>{AND}や@<tt>{OR}を使って複数の検索条件を並置する代わりに、N1QLでは@<tt>{IS [NOT] VALUED}という比較演算が利用可能です。

//blankline

 * @<tt>{IS VALUED}は、値が@<tt>{MISSING}でも@<tt>{NULL}でもない場合、真になります。
 * @<tt>{IS NOT VALUED}は、値が@<tt>{MISSING}あるいは@<tt>{NULL}である場合、真になります。



//footnote[comparisonops][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/comparisonops.html]




== 結合(JOIN)

N1QLで利用することのできる、以下の3つの結合(JOIN@<fn>{63c6596bad48de18597b283a703bfc2f})のタイプについて解説します。

//blankline

 * ANSI JOIN
 * ルックアップ(Lookup)JOIN
 * インデックス(Index)JOIN

//blankline

ANSI JOIN以外のふたつの種類のJOINでは、N1QL独自のキーワードが用いられます。
それらは、@<tt>{ON KEYS}と @<tt>{ON KEY ... FOR} です。
これらは、JSONドキュメントのドキュメントキーによる結合をサポートするために用意されています。

ここでは、以下のデータモデルを用いて、例示による整理を行います。

//blankline

 * @<tt>{airline}(航空会社)と@<tt>{route}(経路)という、ふたつの種類のデータモデルがある
 * @<tt>{route}データモデルは、@<tt>{airline}への参照を含む。参照の値は@<tt>{airline}ドキュメントのキーと一致している。

=== ANSI JOIN

ANSI JOINは、N1QL独自のキーワードを用いない、SQLの一般的なJOINです。
ANSI JOINでドキュメントキーを用いた結合を行うために、@<tt>{META().id}を用いることができます。この場合、@<tt>{META().id}を含む適切なインデックスが作成されている必要があります。 

以下に、基本構文を示します。

//emlist{
lhs-expr
JOIN rhs-keyspace
ON any join condition
//}


下記は、@<tt>{META().id}を用いたANSI JOINの例です。

//emlist{
SELECT *
FROM `travel-sample`.inventory.route r
JOIN `travel-sample`.inventory.airline a
ON r.airlineid = META(a).id;
//}

====[column]N1QLのJOINと標準SQLとの違い

N1QLのANSI JOINには、SQL標準と比べて、以下のような制約があります。

//blankline

 * @<tt>{FULL [OUTER] JOIN} はサポートされていません。
 * @<tt>{CROSS JOIN} はサポートされていません。
 * @<tt>{RIGHT [OUTER] JOIN} は、JOIN連結の最初(または、それのみ)である必要があります。

====[/column]

=== ルックアップJOIN

ルックアップJOINでは、@<tt>{ON KEYS}句を用います(複数形の@<tt>{KEYS}です)。ドキュメントキーを用いた結合を行うために、インデックスを利用する必要はありません。 

以下に、基本構文を示します。

//emlist{
lhs-expr
JOIN rhs-keyspace
ON KEYS lhs-expr.foreign-key
//}

上述のANSI JOINの例と、同内容のクエリを下記のように表現することができます。

//emlist{
SELECT *
FROM `travel-sample`.inventory.route r
JOIN `travel-sample`.inventory.airline
ON KEYS r.airlineid;
//}

=== インデックスJOIN

@<tt>{ON KEY ... FOR}句を用います(単数形の@<tt>{KEY}です)。
インデックスJOINでは、適切なインデックスが作成されている必要があります。

以下に、基本構文を示します。

//emlist{
lhs-keyspace
JOIN rhs-keyspace
ON KEY rhs-keyspace.idx_key
FOR lhs-keyspace
//}


下記のクエリ例では、ルックアップJOINのケースと@<tt>{FROM ... JOIN ...}で指定されているキースペース（コレクション）の順序が異なっていることに注目してください。 

//emlist{
SELECT *
FROM `travel-sample`.inventory.airline a
JOIN `travel-sample`.inventory.route r
ON KEY r.airlineid
FOR a;
//}

上記のクエリを実行するためには、下記のようなインデックスが必要です。

//emlist{
CREATE INDEX route_airlineid ON `travel-sample`.inventory.route(airlineid);
//}

インデックスJOINを使用すると、結合句の方向を反転できます。
キーによる結合以外の条件指定のため、ルックアップJOINが利用できない場合、インデックスJOINが使用できます。

サンプルを示した解説は、データモデルの説明を含めて、長々としたものになってしまうため、ドキュメント@<fn>{join-examples-4}の記述に譲ります。

//footnote[63c6596bad48de18597b283a703bfc2f][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/join.html]

//footnote[join-examples-4][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/join.html#examples-4]




== ユーザー定義関数

Couchbase ServerのN1QLでは、ユーザーが関数を定義して、組み込み関数と同じように、任意の式で呼び出すことができます。

====[column]エディションによる差異

ユーザー定義関数は、エンタープライズエディションでのみ利用することができます。

====[/column]

ここでは、ユーザー定義関数を用いて、何が実現できるかを把握するための情報を提供します。
実際に利用するために必要となる、定義方法や呼び出し方を含めた、具体的な詳細には触れませんが、必要に応じて、ドキュメント@<fn>{4fec4dc4c082b58a3a0c563887d0169d}を参照してください。

=== ユーザー定義関数の種類

ユーザー定義関数は@<tt>{CREATE FUNCTION}@<fn>{9691b9034b80d65250e6f45a580041f0}ステートメントを用いて定義します。

定義の仕方の異なる次のふたつの種類のユーザー定義関数があります。

//blankline

 * @<strong>{インライン関数}は、N1QLクエリを使用して定義されます。複雑な式や、何度も用いられる式に名前を付けて定義し、再利用することができます。
 * @<strong>{外部関数}はN1QLではなく、プログラミング言語を使用して定義されます。この機能により、N1QL式を使用して定義するのが困難または不可能な関数を作成することができます。構文上は、他の言語の利用も想定される書式が用いられていますが、現時点でサポートされているプログラミング言語はJavaScriptのみです。

=== 関数呼び出しの種類

ユーザー定義関数には、次のふたつの呼び出し方法があります。

//blankline

 * N1QLの任意の式中で、組み込み関数と同じように呼び出すことができます。ただし、Couchbase Serverのユーザー定義関数は単純なインライン関数を超えた高い自由度を持ち、関数の中でドキュメントの変更などを行うことができます。その反面、このような「副作用」を持つユーザー定義関数をN1QL式の中から呼び出した場合には、エラーが発生することに注意が必要です。
 * @<tt>{EXECUTE FUNCTION}@<fn>{973a71b323540fcd67ec61262da9d1de}ステートメントを使って、ユーザー定義関数を単体で直接実行することが可能です。これは、ユーザー定義関数をテストする際に用いることができる他、@<tt>{EXECUTE FUNCTION}ステートメントでは副作用のある関数を実行することが可能なため、RDBMSにおけるストアドプロシージャのような使い方ができます(なお、Couchbase Serverでは、RDBMSにおけるトリガに類似する機能としてEventingサービスを利用することができます)。

====[column]エディションによる差異

Eventingサービスは、エンタープライズエディションでのみ利用することができます。

====[/column]


//footnote[4fec4dc4c082b58a3a0c563887d0169d][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/userfun.html]

//footnote[9691b9034b80d65250e6f45a580041f0][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/createfunction.html]

//footnote[973a71b323540fcd67ec61262da9d1de][https://docs.couchbase.com/server/current/n1ql/n1ql-language-reference/execfunction.html]


//blankline

====[column]エディションによる差異

クエリに関連した、コミュニティーエディションには含まれない、エンタープライズエディションの機能として、以下があります。

 * ウィンドウ関数
 * FLEXインデックス（クエリからの全文検索インデックス利用）
 * コストベースオプティマイザ（CBO）
 * 無制限のクエリ同時実行
 * N1QLアグリゲートプッシュダウン
 * N1QLリクエストの監査
 * クエリのモニタリング

====[/column]