= Couchbase Lite全文検索

== 概要

Couchbase Liteは、全文検索(Full-Text Search@<fn>{document-fts})機能を提供します(以下では機能名として、Full-Text Searchの略称「FTS」を用います)。

//footnote[document-fts][https://docs.couchbase.com/couchbase-lite/current/android/fts.html]

=== 言語サポート

Couchbase LiteのFTSは、単語の区切りに空白を使用する言語で利用することができます。

また、ステミング（語幹処理）は、デンマーク語、オランダ語、英語、フィンランド語、フランス語、ドイツ語、ハンガリー語、イタリア語、ノルウェー語、ポルトガル語、ルーマニア語、ロシア語、スペイン語、スウェーデン語、トルコ語でサポートされています。 

日本語は以上の条件を満たしていませんが、全文検索を利用する場面として、以下のケースが考えられます。

//blankline

 * サポートされる言語（たとえば英語）による文章（たとえば、システムログメッセージ）に対する検索
 * サポートされる言語（たとえば英語）を持つ複数のプロパティーに対する検索（通常のクエリでも、複数の条件を組み合わせることはできますが、シンプルな実装になる可能性があります）

====[column]Couchbase Liteの差別化要因としてのFTS

FlutterのCouchbase Lite用パッケージcbl-dart@<fn>{github-cbl-dart}の開発者であるGabriel Terwestenによるブログ記事「How to make your Flutter app offline-first with Couchbase Lite」@<fn>{medium-flutter}では、以下のように書かれています。


「And then there is full-text search (FTS), or rather the lack thereof. The Firestore documentation basically tells you to check out Elastic, Algolia or whatever… just figure it out. Hm, ok, what about SQLite? It comes preinstalled on many systems and has multiple FTS extensions. Looking into it, I found that while possible, it's not exactly simple to make it work. Not all preinstalled versions of SQLite have the same FTS extension enabled or any at all. Of course, the versions of SQLite itself differ between platforms and platform versions, so you have to take that into account. As with any relational database, you have to manage a schema and setting up a FTS index in SQLite is not super simple, either.」


以下、上記引用を翻訳します。

 「そして、全文検索(FTS)、というよりもむしろ、その欠如が問題でした。 Firestoreのドキュメントは、基本的にこんな言いぶりです。Elastic、Algolia、あるいは他の何かを使って自分でなんとかしてください。ああ、そうですか。SQLiteはどうでしょうか?多くのシステムにプリインストールされており、複数のFTS拡張機能があります。調べてみたところ、可能ではあるにしても、思ったようなことを行うのは簡単ではないことがわかりました。プリインストールされているSQLiteのすべてのバージョンで同じFTS拡張機能が使えるわけでもなければ、そもそも使えないものもあります。当然のことながら、SQLiteのバージョン自体がプラットフォームとプラットフォームのバージョンによって異なるため、それを考慮に入れる必要もあります。他のリレーショナルデータベースと同様に、スキーマを管理する必要もあります。いずれにせよ、SQLiteでFTSインデックスをセットアップするのも非常に単純というわけではありません。」


このような認識から、著者は、自身のFlutterアプリ開発のためにCouchbase Liteを選択し、Flutter用のパッケージを開発しています。

====[/column]

//footnote[github-cbl-dart][https://github.com/cbl-dart/cbl-dart]
//footnote[medium-flutter][https://medium.com/flutter-community/how-to-make-your-flutter-app-offline-first-with-couchbase-lite-86bb23780f74]

== FTSインデックス


FTSクエリでは、通常のクエリとは異なり、インデックス作成はオプションではありません。
FTSクエリ実行のための前提条件としてFTSインデックスが作成されている必要があります。

=== 作成


@<tt>{Database}クラスの@<tt>{createIndex}メソッドに、@<tt>{FullTextIndexConfiguration}オブジェクトを渡すことにより、FTSインデックス作成します。


次の例では、「overview」プロパティーにFTSインデックスを作成します。


//emlist[][]{
FullTextIndexConfiguration config = new FullTextIndexConfiguration("overview").ignoreAccents(false);

database.createIndex("overviewFTSIndex", config);
//}

@<tt>{FullTextIndexConfiguration}を作成する際、複数のプロパティーを指定することもできます。

== FTSクエリ

インデックスを作成すると、インデックス付きのプロパティーに対してFTSクエリを作成して実行できます。


なお、FTSでは大文字と小文字が区別されません。

=== クエリビルダーAPI

クエリビルダーAPIでは、全文検索条件は、@<tt>{FullTextFunction}を使って指定します。

//emlist[][]{
Expression whereClause = FullTextFunction.match("overviewFTSIndex", "michigan");

Query query = QueryBuilder.select(SelectResult.all())
    .from(DataSource.database(database))
    .where(whereClause);
//}

