
= Couchbase Lite SQL++/N1QLクエリAPI

Couchbase LiteのSQL++/N1QLクエリAPI@<fn>{query-n1ql-mobile}について解説します。


//footnote[query-n1ql-mobile][https://docs.couchbase.com/couchbase-lite/current/android/query-n1ql-mobile.html]


== ステートメント


=== SELECT


SELECT句は@<tt>{SELECT}キーワードで始まります。

//blankline

@<tt>{SELECT}キーワードに続き、JSONドキュメントのプロパティー名を指定します。
各プロパティーに対して、@<tt>{AS}を使用して、プロパティーにエイリアス名を指定することができます。

ワイルドカード「@<tt>{*}」を使用して、すべてのプロパティーを取得することもできます。

クエリの結果は、JSONオブジェクトの配列になります。
配列の要素オブジェクトの構成は、ワイルドカード「@<tt>{*}」を使用した場合とプロパティー名を指定した場合とで、異なります。

ワイルドカード「@<tt>{*}」を使用した場合、配列の要素ごとに、データベース名をキーとする辞書型オブジェクトで構成されます。この辞書型オブジェクトは、ドキュメントの各プロパティーが、キーと値のペアとして含まれます。

次の例を参照ください。

//emlist[][json]{
[
  {
    "hotels": { 
      "id": "hotel123",
      "type": "hotel",
      "name": "Hotel California",
	  "city": "California"
    }
  },
  {
    "hotels": { 
      "id": "hotel456",
      "type": "hotel",
      "name": "Chelsea Hotel",
	  "city": "New York"
    }
  }
]
//}

クエリでプロパティー名を指定した場合、配列の要素オブジェクトに、指定されたプロパティーが、キーと値のペアとして含まれます。

//emlist[][json]{
[
  { 
    "name": "Hotel California",
	"city": "California"
  },
  { 
    "name": "Chelsea Hotel",
	"city": "New York"
  }
]
//}


また、SQL同様、@<tt>{DISTINCT}キーワードを用いて重複した結果を削除することも可能です。



=== FROM

FROM句ではクエリの照会先となるデータソースを指定します。

Couchbase Liteにはテーブルの概念がなく、データソースはデータベース自体です。
そして、照会先のデータベースは、@<tt>{Database}オブジェクトにおいて、既に一意に定まっています。
そのため、FROM句では以下のようにアンダースコア文字(「@<tt>{_}」)を共通のデータソース表記として使用することができます。

//emlist[][SQL]{
SELECT name FROM _
//}

データソースにはエイリアスをつけることができます。以下の例を参照ください。

//emlist[][SQL]{
SELECT store.name FROM _ AS store
SELECT store.name FROM _ store
//}

@<tt>{Database}オブジェクトを作成するときに使用したデータベースの名前をデータソース名として使うこともできます。


//emlist[][SQL]{
Database database = new Database("db");
Query query = database.createQuery("SELECT * FROM db");
//}

=== WHERE

WHERE句を使用して、クエリによって返されるドキュメントの選択基準を指定できます。

ブーリアン値に評価される任意の数の式をチェーンできます。
次の例を参照ください。


//emlist[][SQL]{
SELECT name FROM _ WHERE department = 'engineer' AND division = 'mobile'
//}

WHERE句内で利用できる表現については、後掲の二項演算子についての解説を参照ください。

=== JOIN

JOIN句を用いて、指定された基準によってリンクされた複数のデータソースからデータを選択できます。

Couchbase Liteには、リレーショナルデータベースにおけるテーブルに対応する概念が存在しないため、クエリの照会先データソースはデータベースそのものです。
Couchbase Liteでは、データベースはファイルに対応しており、複数のデータベース(ファイル)間の結合はサポートされていません。@<fn>{memo-cbs-join}
そのため、Couchbase Liteにおける結合は自己結合となります。

//footnote[memo-cbs-join][Couchbase Serverでは、リレーショナルデータベースにおけるテーブルに対応するコレクションというキースペース(名前空間)が存在し、コレクション間の結合がサポートされています。]

次の5つのJOIN演算子がサポートされています。

//blankline

 * @<tt>{JOIN}
 * @<tt>{LEFT JOIN}
 * @<tt>{LEFT OUTER JOIN}
 * @<tt>{INNER JOIN}
 * @<tt>{CROSS JOIN}

//blankline

