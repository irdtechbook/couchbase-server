
= Couchbase LiteクエリビルダーAPI

Couchbase LiteのクエリビルダーAPI@<fn>{document-querybuilder}について解説します。


//footnote[document-querybuilder][https://docs.couchbase.com/couchbase-lite/current/android/querybuilder.html]

== SELECT

SELECTステートメントでは、クエリの結果セットを指定します。

結果セットとして、特定のプロパティーのみを取得することも、ドキュメント全体を取得することもできます。
また、ドキュメントIDや、検索結果件数の取得も可能です。

クエリの結果セットは配列です。
配列を構成する各要素はJSONオブジェクトですが、各要素の構成はSELECTステートメントに指定された内容によって決定されます。

クエリビルダーAPIにおいて、SELECTステートメントに指定する内容は、@<tt>{SelectResult}クラスで表現されます。


=== すべてのプロパティーを取得

すべてのプロパティーを取得するには、@<tt>{SelectResult.all()}メソッドを使います。


//emlist[][java]{
Query query = QueryBuilder.select(SelectResult.all()).from(DataSource.database(database));
//}


この場合の結果セットは、配列の要素オブジェクトごとにデータベース名を唯一のキーとして持ち、その値はドキュメントの各プロパティーをキーと値のペアとする辞書型オブジェクトです。

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

以下は、すべてのプロパティーを取得した場合の結果セットの利用例です。
@<tt>{Result}オブジェクトにはデータベース名をキーとした辞書型オブジェクトのエントリーがただひとつ存在します。

//emlist[][JAVA]{
for (Result result : query.execute().allResults()) {

    Dictionary props = result.getDictionary(0); 
    String id = props.getString("id");
    String name = props.getString("name");
    String city = props.getString("city");
}
//}

=== 特定のプロパティーを取得

特定のプロパティーを指定して取得するには、クエリのSELECTステートメントで、プロパティーごとにひとつずつ、カンマで区切られたアイテムのリストを用います。


//emlist[][JAVA]{
Query query = QueryBuilder.select(
                SelectResult.property("id"),
                SelectResult.property("type"),
                SelectResult.property("name"))
                .from(DataSource.database(database));
//}


この場合、結果セットの配列に含まれる要素オブジェクトは、指定したプロパティーのキーと値のペアで構成されます。

次の例を参照してください。

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

以下は、特定のプロパティーを取得した場合の結果セットの利用例です。

//emlist[][JAVA]{
for (Result result : query.execute().allResults()) {

    String name = result.getString("name");
    String city = result.getString("city");
}
//}


=== ドキュメントID取得

以下は、ドキュメントIDを取得するクエリの例です。

//emlist[][JAVA]{
Query query = QueryBuilder.select(SelectResult.expression(Meta.id).as("id"))
  .from(DataSource.database(database));
//}


結果セットは、以下のようなJSONオブジェクトの配列になります。

//emlist[][JSON]{
[
  { "id": "123" },
  { "id": "456" }
]
//}


=== 件数取得

以下は、ドキュメントの件数を取得するクエリの例です。


//emlist[][JAVA]{
Query query = QueryBuilder.select(
    SelectResult.expression(Function.count(Expression.string("*"))).as("count")) 
    .from(DataSource.database(database));
//}

結果セットは、以下のようにエイリアス名と件数の組み合わせになります。


//emlist[][JSON]{
{ "count": 6 }
//}

== WHERE


WHERE句を使用して、クエリによって返されるドキュメントの選択基準を指定できます。
WHERE句のフィルタリングを実現するために、任意の数の式を組み合わせることができます。

クエリビルダーAPIにおいてWHERE句を表現するためには、@<tt>{Expression}クラスが使われます。


=== 比較演算子

WHERE句では、条件を指定するために比較演算子を使用することができます。

以下のコードは、「type」プロパティーが「hotel」に等しいドキュメントを選択しています。


//emlist[][JAVA]{
Query query = QueryBuilder.select(SelectResult.all())
                .from(DataSource.database(database))
                .where(Expression.property("type").equalTo(Expression.string("hotel")));
//}

=== IN

IN演算子は、対象を複数指定する場合に使います。


次の例では「name」プロパティーの値が、「Foo」、「Bar」または「Baz」のいずれかに等しいドキュメントを検索します。

