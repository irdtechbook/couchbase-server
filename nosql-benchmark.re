= NoSQL性能評価：MongoDB,Cassandraとの比較

以下のNoSQLデータベースに対して実施されたベンチマークの内容を紹介します。

 * MongoDB v3.6
 * DataStax Enterprise v6 (Cassandra)
 * Couchbase Server v5.5

ここで紹介するベンチマークは、2018年にAltoros@<fn>{bbb133363925367c0a5c46eccf1cc6ef}社によって実施されたもので、下記から入手可能です。

NoSQL Performance Benchmark 2018: Couchbase Server v5.5, DataStax Enterprise v6 (Cassandra), and MongoDB v3.@<fn>{10933fbacf006df6472c3df2d355ed2f}

また、Altoros社によるベンチマーク方法が不適切と考えられる箇所について、適宜、指摘を行います。

== ベンチマーク概要

=== 評価基準・ツール

Yahoo! Cloud Serving
Benchmark(YCSB)@<fn>{ba11732b1ec091926ca4e319cd264f5f}

=== YSCB標準ワークロード

 * ワークロード A: 更新処理
 * ワークロード E: レンジスキャン

=== クエリ種類

 * クエリ１：ページネーション（OFFSETとLIMITによるフィルター）
 * クエリ２：ジョイン（テーブル結合）

クエリ 1 とクエリ 2 は、それぞれ、下記のドメインとシナリオを表現。

 * 財務：フィルター処理されたトランザクションを一覧表示するためのサーバー側のページネーション
 * eコマース:顧客が利用するさまざまな製品やサービスに関する一連のレポート

=== クラスター構成

サイズの異なる以下の３通りのクラスターで検証を実施。

 * 4 ノード
 * 10 ノード
 * 20 ノード

=== サーバーハードウェアスペック

 * インスタンスタイプ: i3.2xlarge (Storage Optimized)
 * vCPUs: 8
 * Memory(GiB): 61
 * Instance storage(GB): 1 x 1,900 (SSD)
 * Network Performance: Up to 10 GB
 * Operating System: Ubuntu 16.04 LTS


=== 共通条件
 * データサイズは、メモリーサイズと適合している状態
 * Durability(耐久性)オプション未使用
 * 各データセットに対して1 つのレプリカ(複製)

====[column]Durability（耐久性）オプション

Durability(耐久性)オプションとは、WRITE処理の際のデータの耐久性のレベルをデフォルトよりも高く設定することです。
このオプションを用いた場合、レプリカの作成やディスクへの同期のために、キューを介した非同期処理を行うのではなく、レプリカ作成やディスクへの永続化の完了と同期的に処理完了とされます。

====[/column]

=== データベース固有の構成

@<strong>{Couchbase Server}は、本番環境ではノード毎に異なるサービスを割り当てることが一般的ですが、ここでは以下の簡略化された構成が用いられています。

 * クラスタのサイズに関係なく、各ノードはDataサービス、Indexサービス、およびQueryサービスで構成。
 * バケットには、使用可能な RAM(36,178 MB) の 60% を割り当て。
 * Indexサービスには、使用可能なRAM の約40%
 * (約24GB)を割り当て。作成された各インデックスは、すべてのIndexスサービス(ノード)に複製。