なお、@<tt>{JOIN}と@<tt>{INNER JOIN}は同義であり、@<tt>{LEFT JOIN}と@<tt>{LEFT OUTER JOIN}は同義です。


JOIN句は、JOIN演算子で始まり、その後にデータソース指定が続きます。
結合制約は、@<tt>{ON}キーワードで始まり、その後に結合制約を定義する式が続きます。

次の例を参照してください。

//emlist[][SQL]{
SELECT one.prop1, other.prop2 FROM _ AS one JOIN _ AS other ON one.key = other.key
//}


=== GROUP BY

GROUP BY句を利用すると、検索結果のデータに対してグループ化を行うことができます。
通常、集計関数（@<tt>{COUNT}、@<tt>{MAX}、@<tt>{MIN}、@<tt>{SUM}、@<tt>{AVG}など）と一緒に使用されます。

オプションとして、@<tt>{HAVING}句を使って、集計関数に基づいて結果をフィルタリングすることができます。 

次の例を参照ください。

//emlist[][SQL]{
SELECT COUNT(empno), city FROM _ GROUP BY city HAVING COUNT(empno) > 100
//}

=== ORDER BY

ORDER BY句を利用すると、クエリの結果を並べ替える(ソートする)ことができます。

オプションの@<tt>{ASC}（昇順）または@<tt>{DESC}（降順）を使用してソート順を指定します。何も指定しなかった場合のデフォルトは@<tt>{ASC}です。

次の例を参照ください。

//emlist[][SQL]{
SELECT name, score FROM _ ORDER BY name ASC, score DESC
//}

=== LIMIT

LIMIT句を利用して、クエリによって返される結果の最大数を指定することができます。

次の例は、10件の結果のみを返します。


//emlist[][SQL]{
SELECT name FROM _ ORDER BY name LIMIT 10 
//}

=== OFFSET


OFFSET句を利用して、クエリの結果をスキップする数を指定することができます。

次の例は、最初の10件の結果を無視して、残りの結果を返します。

//emlist[][SQL]{
SELECT name FROM _ ORDER BY name OFFSET 10 
//}

次の例は、最初の10件の結果を無視してから、次の10件の結果を返します。

//emlist[][SQL]{
SELECT name FROM _ ORDER BY name LIMIT 10 OFFSET 10 
//}


== リテラル


次のようなリテラル表現を用いることができます。


=== 数値


下の例のような表現で数値を表すことができます。


//emlist[][SQL]{
SELECT * FROM _ WHERE number = 10
SELECT * FROM _ WHERE number = 0
SELECT * FROM _ WHERE number = -10
SELECT * FROM _ WHERE number = 10.25
SELECT * FROM _ WHERE number = 10.25e2
SELECT * FROM _ WHERE number = 10.25E2
SELECT * FROM _ WHERE number = 10.25E+2
SELECT * FROM _ WHERE number = 10.25E-2
//}

=== 文字列リテラル

クォートまたはダブルクォートを用いて、文字列を表現します。

//emlist[][SQL]{
SELECT * FROM _ WHERE middleName = "Fitzgerald"
SELECT * FROM _ WHERE middleName = 'Fitzgerald'
//}

=== 真偽値

Oracle、MySQL、PostgreSQL同様(SQL ServerやSQLiteとは異なり)、ブーリアン型を扱うことができます。

//emlist[][SQL]{
SELECT * FROM _ WHERE value = true
SELECT * FROM _ WHERE value = false
//}

=== NULL


NULLは、空の値を表します。



JSON仕様には、空の値の表現として、@<tt>{null}があり、以下のようなデータ表現が可能です。

//emlist[][]{
{
  "firstName": "John",
  "middleName": null,
  "lastName": "Kennedy"
}
//}

下記のクエリの検索条件は、上記のデータと一致します。

//emlist[][SQL]{
SELECT firstName, lastName FROM _ WHERE middleName IS NULL
//}

=== MISSING


MISSINGは、ドキュメントに定義されていない(欠落している)名前と値のペアを表します。

たとえば、以下のようなデータがあるとします。

//emlist[][]{
{ 
  "firstName": "John",
  "lastName": "Kennedy"
}
//}

下記のクエリの検索条件は、上記のデータと一致します。

//emlist[][SQL]{
SELECT firstName, lastName FROM _ WHERE middleName IS MISSING
//}

=== 配列

JSON同様の表現で配列を表します。