//emlist[][JAVA]{
Expression[] values = new Expression[] {
  Expression.string("Foo"),
  Expression.string("Bar"),
  Expression.string("Baz")
};

Query query = QueryBuilder.select(SelectResult.all())
  .from(DataSource.database(database))
  .where(Expression.property("name").in(values));
//}

次の例では「first」、「last」または「username」のいずれかのプロパティーの値が「Cameron」に等しいドキュメントを検索します。

//emlist[][JAVA]{
Expression[] properties = new Expression[] {
  Expression.property("first"),
  Expression.property("last"),
  Expression.property("username")
};

Query query = QueryBuilder.select(SelectResult.all())
  .from(DataSource.database(database))
  .where(Expression.string("Cameron").in(properties));
//}

=== 文字列マッチング(LIKE)


文字列マッチングのために、LIKE演算子を使用することができます。


LIKE演算子を用いた文字列マッチングでは、大文字と小文字が区別されます。
大文字と小文字を区別しないマッチングを実行するには、 関数(@<tt>{Function.lower}または@<tt>{Function.upper})を使用して、大文字と小文字の違いを取り除きます。

SQL同様、以下の二種類の照合が可能です。

//blankline

 * ワイルドカードマッチ(@<tt>{%}): 0個以上の文字に一致します。
 * キャラクターマッチ(@<tt>{_}): 1文字に一致します。

//blankline

以下に例を示します。

「@<tt>{%}」記号を使用して、0個以上の文字に対してワイルドカードマッチを行うことができます。

以下の例では、@<tt>{"eng%e%"}という指定により、「name」プロパティーの値に対してワイルドカードマッチを行います。


//emlist[][JAVA]{
Query query = QueryBuilder.select(SelectResult.all())
  .from(DataSource.database(database))
  .where(Expression.property("type").equalTo(Expression.string("landmark"))
  .and(Function.lower(Expression.property("name")).like(Expression.string("eng%e%"))));
//}


「@<tt>{_}」記号を使用して、単一の文字に対するキャラクターマッチを行うことができます。


以下の例では、@<tt>{"eng____r"}という指定により、「name」プロパティーの値に対して、キャラクターマッチを行います。「eng」で始まり、その後に正確に4つの任意の文字が続き、最後が「r」で終わる文字列に一致するドキュメントを検索しています。


//emlist[][JAVA]{
Query query = QueryBuilder.select(SelectResult.all())
  .from(DataSource.database(database))
  .where(Expression.property("type").equalTo(Expression.string("landmark"))
  .and(Function.lower(Expression.property("name")).like(Expression.string("eng____r"))));
//}



=== 正規表現演算子


LIKE式表現に加えて、正規表現によるパターンマッチングを利用することができます。
正規表現によるパターンマッチングを行うための@<tt>{regex}メソッドが提供されています。

以下の例では、「name」プロパティーの値が、「eng」で始まり「r」で終わる任意の文字列に一致するデータを検索しています。

//emlist[][JAVA]{
Query query = QueryBuilder.select(SelectResult.all())
  .from(DataSource.database(database))
  .where(Expression.property("type").equalTo(Expression.string("landmark"))
  .and(Function.lower(Expression.property("name"))
  .regex(Expression.string("\\beng.*r\\b"))));      
//}

Couchbase Liteで使用される正規表現仕様の詳細については、cplusplus.com正規表現リファレンスページ@<fn>{8500ce78c807a8f6b27ea5e932c6bea2}を参照ください。

=== NULLおよび未定義値

NULL値の扱いは、SQLに準じています。
JSONデータは、プロパティーの値としてnullを指定できます。
一方、存在していない値について、プロパティー自体を設けないこともできます。

NULLと未定義値を区別する必要はない場合があります。
その場合に、クエリを簡潔に表現することのできる@<tt>{isValued}と@<tt>{isNotValued}メソッドが用意されています。@<fn>{Expression-isValued}
これらは、プロパティー自体が定義されていないことと、プロパティーの値がnullであることを区別しません。

次の例は、「email」プロパティーが定義されており、かつnull以外の何らかの値を持っているドキュメントを検索しています。

//emlist[][JAVA]{
Query query = QueryBuilder
  .select(SelectResult.expression(Expression.property("email")))
  .from(DataSource.database(database))
  .where(Expression.property("email").isValued());
