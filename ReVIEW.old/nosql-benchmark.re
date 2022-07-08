= NoSQL性能評価: MongoDB、Cassandraとの比較

以下のNoSQLデータベースに対して実施されたベンチマークの内容を紹介します。

//blankline

 * MongoDB v3.6
 * DataStax Enterprise v6 (Cassandra)
 * Couchbase Server v5.5

//blankline

ここで紹介するベンチマークは、2018年にAltoros@<fn>{bbb133363925367c0a5c46eccf1cc6ef}社によって実施されたもので、下記のタイトルで公開されています。

//blankline

NoSQL Performance Benchmark 2018: Couchbase Server v5.5, DataStax Enterprise v6 (Cassandra), and MongoDB v3.@<fn>{10933fbacf006df6472c3df2d355ed2f}

//blankline

ここでは、このベンチマークについて引用を行いながら、全体の要約を試みます。
要約に際して、偏った解釈を交えないよう心掛けていますが、正確な内容や詳細の確認については原本に当たっていただきますようお願いします。

また、Altoros社によってCouchbase Serverの検証に用いられた方法が不適切と考えられる箇所で、指摘を行います。

== ベンチマーク概要

=== 検証パターン

検証のパターンとして、２種類のYCSB(Yahoo! Cloud Serving Benchmark)@<fn>{ba11732b1ec091926ca4e319cd264f5f}標準ワークロードと、２種類の独自のクエリが用いられています。

//blankline

 * ワークロード A: 更新処理
 * ワークロード E: レンジスキャン

 * クエリ１: ページネーション（OFFSETとLIMITによるフィルター）
 * クエリ２: ジョイン（テーブル結合）

//blankline

クエリ１とクエリ２は、それぞれ下記のドメインとシナリオを表現しています。

//blankline

 * 財務: 地域を指定した財務データの一覧表示(ブラウザー表示のためのページネーションを考慮)
 * eコマース: 期間と地域を指定した売り上げ集計レポート

=== クラスター構成

サイズの異なる以下の３通りのクラスターで、それぞれ異なるサイズのデータを用いて検証が実施されています。

//blankline

 * 4ノード
 * 10ノード
 * 20ノード

=== サーバーハードウェアスペック

検証環境としてAWS(Amazon Web Services)が利用され、クラスターを構成するサーバーとして、下記スペックのEC2インスタンスが用いられています。


//table{
----------------------------
インスタンスタイプ	i3.2xlarge (Storage Optimized)
vCPUs	8
メモリー	61 GB
インスタンスストレージ	1,900 GB (SSD)
ネットワークパフォーマンス	最大 10 GB
オペレーティングシステム	Ubuntu 16.04 LTS
//}



=== 共通条件

 * データサイズは、メモリーサイズと適合している状態
 * 各データセットに対してひとつのレプリカ(複製)


=== データベース固有の構成

@<strong>{Couchbase Server}は、本番環境ではノード毎に異なるサービスを割り当てることが一般的ですが、ここでは以下の簡略化された構成が用いられています。

//blankline

 * クラスターのサイズを問わず、全てのノードでDataサービス、Indexサービス、Queryサービスを有効化。
 * Dataサービスには、データベースに使用可能なRAMの60%を割り当て。
 * Indexサービスには、データベースに使用可能なRAMの40%を割り当て。インデックスはすべてのノードに複製。

//blankline

@<strong>{MongoDB}は、Routerプロセス、Configサーバー、およびデータシャードからなる階層型クラスタートポロジーを持っていますが、ここでは以下の構成が使用されています。