//emlist[][SQL]{
SELECT ["a", "b", "c"] FROM _

SELECT [ property1, property2, property3] FROM _
//}

=== オブジェクト

JSON同様の表現で辞書型のオブジェクトを表します。

//emlist[][SQL]{
SELECT { 'name': 'James', 'department': 10, 'phones': ['650-100-1000', '650-100-2000'] } FROM _
//}

== 識別子

クエリ内のデータベース名、プロパティー名、エイリアス名、パラメータ名、関数名、インデックス名のような識別子には、以下のような文字を利用することができます。

//blankline

 * アルファベット(@<tt>{a}-@<tt>{z}、@<tt>{A}-@<tt>{Z})
 * 数字(@<tt>{0}-@<tt>{9})
 * @<tt>{_}（アンダースコア）

//blankline

アルファベットでは、大文字と小文字が区別されます。

クエリの識別子として利用可能な文字と、JSONのプロパティー名などに利用することができる文字の範囲は異なります。
上記以外の文字（例えばハイフン「@<tt>{-}」）をクエリ内で使用する場合には、バッククォート(@<tt>{`})で囲みます。
以下の例を参照ください。


//emlist[][SQL]{
SELECT `first-name` FROM _
//}

== 式

=== プロパティー式

ドキュメントのプロパティーを参照するためにプロパティー式が使用されます。以下のような使い方があります。

//blankline

 * ドット構文を使用して、ネストされたプロパティーを参照します。
 * 角括弧（@<tt>{[} @<tt>{]}）を使った添え字構文を使用して、配列内の項目を参照します。
 * アスタリスク(@<tt>{*})を、SELECT句の結果リストでのみ、すべてのプロパティーを表すために使用できます。
 * プロパティー式の前にデータソース名またはそのエイリアス名を付けて、その起源を示します（クエリ内でひとつのデータソースのみが利用されている場合は省略可能です）。

//blankline

以下に例を示します。

//emlist[][SQL]{
SELECT contact.address.city, contact.phones[0] FROM _

SELECT directory.* FROM _ AS directory
//}

=== 配列式

配列内のアイテムを評価するための独自の構文があります。

以下に例を示します。


//emlist[][SQL]{
SELECT name
  FROM _
  WHERE ANY v
          IN contacts
          SATISFIES v.city = 'San Mateo'
        END
//}

配列式は、@<tt>{ANY/SOME}、@<tt>{EVERY}、またはそれらの組み合わせで始まります。それぞれが以下に説明するように異なる機能を持ちます。

//blankline

 * @<tt>{ANY/SOME}: 配列内の少なくともひとつの項目が式を満たしている場合にTRUEを返します。そうでない場合は、FALSEを返します。@<tt>{ANY}と@<tt>{SOME}は同義です。
 * @<tt>{EVERY}: 配列内のすべての項目が式を満たしている場合はTRUEを返します。そうでない場合は。FALSEを返します。配列が空の場合は、TRUEを返します。
 * @<tt>{ANY/SOME AND EVERY}：配列が値を持つ場合は、@<tt>{EVERY}と同様ですが、配列が空の場合にFALSEを返します。

//blankline

上記キーワードのいずかに続き、評価される配列内の各要素、または要素オブジェクトのプロパティーを指定します。
配列の要素を表現するために上記例の@<tt>{v}のように任意の変数を用いることができます。

@<tt>{IN}キーワードに続いて評価する配列型のプロパティー名を指定します。

@<tt>{SATISFIES}キーワードに続いて、配列内の各要素を評価するための条件を指定します。

配列式は、@<tt>{END}キーワードで終了します。 


=== パラメータ式


パラメータ化クエリを利用することができます。

検索条件として用いられる値のようにクエリの実行毎に内容が異なる箇所にパラメータを用いることによって、SQLインジェクション対策やパフォーマンス改善の効果を得ることができます。

クエリ文字列内でパラメータを指定するには、名前の前に、@<tt>{$}を付けます。

このパラメータに対して、クエリ実行時に指定するパラメータマップから値が割り当てられます。

以下に、使用例を示します。


//emlist[][java:Java]{
Query query = database.createQuery("SELECT * FROM _ WHERE type = $type");
query.setParameters(new Parameters().setString("type", "hotel"));
ResultSet rs = query.execute();
//}


=== 括弧式

式をグループ化して読みやすくしたり、演算子の優先順位を設定するために、丸括弧を使用できます。

以下に、使用例を示します。

//emlist[][SQL]{
SELECT (n1 + n3) * r AS n FROM _ WHERE (n1 = n2) AND (n3 = n4) 
//}


== 二項演算子

=== 算術演算子

算術演算子として、以下がサポートされています。

//blankline

@<tt>{+}(加算)  @<tt>{-}(減算)  @<tt>{*}(乗算)  @<tt>{/}(除算)  @<tt>{%}(剰余)

//blankline

@<tt>{/}(除算)については、両方のオペランドが整数の場合は整数除算が使用され、一方が浮動小数点の場合は浮動小数点除算が使用されます。算術関数@<tt>{DIV}では、常に浮動小数点除算が実行されます。

=== 比較演算子

比較演算子として、以下がサポートされています。

//blankline

@<tt>{=}(または @<tt>{==})  @<tt>{<>}(また @<tt>{!=})  @<tt>{>}  @<tt>{>=}  @<tt>{<}  @<tt>{<=}

=== IN

左辺の値が右辺に指定された式に含まれるかどうかを評価します。

//emlist[][SQL]{
SELECT name FROM _ WHERE department IN ('engineering', 'sales')
//}


=== LIKE


@<tt>{LIKE}を用いて曖昧検索を行うことができます。

SQL同様、以下の二種類の照合が可能です。

//blankline

 * ワイルドカードマッチ(@<tt>{%}): 0個以上の文字に一致します。
 * キャラクターマッチ(@<tt>{_}): 1文字に一致します。

//blankline

//emlist{
SELECT name FROM _ WHERE name LIKE 'art%'
SELECT name FROM _ WHERE name LIKE 'a__'
//}


@<tt>{LIKE}式によるマッチングは、ASCII文字では大文字と小文字を区別せず、非ASCII文字では大文字と小文字を区別します。


=== BETWEEN


@<tt>{BETWEEN}を用いて、値の範囲を指定することができます。


//emlist{
SELECT * FROM _ WHERE n BETWEEN 10 and 100
//}


上の式は、下の式と同等です。

//emlist{
SELECT * FROM _ WHERE n >= 10 AND n <= 100
//}


=== IS (NOT) NULL | MISSING | VALUED

SQLでは、NULL値を持つ項目を検索条件に利用する場合は、以下のような表現を行います。
N1QLでも、同じ表現が利用可能です。

//emlist{
SELECT * FROM _ WHERE p IS NULL
SELECT * FROM _ WHERE p IS NOT NULL
//}

N1QLでは加えて、以下のような検索条件を指定することができます。

//blankline

 * @<tt>{IS MISSING}: MISSINGに等しい(プロパティーが定義されていない)
 * @<tt>{IS NOT MISSING}: MISSINGに等しくない(プロパティーが定義されている)
 * @<tt>{IS VALUED}: NULLでも、MISSINGでもない(プロパティーが定義されており、@<tt>{null}以外の値を持つ)
 * @<tt>{IS NOT VALUED}: NULLまたは、MISSINGのいずれか(プロパティーが定義されていないか、定義されていても値が@<tt>{null}である)


=== 論理演算子の論理規則

論理演算子は、ブーリアンとしての評価において、次の論理規則を使用して式を結合します。

//blankline

 * TRUEはTRUE、FALSEはFALSE
 * 数字0または0.0はFALSE
 * 配列とオブジェクトはFALSE
 * 文字列は、値が0または0.0としてキャストされる場合はFALSE、それ以外はTRUE、
 * NULLはFALSE
 * MISSINGはMISSING

//blankline

@<tt>{TOBOOLEAN}関数を用いる評価では、以下のような違いがあります。

//blankline

 * MISSING、NULLおよびFALSEはFALSE
 * 数字0はFALSE
 * 空の文字列、配列、およびオブジェクトはFALSE
 * 他のすべての値はTRUE

//blankline

@<tt>{TOBOOLEAN}関数では、Couchbase ServerのN1QLと同様の基準で値を変換することができます。

また、以下の論理演算子@<tt>{AND}、@<tt>{OR}の評価においても、Couchbase ServerのN1QLとは異なる部分があります。詳細はドキュメントを参照ください。


=== AND

オペランドの両方がTRUEと評価された場合、TRUEを返します。それ以外の場合はFALSEを返します。

以下に、利用例を示します。

//emlist{
SELECT * FROM _ WHERE city = "Paris" AND state = "Texas"
//}

ただし、以下の例外があります。

//blankline

 * 一方のオペランドがMISSINGで、もう一方がTRUEの場合はMISSINGを返し、もう一方のオペランドがFALSEの場合はFALSEを返します。
 * 一方のオペランドがNULLで、もう一方がTRUEの場合はNULLを返し、もう一方のオペランドがFALSEの場合はFALSEを返します。

=== OR

オペランドのひとつがTRUEと評価された場合、TRUEを返します。それ以外の場合はFALSEを返します。

以下に、利用例を示します。

//emlist{
SELECT * FROM _ WHERE city = "San Francisco" OR city = "Santa Clara"
//}

ただし以下の例外があります。

//blankline

 * 一方のオペランドがMISSINGの場合、他のオペランドがFALSEの場合はMISSINGになり、他のオペランドがTRUEの場合はTRUEになります。
 * 一方のオペランドがNULLの場合、他のオペランドがFALSEの場合はNULLになり、他のオペランドがTRUEの場合はTRUEになります。


=== 文字列演算子


文字列の連結のために@<tt>{||}を利用することができます。

以下に、利用例を示します。

//emlist{
SELECT firstName || lastName AS fullName FROM _
//}

== 単項演算子


次の3つの単項演算子が提供されています。

//blankline 

 * @<tt>{-}: オペランドを加法における逆言（反数）に置き換えます。
 * @<tt>{NOT}: ブーリアンの値を反転します。

//blankline

以下は、負の値の表現の例です。

//emlist[][sql]{
SELECT * FROM _ WHERE n1 >= -10 AND n1 <= 10
//}


以下は、論理否定演算子の利用例です。

//emlist[][sql]{
SELECT * FROM _ WHERE name NOT IN ("James","Jane")
//}

このように、@<tt>{NOT}演算子は、@<tt>{IN}、@<tt>{LIKE}、@<tt>{MATCH}、@<tt>{BETWEEN}などの演算子と組み合わせて使用できます。
なお、@<tt>{NOT}演算は、NULL値やMISSING値に対しては作用しません。NULL値に対する@<tt>{NOT}演算はNULLを返し、MISSING値に対する@<tt>{NOT}演算は、MISSINGを返します。


== COLLATE演算子


@<tt>{COLLATE}演算子を用いて、文字列比較(照合)の実行方法を指定できます。文字列比較式および@<tt>{ORDER BY}句と組み合わせて使用します。

複数の照合を使用する場合は、括弧を用います。照合がひとつだけ使用される場合、括弧はオプションです。

@<tt>{COLLATE}演算子に使用可能なオプションは次のとおりです。

//blankline

 * @<tt>{UNICODE}: Unicode比較を実行します(デフォルトでは、ASCII比較です)。
 * @<tt>{CASE}: 大文字と小文字を区別して比較を行います。
 * @<tt>{DIACRITIC}: アクセントと発音区別符号を考慮に入れます。デフォルトで有効です。
 * @<tt>{NO}: 他の照合の接頭辞として使用して、それらを無効にします（たとえば、@<tt>{NOCASE}は、大文字と小文字を区別しません）。

//blankline

以下に、利用例を示します。

//emlist[][SQL]{
SELECT name FROM _ ORDER BY name COLLATE UNICODE
//}


== CASE演算子


@<tt>{CASE}演算子によって、クエリ中で条件式を評価することが可能です。

シンプルケース(Simple Case)式とサーチドケース(Searched Case)式があります。

=== シンプルケース式

 * @<tt>{CASE}式が最初の@<tt>{WHEN}式と等しい場合、結果は@<tt>{THEN}式になります。
 * @<tt>{CASE}式が先行する@<tt>{WHEN}式に一致しない場合、後続の@<tt>{WHEN}(〜@<tt>{THEN})句は同じ方法で評価されます。
 * すべての@<tt>{WHEN}句がFALSEに評価され、一致するものが見つからない場合、結果は@<tt>{ELSE}式になり、@<tt>{ELSE}式が指定されていない場合は@<tt>{NULL}になります。

//blankline

以下にシンプルケース式の例を示します。

//emlist[][SQL]{
SELECT CASE state WHEN 'CA' THEN 'Local' ELSE 'Non-Local' END FROM _
//}


=== サーチドケース式

 * 最初の@<tt>{WHEN}式がTRUEの場合、この式の結果はその@<tt>{THEN}式になります。
 * それ以外の場合、後続の@<tt>{WHEN}句は同じ方法で評価されます。
 * すべての@<tt>{WHEN}句がFALSEに評価され、一致するものが見つからない場合、、式の結果は@<tt>{ELSE}式になり、@<tt>{ELSE}式が指定されていない場合はNULLになります。

//blankline

以下にサーチドケース式の例を示します。

//emlist[][SQL]{
SELECT CASE WHEN shippedOn IS NOT NULL THEN 'Shipped' ELSE 'Not-Shipped' END FROM _
//}


== 関数


SQL++/N1QLクエリで利用できる関数の全体像を紹介しながら、Couchbase Liteに固有の部分を中心に解説します。
詳細についてはドキュメント@<fn>{lbl-functions}を参照ください。

//footnote[lbl-functions][https://docs.couchbase.com/couchbase-lite/current/android/query-n1ql-mobile.html#lbl-functions]

=== メタデータ関数

メタデータ関数 (@<tt>{META})を用いて、Couchbase Liteドキュメントのメタデータを取得することができます。


以下に、利用例を示します。
この例では、ドキュメントIDとドキュメントが削除されたかどうかを示すフラグを取得しています。

//emlist[][sql]{
SELECT META().id, META().deleted FROM _
//}

上記のようなシンプルなクエリの場合は不要ですが、@<tt>{META(<データソース名>)}のように、データソース名を引数に指定することによって、メタデータを取得する対象を識別することが可能です。
これは、下記のような結合を行っているクエリで利用することができます。

//emlist[][sql]{
SELECT p.name, r.rating FROM _ as p INNER JOIN _ AS r ON META(r).id = p.reviewID WHERE META(p).id = "product123"
//}

=== 配列関数

配列関数は、入力として配列を受け取る関数です。

//table[tbl1][]{
関数	説明
-----------------
ARRAY_AGG(expr)	入力を要素とする配列を返します。MISSINGは無視されます。
ARRAY_AVG(array)	配列内のすべての数値の平均を返します。配列内に平均を求める要素がない場合はNULLを返します。
ARRAY_CONTAINS(array, value)	配列に指定された値が存在する場合はTRUEを返します。それ以外の場合はFALSEを返します。
ARRAY_COUNT(array)	配列内のNULL以外の値の数を返します。
ARRAY_IFNULL(array)	配列内のNULL以外の最初の値を返します。
ARRAY_MAX(array)	配列内の最大の値を返します。
ARRAY_MIN(array)	配列内の最小の値を返します。
ARRAY_LENGTH(array)	配列の長さを返します。
ARRAY_SUM(array)	配列内のすべての数値の合計を返します。
//}

=== 条件関数

条件関数は、複数の入力を受け取り、それらの中からそれぞれの関数に備わった条件に合致する値を返す関数です。

//table[tbl2][]{
関数	説明
-----------------
IFMISSING(expr1, expr2, ...)	MISSING以外の最初の値を返します。すべての値がMISSINGの場合はNULLを返します。
IFMISSINGORNULL(expr1, expr2, ...)	非NULLおよび非MISSINGである最初の値を返します。すべての値がNULLまたはMISSINGの場合はNULLを返します。
IFNULL(expr1, expr2, ...)	最初の非NULL値を返します。すべての値がNULLの場合はNULLを返します
MISSINGIF(expr1, expr2)	expr1 = expr2の場合にMISSINGを返します。それ以外の場合はexpr1を返します。いずれかまたは両方がMISSINGの場合はMISSINGを返します。いずれかまたは両方がNULLの場合はNULLを返します。
NULLF(expr1, expr2)	expr1 = expr2の場合にNULLを返します。それ以外の場合はexpr1を返します。いずれかまたは両方がMISSINGの場合はMISSINGを返します。いずれかまたは両方がNULLの場合はNULLを返します。
//}

=== 日付時間関数

日付時間関数は、日付と時刻を表す異なる形式の文字列間の変換を行います。

//table[tbl3][]{
関数	説明
-----------------
STR_TO_MILLIS(expr)	ISO 8601形式の文字列を、UNIX時間(ミリ秒)に変換します。
STR_TO_UTC(expr)	ISO 8601形式の文字列を、UTCのISO 8601形式の文字列に変換します。
MILLIS_TO_STR(expr)	UNIX時間(ミリ秒)をローカルタイムゾーンのISO 8601形式の文字列に変換します。
MILLIS_TO_UTC(expr)	UNIX時間(ミリ秒)をUTCのISO 8601形式の文字列に変換します。
//}

UNIX時間は、UNIX OSにおいて利用される日時の表現であり、具体的には協定世界時(UTC)での1970年1月1日午前0時0分0秒(UNIXエポック)を基準として、そこから経過した秒数で日時を表現します。

=== パターンマッチ関数

パターンマッチ関数は、正規表現を用いることができる関数です。

//table[tbl4][]{
関数	説明
-----------------
REGEXP_CONTAINS(expr, pattern)	文字列に正規表現(@<tt>{pattern})に一致するシーケンスが含まれている場合はTRUEを返します。
REGEXP_LIKE(expr, pattern)	文字列が正規表現(@<tt>{pattern})と完全に一致する場合はTRUEを返します。
REGEXP_POSITION(expr, pattern)	文字列中の正規表現(@<tt>{pattern})の出現する最初の位置を返します。一致するものが見つからない場合は-1を返します。位置のカウントはゼロから始まります。
REGEXP_REPLACE(expr, pattern, repl [, n])	@<tt>{pattern}が@<tt>{repl}に置き換えられた新しい文字列を返します。nを指定すると、最大n個の置換が実行されます。nが指定されていない場合、一致するすべてが置き換えられます。
//}

=== データ型チェック関数

データ型チェック関数は、入力となる値の型をチェックする関数です。
データ型チェック関数は、評価の結果としてブーリアン値を返します。@<fn>{memo-datacheck}
下記の関数が利用可能です。

//blankline


@<tt>{ISARRAY} @<tt>{ISATOM} @<tt>{ISBOOLEAN} @<tt>{ISNUMBER} @<tt>{ISOBJECT} @<tt>{ISSTRING}

//blankline

@<tt>{ISATOM} は、値がブール値、数値、または文字列の場合に、TRUEを返します。

//footnote[memo-datacheck][SQL++/N1QLではブーリアン評価の結果として、TRUEとFALSE以外にNULLやMISSINGを取ることがあります。データ型チェック関数が評価する値がNULL、またはMISSINGの場合は、ブーリアン型への評価が行われず、そのままNULLまたはMISSINGが返されます。]

=== データ型判別関数

@<tt>{TYPE}関数は、値に型に基づいて、次のいずれかの文字列を返します。

//blankline

@<tt>{"missing"},@<tt>{"null"},@<tt>{"boolean"},@<tt>{"number"},@<tt>{"string"},@<tt>{"array"},@<tt>{"object"},@<tt>{"binary"}

=== データ型変換関数

データ型変換関数は、入力値を特定の型に変換します。
下記のようなデータ型変換関数が提供されています。

//blankline

@<tt>{TOARRAY} @<tt>{TOATOM} @<tt>{TOBOOLEAN} @<tt>{TONUMBER} @<tt>{TOOBJECT} @<tt>{TOSTRING}

=== 算術関数

下記のような算術関数が提供されています。

//blankline

@<tt>{ABS} @<tt>{ACOS} @<tt>{ASIN} @<tt>{ATAN} @<tt>{ATAN2} @<tt>{CEIL} @<tt>{COS} @<tt>{DIV} @<tt>{DEGREES} @<tt>{E} @<tt>{EXP} @<tt>{FLOOR} @<tt>{LN} @<tt>{LOG} @<tt>{PI} @<tt>{POWER} @<tt>{RADIANS} @<tt>{ROUND} @<tt>{ROUND_EVEN} @<tt>{SIGN} @<tt>{SIN} @<tt>{SQRT} @<tt>{TAN} @<tt>{TRUNC}

=== 文字列関数

下記のような文字列関数が提供されています。

//blankline

@<tt>{CONTAINS} @<tt>{LENGTH} @<tt>{LOWER} @<tt>{LTRIM} @<tt>{RTRIM} @<tt>{TRIM} @<tt>{UPPER}


=== 集計関数

集計関数は、入力となる集合に対し、ひとつの結果を返す関数です。下記の関数が利用可能です。

//blankline

@<tt>{AVG} @<tt>{COUNT} @<tt>{MIN} @<tt>{MAX} @<tt>{SUM}