//}

//footnote[Expression-isValued][https://docs.couchbase.com/mobile/3.0.0/couchbase-lite-java/com/couchbase/lite/Expression.html#isValued()]

=== メタデータ

ドキュメントのメタデータを条件としてクエリを行うことが可能です。

以下の例では、ドキュメント有効期限が５分以内のドキュメントを検索しています。

//emlist[][JAVA]{
Instant fiveMinutesFromNow = Instant.now().plus(5, ChronoUnit.MINUTES);

Query query = QueryBuilder.select(SelectResult.expression(Meta.id))
    .from(DataSource.database(database))
    .where(Meta.expiration.lessThan(
        Expression.doubleValue(fiveMinutesFromNow.toEpochMilli())));
//}

=== 配列

WHERE句の中で、配列型のプロパティーを持つドキュメントに対して、配列の要素を用いたフィルタリングを行うことが可能です。


指定された値が配列に存在するかどうかを確認するために、@<tt>{ArrayFunction}クラスを利用できます。

次の例では、配列型の値を持つ「like」プロパティーに「Graham Nash」と等しい値が含まれているドキュメントを検索します。


以下のように、プロパティーの値が配列となっているドキュメントに対して用います。

//emlist[][JSON]{
{
  "id": "hotel456",
  "type": "hotel",
  "name": "Chelsea Hotel",
  "city": "New York"
  "like": ["Leonard Cohen", "Graham Nash", "Nico"]
}
//}

//emlist[][JAVA]{
Query query = QueryBuilder.select(SelectResult.all())
  .from(DataSource.database(database))
  .where(Expression.property("type").equalTo(Expression.string("hotel"))
    .and(ArrayFunction.contains(Expression.property("like"), 
        Expression.string("Graham Nash"))));
//}

Couchbase LiteのN1QLは、配列型のプロパティーに対する、より複雑な検索条件を利用する方法を提供しています。
これは、クエリビルダーAPIでも、SQL++/N1QL APIでも同様に利用可能です。
ただしSQLには存在しない構文であるため、クエリ文字列の方が理解しやすいと思われるため、配列に対する高度な検索については、SQL++/N1QLクエリAPIの解説において扱います。

クエリビルダーAPIを用いた詳細な解説について、Couchbase Blog: How to Query Array Collections in Couchbase Lite@<fn>{querying-array-collections-couchbase-mobile}を参考として紹介します。

//footnote[querying-array-collections-couchbase-mobile][https://blog.couchbase.com/querying-array-collections-couchbase-mobile/]

== JOIN

JOIN句を用いて、指定された基準によってリンクされた複数のデータソースからデータを選択できます。

Couchbase Liteには、リレーショナルデータベースにおけるテーブルに対応する概念が存在しないため、クエリの照会先データソースはデータベースそのものです。
Couchbase Liteでは、データベースはファイルに対応しており、複数のデータベース(ファイル)間の結合はサポートされていません。@<fn>{memo-cbs-join}
そのため、Couchbase Liteにおける結合は自己結合となります。


//footnote[memo-cbs-join][Couchbase Serverでは、リレーショナルデータベースにおけるテーブルに対応するコレクションというキースペース(名前空間)が存在し、コレクション間の結合がサポートされています。]



以下は、航空会社(airline)の情報を、ルート(route)の情報と組み合わせる例です。
それぞれ「type」プロパティーとして「airline」と「route」を持ち、「route」タイプのドキュメントには、「airline」タイプのドキュメントのドキュメントIDが、プロパティー「airlineid」として格納されています。


//emlist[][java]{
Query query = QueryBuilder.select(
  SelectResult.expression(Expression.property("name").from("airline")),
  SelectResult.expression(Expression.property("callsign").from("airline")),
  SelectResult.expression(Expression.property("destinationairport").from("route")),
  SelectResult.expression(Expression.property("stops").from("route")),
  SelectResult.expression(Expression.property("airline").from("route")))
  .from(DataSource.database(database).as("airline"))
  .join(Join.join(DataSource.database(database).as("route"))
  .on(Meta.id.from("airline").equalTo(Expression.property("airlineid").from("route"))))
  .where(Expression.property("type").from("route").equalTo(Expression.string("route"))
  .and(Expression.property("type").from("airline").equalTo(Expression.string("airline")))
  .and(Expression.property("sourceairport")
  .from("route").equalTo(Expression.string("RIX"))));