//blankline

 * Configサーバーは、3つのメンバーからなるレプリカセットとして構成(クラスターのノード数としてはカウントしない、別のサーバーを利用）。
 * 各シャードは、3つのメンバー (Primary、Secondary、Arbeiter)からなるレプリカセットとして構成。
 *  @<tt>{mongos} （Routerプロセス）は、クライアント環境にデプロイ。

//blankline

@<strong>{Cassandra}のクラスター構成については、ノード毎の差異はなく、全てのノードに共通の設定が適用されています。


== 検証パターン別詳細

以下、各検証パターン毎に、下記の情報を整理します。

//blankline

 * データアクセス特性
 * 利用データ件数
 * 評価用クエリ
 * 測定結果

=== ワークロード A: 更新処理

@<strong>{データアクセス特性}

 * 読み取り: 50%
 * 更新: 50%



//table[zf679lFIDH][ワークロード A 利用データ件数]{
クラスター構成	テストデータ
----------------------------
4 ノード	5千万件 (1KB/1件)
10 ノード	1 億件
20 ノード	2 億件
//}



//image[altros_nosql_benchmark_workload_A_query][ワークロード A 評価用クエリ]{
//}



//image[altros_nosql_benchmark_workload_A_result][ワークロード A 測定結果]{
//}

=== ワークロード E: レンジスキャン

@<strong>{データアクセス特性}

 * 読み取り: 95%
 * 更新: 5%



//table[2I40ah2Obz][ワークロード E 利用データ件数]{
クラスター構成	テストデータ
----------------------------
4 ノード	5千万件 (1KB/1件)
10 ノード	1 億件
20 ノード	2 億5千万件
//}



//image[altros_nosql_benchmark_workload_E_query][ワークロード E 評価用クエリ]{
//}



//image[altros_nosql_benchmark_workload_E_result][ワークロード E 測定結果]{
//}


冒頭に記した、「Altoros社によるベンチマーク方法が不適切と考えられる箇所」は、ワークロードEに関係しています。
原本中の解説では、以下のインデックスを利用したとされています。

//source{
CREATE PRIMARY INDEX `ycsb_primary` ON `ycsb`
USING GSI WITH {"nodes": [...]};
//}

ここには、Couchbase Serverのプライマリインデックスに対する誤解が見えます。

原本には、「スキャン操作はCouchbaseの主キーに対して実行されるため(Because the scan operation is performed over the primary key in Couchbase)」このプライマリインデックスが作成された、とされています。
しかしながら実際には、Couchbase Serverにおいて、主キーに相当するものはドキュメントキーであり、Couchbase Serverのプライマリインデックスは、主キーとは直接関係がないものです。
Couchbase Serverのプライマリインデックスは、そのキースペース（この検証では、@<tt>{ycsb}バケット）に対する、ドキュメントの特定のフィールドでの検索に限定されないクエリを可能にする（そのキースペースに対する全件スキャンを行うことを可能にする）ためのものであって、ドキュメントキーを使った検索を最適化するものではありません。

検証に用いられたクエリの実行を最適化するためには、以下のように実際にクエリのWHERE句で用いられている@<tt>{META().id}を指定した(セカンダリー)インデックスを作成する必要があります。

//emlist{
CREATE INDEX ycsb_secondary on ycsb(META().id);
//}

性能測定結果においても、次の「ページネーション」の検証と比べたとき、最も効率がいいはずであるIDを用いた検索よりも、通常のデータの値を用いた検索の方が、はるかに高い性能が出ていることがわかります。
これは明らかに矛盾しており、それを裏付けるように、他のデータベースでは前者が後者に勝るという納得できる結果となっています。
この違いは、後者の検証では(原本に記載されている通り)下記のような適切な(セカンダリー)インデックスが作成されていることからきています。

//source{
CREATE INDEX `ycsb_address_country` ON `ycsb` (address.country)
USING GSI WITH {"nodes": [...]}
//}

=== クエリ１: ページネーション（OFFSETとLIMITによるフィルター）

@<strong>{データアクセス特性}

 * 読み取り: 100%



//table[BqrGx0u4Z6][ページネーションワークロード利用データ件数]{
クラスター構成	テストデータ
----------------------------
4 ノード	5百万件の顧客データ(4KB/1件)
10 ノード	2千5百万件の顧客データ
20 ノード	5千万件の顧客データ
//}



//image[altros_nosql_benchmark_query_1_query][ページネーションワークロード評価用クエリ]{
//}



//image[altros_nosql_benchmark_query_1_result][ページネーションワークロード測定結果]{
//}

=== クエリ２: JOIN（テーブル結合）

@<strong>{データアクセス特性}

 * 読み取り: 100%



//table[rysQN8F8mP][JOINワークロード利用データ件数]{
クラスター構成	テストデータ
----------------------------
4 ノード	5百万件の顧客データ(4KB/1件)、５百万件の受注データ(4.5KB/1件)
10 ノード	2千5百万件の顧客データ、2千5百万件の受注データ
20 ノード	5千万件の顧客データ、5千万件の受注データ
//}



//image[altros_nosql_benchmark_query_2_query][JOINワークロード評価用クエリ]{
//}



//image[altros_nosql_benchmark_query_2_result_1_0f_2][JOINワークロード測定結果(オペレーションベース)]{
//}

//image[altros_nosql_benchmark_query_2_result_2_0f_2][JOINワークロード測定結果(ドキュメントベース)]{
//}


//footnote[bbb133363925367c0a5c46eccf1cc6ef][https://www.altoros.com/]

//footnote[10933fbacf006df6472c3df2d355ed2f][https://www.altoros.com/research-papers/nosql-performance-benchmark-2018-couchbase-server-v5-5-datastax-enterprise-v6-cassandra-and-mongodb-v3-6/]

//footnote[ba11732b1ec091926ca4e319cd264f5f][https://en.wikipedia.org/wiki/YCSB]

== 結論

以下、結論部から引用します。

//quote{

Hardly any NoSQL database can perfectly fit all the requirements of any given use case.
Every solution has its advantages and disadvantages that become more or less important depending on specific criteria to meet. 

First of all, it should be noted that all the workloads were executed with the assumption that a data set fits the available memory. With that in mind, all the reads from Data Service and Index Service for Couchbase were from RAM and thus performed on in-memory speeds. 

With the same amount of available RAM, DataStax (Cassandra) did not allow to store everything in cache. Therefore, the majority of the reads were made from disk. Couchbase demonstrated good performance across all the evaluated workloads and appears to be a good choice, providing out-of-the-box functionality sufficient to handle the deployed workloads and requiring no in-depth knowledge of the database’s architecture. Furthermore, the query engine of Couchbase supports aggregation, filtering, and JOIN operations on large data sets without the need to model data for each specific query. As clusters and data sets grow in size, Couchbase ensures a satisfactory level of scalability across these operations. 

MongoDB produced comparatively decent results on relatively small clusters. MongoDB is scalable enough to handle increasing amounts of data and cluster extension. 
Under this benchmark, the one issue we observed was that MongoDB did not support JOIN operations on sharded collections out of the box. This way, dedicated data modeling provided a way out—however, with a negative impact on performance. 

Cassandra provided rather good performance for intensive parallel writes and reads by a partition key and, as expected, failed on non-clustering key-read operations. 
In general, we proved that Cassandra is able to show great performance for write-intensive operations and reads by a partition key. Still, Cassandra is operations-agnostic and behaves well only if multiple conditions are satisfied. For instance, reads are processed by a known primary key only, data is evenly distributed across multiple cluster nodes, and, finally, there is no need for joins or aggregates. 

//}

以下、上記引用からの翻訳を示します。

//quote{

NoSQLデータベースが、任意のユースケースのすべての要件に完全に適合することは稀です。
すべてのソリューションには長所と短所があり、満たすべき基準に応じて重要性が異なります。

まず、すべてのワークロードは、データセットが使用可能なメモリーに適合することを前提として実行されました。
そのため、CouchbaseのDataサービスとIndexサービスからの読み取りはすべてRAMから行われ、インメモリーの速度で実行されました。

使用可能なRAMは同じ量ながら、DataStax（Cassandra）では、すべてをキャッシュに保存することができませんでした。したがって、読み取りの大部分はディスクから行われました。
Couchbaseは、評価されたすべてのワークロードで優れたパフォーマンスを示し、適切な選択であるように伺われました。
デプロイされたワークロードを処理するのに十分な機能が提供されており、それらを特別な設定を行わずとも利用することができたため、データベースのアーキテクチャーに関する深い知識は必要ありませんでした。
さらに、Couchbaseのクエリエンジンは、特定のクエリに合わせてデータをモデル化する必要なく、大規模なデータセットに対する集計、フィルタリング、およびJOIN操作をサポートしています。
Couchbaseは、大規模のクラスターと大量のデータセットを用いた検証において、オペレーション全般について十分なレベルのスケーラビリティーを実証しました。

MongoDBは、小さなクラスターでの検証では、比較的適切な結果を生成しました。 MongoDBは、増加するデータ量をクラスターの拡張により、処理することにおいて十分スケーラブルだといえます。
このベンチマークの下で、私たちが観察したひとつの問題は、MongoDBがシャードコレクションでのJOIN操作を素のままの状態では、サポートしていないことでした。
そのため、専用のデータモデリングを行わねばならず、このことは性能に悪影響を与えました。

Cassandraは、パーティションキーによる集中的な並列書き込みと読み取りに対してかなり優れたパフォーマンスを提供し、予期されたことでしたが、非クラスタリングキー読み取り(non-clustering key-read)操作で失敗しました。
一般に、Cassandraは書き込み集中型(write-intensive)操作とパーティションキーによる読み取りに対して優れたパフォーマンスを発揮できることを証明しました。
一方、operations-agnosticな性格を持つCassandraは、複数の条件が満たされた場合にのみ正常に動作します。
たとえば、読み取りは既知の主キーによってのみ処理され、データは複数のクラスターノードに均等に分散され、そして最後に、結合や集計は必要とされていません。

//}