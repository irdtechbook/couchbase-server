
= Couchbase Liteインデックス


== 概要


Couchbase Liteは、多くのデータベースと同じように、インデックス(Index@<fn>{document-indexing})を作成することによってクエリを高速化することができます。

インデックス作成はオプションであり、インデックスが作成されていないプロパティーを検索条件に利用することが可能です。

=== 留意点

インデックスを利用する際には、以下に留意する必要があります。

//blankline

 * ドキュメントが更新されるたびにインデックスが更新されるため、データベースへの書き込み操作時に多少なりともオーバーヘッドが生じます。
 * インデックスの情報がデーターベースに格納されるため、データベースのサイズが大きくなります。
 * インデックスの数が多すぎると、パフォーマンスが低下する可能性があります。

//footnote[document-indexing][https://docs.couchbase.com/couchbase-lite/current/android/indexing.html]


== インデックス操作


=== インデックス作成


以下の例は、ドキュメントのプロパティー@<tt>{type}と@<tt>{name}に対して、新しいインデックスを作成します。

//emlist[][JAVA]{
database.createIndex(
  "TypeNameIndex",
  IndexBuilder.valueIndex(
    ValueIndexItem.property("type"),
    ValueIndexItem.property("name")));
//}



=== インデックス削除

以下の例は、指定した名前のインデックスを削除します。

//emlist[][JAVA]{
database.deleteIndex("TypeNameIndex");
//}

== インデックス利用条件

インデックスが作成されているプロパティーがクエリの検索条件として利用されている場合、インデックスが利用されます。
ただし、クエリの内容によって、以下のような条件があります。

=== LIKE検索

LIKE検索では、次の条件が満たされている場合のみ、インデックスが使用されます。

//blankline

 * 検索文字列がワイルドカードで始まらない。
 * 検索文字列は実行時に既知の値である（クエリの処理中に導出された値ではない）。

=== 関数利用

インデックスの作成されているプロパティーを検索条件に用いる際、そのプロパティーに対して関数が適用されていると、インデックスは利用されません。

たとえば、以下のようなクエリでは、@<tt>{name}プロパティーに対してインデックスが作成されている場合であっても、そのインデックスは利用されません。

//emlist[][JAVA]{
Query query = QueryBuilder
  .select(SelectResult.all())
  .from(DataSource.database(database))
  .where(Function.lower(Expression.property("name")).equalTo(Expression.string("apple"))); 

Log.i(TAG, query.explain());
//}

このようなケースにおいては、クエリ実行時ではなく、インデックスを作成するときに関数を適用しておくことが考えられます。

== インデックス最適化

Couchbase Liteでは、クエリオプティマイザーによる統計情報の収集に基づき、クエリ速度向上のためのインデックス最適化が行われます。

統計情報は、次のような特定のイベントの後に収集されます。

//blankline

 * インデックス作成後
 * データベース再オープン後
 * データベース圧縮後

//blankline

統計情報収集の効果として、データベースをオープンした後や、インデックスを削除して再作成した後など、それ以前と比べてクエリの実行速度が速くなる場合があります。

ß
//footnote[query-troubleshooting-queries-and-indexes][https://docs.couchbase.com/couchbase-lite/current/objc/query-troubleshooting.html#queries-and-indexes]