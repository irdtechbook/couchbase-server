= データマイグレーションツール

Couchbase ServerとRDBとの間でデータマイグレーションを行うツールが、サードパーティーから、あるいはオープンソースプロジェクトとして提供されています。

このような外部ツールを用いなくとも、RDBからCSV形式でデータを出力し、Couchbase Serverが提供しているツール(@<tt>{cbimport})を使って、データマイグレーションプロセスを構築することができます。

一方、このような既存のツールの存在を押さえておくことも決して無意味ではないと考え、ここに紹介します。

== GlueSync

 @<strong>{GlueSync}@<fn>{gluesync}は、MOLO17@<fn>{molo17}社によって提供される「The only Couchbase-native data streaming connector for relational databases」です。
 Microsoft SQL ServerまたはOracleとCouchbase Serverとの間でリアルタイムデータ同期を実現します。

 以下、サイトから引用します。

//quote{
GlueSync allows you to replicate data to and from RDBMS (Microsoft SQL Server and Oracle) in real time with Couchbase. Whether the goal is to Data Offload or increase the potential of your Business Apps, GlueSync is the only zero impact plug&play tool on your infrastructure.
//}


//footnote[gluesync][https://gluesync.com/]

//footnote[molo17][https://molo17.com/]


== Couchgres

 @<strong>{Couchgres}@<fn>{couchgres}はオープンソースプロジェクトであり、その名の通り、CouchbaseとPostgreSQLとのデータマイグレーションを実現します。

 以下、GitHubリポジトリーのREADMEから引用します。

//quote{
These are some small tools for easier migration between PostgreSQL and Couchbase 7.0, and visualising a Couchbase bucket with scopes and collections in the form of a pie chart.
//}

//footnote[couchgres][https://github.com/metonymic-smokey/couchgres]

== SqlServerToCouchbase

@<strong>{SqlServerToCouchbase}@<fn>{SqlServerToCouchbase}は、SQL ServerからCouchbaseへのデータ移動のためのライブラリーを提供するオープンソースプロジェクトです。

以下、GitHubリポジトリーのREADMEから引用します。

//quote{
A library to help guide your SQL Server to Couchbase migration/sync efforts. This library represents a best effort at mapping SQL Server concepts to Couchbase Server concepts automatically. It may or may not fulfill every one of your requirements. In the worst case, it can at least be an educational tool. Think of it like Google Translate.
//}

//footnote[SqlServerToCouchbase][https://github.com/mgroves/SqlServerToCouchbase]

このツールを紹介しつつ、ツールの紹介に終わらない、RDBからCouchbase Serverへのマイグレーションに関する論点を整理したブログ記事「SQL to NoSQL: Automated Migration」@<fn>{sql-to-nosql-automated-migration}が発表されています。


//footnote[sql-to-nosql-automated-migration][https://blog.couchbase.com/sql-to-nosql-automated-migration/]