//}

== GROUP BY

GROUP BY句を利用すると、検索結果のデータに対してグループ化を行うことができます。

ここでの例では、以下のような情報を持つ空港(airport)ドキュメントを用います。


//emlist[][JSON]{
{
  "_id": "airport123",
  "type": "airport",
  "country": "United States",
  "geo": { "alt": 456 },
  "tz": "America/Anchorage"
}
//}


次の例は、高度が300フィートを超えるすべての空港を選択するクエリを示しています。結果は、国およびタイムゾーンごとにグループ化され、結果にはグループ毎の件数が含まれます。


//emlist[][JAVA]{
Query query = QueryBuilder.select(
    SelectResult.expression(Function.count(Expression.string("*"))),
    SelectResult.property("country"),
    SelectResult.property("tz"))
    .from(DataSource.database(database))
    .where(Expression.property("type").equalTo(Expression.string("airport"))
        .and(Expression.property("geo.alt").greaterThanOrEqualTo(Expression.intValue(300))))
    .groupBy(Expression.property("country"), Expression.property("tz"));
//}

== ORDER BY

ORDER BY句を利用すると、クエリの結果を並べ替えることができます。


次の例では、「name」プロパティーの値の昇順で並べ替えられたドキュメントが返されます。


//emlist[][JAVA]{
Query query = QueryBuilder.select(
    SelectResult.expression(Meta.id),
    SelectResult.property("name"))
    .from(DataSource.database(database))
    .where(Expression.property("type").equalTo(Expression.string("hotel")))
    .orderBy(Ordering.property("name").ascending());
//}


== LIMIT

LIMIT句を利用して、指定されたオフセットから始まる指定された数の結果を返すことができます。

これはたとえば、大量の結果を返すクエリでページネーションを処理するために利用することができます。

//emlist[][JAVA]{
int offset = 0;
int limit = 20;

Query query = QueryBuilder.select(SelectResult.all())
    .from(DataSource.database(database))
    .limit(Expression.intValue(limit), Expression.intValue(offset)); 
//}


== 日付データ


Couchbase Liteは、JSONドキュメントのプロパティーとしてDateタイプ@<fn>{querybuilder-lbl-date-time}をサポートしています。Dateタイプは内部的に、ISO8601形式の日付(GMT/UTCタイムゾーン)を持っています。

ISO8601形式の日付と、他のフォーマットの日付とを比較するために、以下のようなメソッドが提供されています。


次の例では、ISO8601に従って有効にフォーマットされた文字列を入力として取り、出力はUnixエポックからのミリ秒を表す@<tt>{Expression}オブジェクトになります。

//emlist[][JAVA]{
Expression eStM = Function.stringToMillis(Expression.property("date_time_str"));
//}

次の例では、ISO8601に従って有効にフォーマットされた文字列を入力として取り、出力は日付と時刻をUTC ISO8601文字列として表す文字列コンテンツを表す@<tt>{Expression}オブジェクトになります。

//emlist[][JAVA]{
Expression eStU = Function.stringToUTC(Expression.property("date_time_str"));
//}


次の例では、入力はUnixエポックからのミリ秒を表す数値です。
出力はデバイスのタイムゾーンでISO 8601文字列として日付と時刻を表す文字列コンテンツを表す@<tt>{Expression}オブジェクトになります。

//emlist[][JAVA]{
Expression eMtS = Function.millisToString(Expression.property("date_time_millis"));
//}

次の例では、入力はUnixエポックからのミリ秒を表す数値です。
出力は、日付と時刻をUTC ISO8601文字列として表す文字列コンテンツを表す@<tt>{Expression})オブジェクトになります。

//emlist[][JAVA]{
Expression eMtU = Function.millisToUTC(Expression.property("date_time_millis"));
//}

//footnote[8500ce78c807a8f6b27ea5e932c6bea2][http://www.cplusplus.com/reference/regex/ECMAScript/]

//footnote[querybuilder-lbl-date-time][https://docs.couchbase.com/couchbase-lite/current/android/querybuilder.html#lbl-date-time]


//footnote[sql-for-json-query-interface-couchbase-mobile][https://blog.couchbase.com/sql-for-json-query-interface-couchbase-mobile/]