=== SQL++/N1QLクエリAPI

SQL++/N1QLクエリAPIでは、全文検索条件は、@<tt>{MATCH}を使って指定します。

//emlist[][]{
Query query = database.createQuery("SELECT * FROM _ WHERE MATCH(overviewFTSIndex, 'michigan')");
//}

FTSインデックスの照合には、上記のような単純な検索語指定だけでなく、以下で紹介するパターンマッチングフォーマットを使うことができます。

== パターンマッチングフォーマット

以下の形式で、検索条件として照合するパターンを指定することができます。

=== プレフィックスクエリ

検索語に続けて「*」（アスタリスク）文字を付すことで、プレフィックス（接頭辞）によるクエリを行うことができます。

以下の例では、接頭辞「lin」が付いた単語を含むすべてのドキュメントを検索します。

//emlist{
lin*
//}

=== プロパティー名指定

FTSインデックス作成時に、複数のプロパティーを指定することができます。
通常、検索語は、インデックス定義に用いられた全てのプロパティーと照合されます。プロパティー名と「:」の組み合わせに続いて検索語を指定することで、インデックス中の特定のプロパティーに対する照合を表現することができます。

プロパティー名と「:」の間にはスペースを含みません。「:」と検索語の間にはスペースを入れます。

次の例は、「linux」という単語がドキュメントの@<tt>{title}プロパティーに存在し、「problems」という単語がFTSインデックス作成時に指定されたいずれかのプロパティーに含まれるドキュメントを検索します。


//emlist{
title: linux problems
//}

=== フレーズクエリ


フレーズクエリは、スペースで区切られた単語のシーケンスを二重引用符（「"」）で囲むことによって指定します。


次の例は、「linux applications」というフレーズを含むドキュメントを検索します。


//emlist{
"linux applications"
//}

=== NEARクエリ

NEARクエリを利用して、複数の検索語の関係を指定することができます。
NEARクエリでは、ふたつの検索語の間にキーワード@<tt>{NEAR}を置くことで、これらの検索語間の近接度を指定します。
近接度を指定するには、「@<tt>{NEAR/n}」の形式を用います。ここで、@<tt>{n}は、近接度を示す数字です。デフォルトの近接度は10です。

次の例は、「database」という単語と「replication」という単語を含み、これらの単語の間に存在する単語がふたつ以下である(3つ以上の単語によって隔てられていない)ドキュメントを検索します。

//emlist{
database NEAR/2 replication
//}

=== 演算子 AND, OR, NOT

FTSクエリ構文は、演算子として、@<tt>{AND}、@<tt>{OR}、@<tt>{NOT}をサポートします。

FTSクエリでは、演算子には必ず大文字を使用する必要があることに注意が必要です。それ以外の場合は、演算子ではなく検索語として解釈されます。


次の例は、「couchbase」という単語と「database」という単語の両方を含むドキュメントを検索します。


//emlist{
couchbase AND database
//}

=== 演算子の優先順位

括弧を使用して、さまざまな演算子の優先順位を指定できます。

以下の例は、「linux」という単語を含み、かつ「couchbase database」または「sqlite library」というフレーズの少なくともひとつを含むドキュメントを検索します。


//emlist{
("couchbase database" OR "sqlite library") AND linux
//}

== ランキング


全文検索において、検索結果を関連性の高い順に並べ替えることは、一般的な要件です。
Couchbase Liteは、このような要件に応えるランキング機能を提供しています。

FTSクエリと組み合わせて利用することができるRANK関数が提供されています。
RANK関数の結果をORDER BY句に指定することによって、検索結果を関連性の高い順に並べ替えることができます。

=== クエリビルダーAPI

以下は、クエリビルダーAPIにおけるRANK関数利用の例です。

//emlist[][]{
Expression whereClause = FullTextFunction.match("overviewFTSIndex", "'michigan'");

Query query = QueryBuilder.select(SelectResult.all())
    .from(DataSource.database(database))
    .where(whereClause)
    .orderBy(Ordering.expression(FullTextFunction.rank("overviewFTSIndex")).descending());
//}

=== SQL++/N1QLクエリAPI

以下は、SQL++/N1QLクエリAPIにおけるRANK関数利用の例です。

//emlist[][]{
Query query = database.createQuery("SELECT * FROM _ WHERE MATCH(overviewFTSIndex, 'michigan') ORDER BY RANK(overviewFTSIndex)");
//}


//footnote[FTS-xcplaygroundpage-Contents-swift][https://github.com/couchbaselabs/couchbase-lite-ios-api-playground/blob/master/CBLQueryPlayground.playground/Pages/FTS.xcplaygroundpage/Contents.swift]
//footnote[full-text-search-couchbase-mobile-2-0][https://blog.couchbase.com/full-text-search-couchbase-mobile-2-0/}]