@<strong>{MongoDB}は、Routerプロセス、Configサーバー、およびデータシャードからなる階層型クラスタートポロジーを採用しています。各クラスタサイズ(4、10、および 20 ノード)に対して、以下の構成が使用されています。

 * Configサーバーは、3つのメンバーからなるレプリカセットとして構成(クラスタのノード数としてはカウントしない、別のコンピュータ）
 * 各シャードは、3つのメンバー (Primary、1つのSecondary、1つのArbeiter)からなるレプリカ セットとして構成
 * 各クライアントに3つの @<tt>{mongos} （Routerプロセス）をデプロイ

@<strong>{Cassandra}のクラスター構成については、ノードによる差異はなく、全てのノードに共通の設定を適用しており、設定項目に関してはCassandra固有の性格が強いため、割愛します。


== ワークロード別詳細

=== ワークロード A: 更新処理

 * 読み取り： 50%
 * 更新： 50%
 * 4 ノード クラスタ: 5千万レコード (1レコード1KB)
 * 10 ノード クラスタ: 1 億レコード
 * 20 ノード クラスタ: 2 億レコード

@<strong>{クエリ}

//image[altros_nosql_benchmark_workload_A_query][ワークロード A 評価用クエリ]{
//}

@<strong>{結果}

//image[altros_nosql_benchmark_workload_A_result][ワークロード A 測定結果]{
//}

=== ワークロード E: レンジスキャン

 * 読み取り： 95%
 * 更新： 5%
 * 4 ノード クラスタ: 5千万レコード (1レコード1KB)
 * 10 ノード クラスタ: 1 億レコード
 * 20 ノード クラスタ: 2 億5千万レコード

@<strong>{クエリ}

//image[altros_nosql_benchmark_workload_E_query][ワークロード E 評価用クエリ]{
//}

@<strong>{結果}

//image[altros_nosql_benchmark_workload_E_result][ワークロード E 測定結果]{
//}


冒頭に記した、「Altoros社によるベンチマーク方法が不適切と考えられる箇所」は、ワークロードEに関係しています。
ホワイトペーパー中の解説では、以下のインデックスを利用したとされています。


//source{
CREATE PRIMARY INDEX `ycsb_primary` ON `ycsb`
USING GSI WITH {"nodes": [...]\}
//}

ここには、Couchbaseのプライマリーインデックスに対する誤解が見えます。
原文中には、「スキャン操作はCouchbaseの主キーに対して実行されるため(Because the scan operation is performed over the primary key in Couchbase)」このプライマリーインデックスが作成された、とされています。しかしながら「the primary key in Couchbase(Couchbaseの主キー)」の役割を果たすのは、実際には、キーバリューストアとしてドキュメントが格納される際のキー（ドキュメントID）であり、プライマリーインデックスの概念とは異なるものです。Couchbase Serverのプライマリーインデックスは、そのキースペース（この検証では、@<tt>{yscb}バケット）に対する、ドキュメントの特定のフィールドに限定されない、あらゆるクエリを可能にする（Indexサービスがそのキースペースに対する全件スキャンを行うことを可能にする）ためのものであって、そのキー（ドキュメントID）を使った検索を最適化するものではありません。
後出のクエリの実行を最適化するためには、以下のように、実際にクエリで用いられている@<tt>{META().id}を指定したインデックスを作成する必要があります。

//emlist{
CREATE INDEX `yscb_primary` on `yscb`(META().id);
//}


=== クエリ１：ページネーション（OFFSETとLIMITによるフィルター）

 * 読み取り：　100%
 * 4 ノード クラスタ: 5百万顧客レコード (1レコード4KB)
 * 10 ノード クラスタ: 2千5百万顧客レコード
 * 20 ノード クラスタ: 5千万顧客レコード

@<strong>{クエリ}

//image[altros_nosql_benchmark_query_1_query][ページネーションワークロード評価用クエリ]{
//}

@<strong>{結果}

//image[altros_nosql_benchmark_query_1_result][ページネーションワークロード測定結果]{
//}

=== クエリ２：JOIN（テーブル結合）

 * 読み取り：　100%
 * 4 ノード クラスタ: 5百万顧客レコード、５百万受注レコード(1レコード4.5KB)
 * 10 ノード クラスタ: 2千5百万顧客レコード、2千5百万受注レコード
 * 20 ノード クラスタ: 5千万顧客レコード、5千万受注レコード

@<strong>{クエリ}

//image[altros_nosql_benchmark_query_2_query][JOINワークロード評価用クエリ]{
//}

@<strong>{結果}

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

First of all, it should be noted that all the workloads were executed with the assumption that a data set fits the available memory. 
With that in mind, all the reads from Data Service and Index Service for Couchbase were from RAM and thus performed on in-memory speeds. 

With the same amount of available RAM, DataStax (Cassandra) did not allow to store everything in cache.
Therefore, the majority of the reads were made from disk. 
Couchbase demonstrated good performance across all the evaluated workloads and appears to be a good choice, providing out-of-the-box functionality sufficient to handle the deployed workloads and requiring no in-depth knowledge of the database’s architecture. Furthermore, the query engine of Couchbase supports aggregation, filtering, and JOIN operations on large data sets without the need to model data for each specific query. 
As clusters and data sets grow in size, Couchbase ensures a satisfactory level of scalability across these operations. 

MongoDB produced comparatively decent results on relatively small clusters. 
MongoDB is scalable enough to handle increasing amounts of data and cluster extension. 
Under this benchmark, the one issue we observed was that MongoDB did not support JOIN operations on sharded collections out of the box. This way, dedicated data modeling provided a way out—however, with a negative impact on performance. 

Cassandra provided rather good performance for intensive parallel writes and reads by a partition key and, as expected, failed on non-clustering key-read operations. 
In general, we proved that Cassandra is able to show great performance for write-intensive operations and reads by a partition key. 
Still, Cassandra is operations-agnostic and behaves well only if multiple conditions are satisfied. For instance, reads are processed by a known primary key only, data is evenly distributed across multiple cluster nodes, and, finally, there is no need for joins or aggregates. 

//}

以下、上記引用からの翻訳を示します。

//quote{

NoSQLデータベースが、任意のユースケースのすべての要件に完全に適合することは稀です。
すべてのソリューションには長所と短所があり、満たすべき基準に応じて重要性が異なります。

まず、すべてのワークロードは、データセットが使用可能なメモリに適合することを前提として実行されたことにご留意ください。
そのため、Couchbaseのデータサービスとインデックスサービスからの読み取りはすべてRAMから行われ、インメモリの速度で実行されました。

使用可能なRAMは同じ量ながら、DataStax（Cassandra）では、すべてをキャッシュに保存することができませんでした。したがって、読み取りの大部分はディスクから行われました。
Couchbaseは、評価されたすべてのワークロードで優れたパフォーマンスを示し、適切な選択であるように伺われました。
デプロイされたワークロードを処理するのに十分な機能が提供されており、それらを特別な設定を行わずとも利用することができたため、データベースのアーキテクチャに関する深い知識は必要ありませんでした。
さらに、Couchbaseのクエリエンジンは、特定のクエリごとにデータをモデル化する必要なしに、大規模なデータセットに対する集計、フィルタリング、およびJOIN操作をサポートしています。
Couchbaseは、大規模のクラスターと大量のデータセットを用いた検証において、操作全般について十分なレベルのスケーラビリティが保証されました。

MongoDBは、小さなクラスターでの検証では、比較的適切な結果を生成しました。 MongoDBは、増加するデータ量をクラスターの拡張により処理することにおいて十分スケーラブルだといえます。
このベンチマークの下で、私たちが観察した1つの問題は、MongoDBがシャードコレクションでのJOIN操作を、素のままの状態では、サポートしていないことでした。
そのため、専用のデータモデリングを行わねばならず、このことは性能に悪影響を与えました。

Cassandraは、パーティションキーによる集中的な並列書き込みと読み取りに対してかなり優れたパフォーマンスを提供し、予期されたことでしたが、非クラスタリングキー読み取り(non-clustering key-read)操作で失敗しました。
一般に、Cassandraは、書き込み集中型(write-intensive)操作とパーティションキーによる読み取りに対して優れたパフォーマンスを発揮できることを証明しました。
一方、operations-agnosticな性格を持つCassandraは複数の条件が満たされた場合にのみ正常に動作します。
たとえば、読み取りは既知の主キーによってのみ処理され、データは複数のクラスターノードに均等に分散され、そして最後に、結合や集計は必要とされていません。

//}