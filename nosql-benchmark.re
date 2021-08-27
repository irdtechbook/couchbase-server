= NoSQL性能評価：MongoDB,Cassandraとの比較



以下のNoSQLデータベースに対して実施されたベンチマークの内容を紹介します。



 * MongoDB v3.6
 * DataStax Enterprise v6 (Cassandra)
 * Couchbase Server v5.5




ここで紹介するベンチマークは、2018年にAltoros@<fn>{bbb133363925367c0a5c46eccf1cc6ef}社によって実施されたもので、下記から入手可能です。





NoSQL Performance Benchmark 2018: Couchbase Server v5.5, DataStax Enterprise v6 (Cassandra), and MongoDB v3.@<fn>{10933fbacf006df6472c3df2d355ed2f}





また、必要と思われる箇所で、Altoros社によるベンチマーク方法が不適切と考えられる箇所についての指摘を行います。








== ベンチマーク概要






=== 評価基準・ツール



Yahoo! Cloud Serving
Benchmark(YCSB)@<fn>{ba11732b1ec091926ca4e319cd264f5f}









=== ワークロード・カテゴリー






==== YSCB標準

 * ワークロード A: 更新処理
 * ワークロード E: レンジスキャン







==== クエリ



 * クエリ１：ページネーション（OFFSETとLIMITによるフィルター）
 * クエリ２：ジョイン（テーブル結合）




クエリ 1 とクエリ 2 は、それぞれ、下記のドメインとシナリオを表現。





 * 財務：フィルター処理されたトランザクションを一覧表示するためのサーバー側のページネーション
 * eコマース:顧客が利用するさまざまな製品やサービスに関する一連のレポート







=== 環境



サイズの異なる以下の３通りのクラスターで検証。





 * 4 ノード
 * 10 ノード
 * 20 ノード




@<strong>{データベース用インスタンス}






//image[altros_nosql_benchmark_server_settings_1_of_2][altros nosql benchmark server settings 1 of 2]{
//}








//image[altros_nosql_benchmark_server_settings_2_of_2][altros nosql benchmark server settings 2 of 2]{
//}







@<strong>{クライアント用インスタンス}






//image[altros_nosql_benchmark_client_settings][altros nosql benchmark client settings]{
//}











=== 共通条件
 * データサイズは、メモリーサイズと適合している状態
 * Durability（耐久性）オプション未使用
 * 各データセットに対して1 つのレプリカ（複製）




====[column]

Durability（耐久性）オプションとは、WRITE処理の際のデータの耐久性のレベルをデフォルトよりも高く設定することです。このオプションを用いた場合、レプリカの作成やディスクへの同期のために、キューを介した非同期処理を行うのではなく、レプリカ作成やディスクへの永続化の完了をもってデータ更新処理完了とされます。

====[/column]





=== データベース固有の構成






==== Couchbase Server



クラスタのサイズに関係なく、各ノードはDataサービス、Indexサービス、およびQueryサービスで構成。
バケットには、使用可能な RAM(36,178 MB) の 60% を割り当て。
Indexサービスには、使用可能なRAM の約40%
(約24GB)を割り当て。作成された各インデックスは、すべてのIndexスサービス(ノード)に複製。









==== MongoDB



MongoDBは、Routerプロセス、Configサーバー、およびデータシャードからなる階層型クラスタトポロジを採用しています。各クラスタサイズ(4、10、および 20 ノード)に対して、以下の構成が使用されています。





 * Configサーバーは、3つのメンバーからなるレプリカセットとして構成(クラスタのノード数としてはカウントしない、別のコンピュータ）
 * 各シャードは、3つのメンバー (Primary、1つのSecondary、1つのArbeiter)からなるレプリカ セットとして構成
 * 各クライアントに3つの @<tt>{mongos} （Routerプロセス）をデプロイ







==== Cassandra



下記の設定を適用






//image[altros_nosql_benchmark_cassandra_settings][altros nosql benchmark cassandra settings]{
//}













== ワークロード別詳細






=== ワークロード A: 更新処理



 * 読み取り： 50%
 * 更新： 50%
 * 4 ノード クラスタ: 5千万レコード (1レコード1KB)
 * 10 ノード クラスタ: 1 億レコード
 * 20 ノード クラスタ: 2 億レコード




@<strong>{クエリ}






//image[altros_nosql_benchmark_workload_A_query][altros nosql benchmark workload A query]{
//}







@<strong>{結果}






//image[altros_nosql_benchmark_workload_A_result][altros nosql benchmark workload A result]{
//}











=== ワークロード E: レンジスキャン



 * 読み取り： 95%
 * 更新： 5%
 * 4 ノード クラスタ: 5千万レコード (1レコード1KB)
 * 10 ノード クラスタ: 1 億レコード
 * 20 ノード クラスタ: 2 億5千万レコード







==== 本検証における問題



冒頭に記した、「Altoros社によるベンチマーク方法が不適切と考えられる箇所」は、この検証に対して行われます。以下、原文から引用します。




//quote{
Because the scan operation is performed over the primary key in
Couchbase, the following primary index has been created
//}


//source{
CREATE PRIMARY INDEX `ycsb_primary` ON `ycsb`
USING GSI WITH {"nodes": [...]\}
//}




ここには、Couchbaseのプライマリーインデックスに対する誤解が見えます。
原文で、「the primary key in Couchbase(Couchbaseのプライマリーキー)」と呼ばれているのは、実際には、キーバリューストアとしてドキュメントが格納される際のキー（ドキュメントID）であり、インデックスの概念とは異なるものです。
また、Couchbase Serverのプライマリーインデックスは、そのバケット（上記の @<tt>{yscb}）に対する、あらゆるクエリを可能にする（インデックスサービスがそのバケットに対する全件スキャンを可能にする）ためのものであって、そのキー（ドキュメントID）を使った検索を最適化するものではありません（上記のDDLでは、バケット名@<tt>{ycsb} のみが指定されていることに注意）。
後出のクエリの実行を最適化するためには、以下のように、実際にクエリで使われている@<tt>{META().id}を利用するためのインデックスを、(Couchbase Serverの概念におけるセカンダリーインデックスとして）作成する必要があります。






//emlist{
CREATE INDEX `yscb_primary` on `yscb`(META().id) using GSI;
//}







====[column]

Couchbase　ServerのN1QLクエリは、SQLをJSONへのクエリとして拡張したものです。
キーバリューストアのキーとしてのドキュメントIDは、JSONデータを構成する要素ではなく、メタデータの位置づけとなるため、上記のような `META().id` という指定が用いられます。

====[/column]



@<strong>{クエリ}






//image[altros_nosql_benchmark_workload_E_query][altros nosql benchmark workload E query]{
//}







@<strong>{結果}






//image[altros_nosql_benchmark_workload_E_result][altros nosql benchmark workload E result]{
//}












=== クエリ１：ページネーション（OFFSETとLIMITによるフィルター）



 * 読み取り：　100%
 * 4 ノード クラスタ: 5百万顧客レコード (1レコード4KB)
 * 10 ノード クラスタ: 2千5百万顧客レコード
 * 20 ノード クラスタ: 5千万顧客レコード




@<strong>{クエリ}






//image[altros_nosql_benchmark_query_1_query][altros nosql benchmark query 1 query]{
//}







@<strong>{結果}






//image[altros_nosql_benchmark_query_1_result][altros nosql benchmark query 1 result]{
//}











=== クエリ２：JOIN（テーブル結合）



 * 読み取り：　100%
 * 4 ノード クラスタ: 5百万顧客レコード、５百万受注レコード(1レコード4.5KB)
 * 10 ノード クラスタ: 2千5百万顧客レコード、2千5百万受注レコード
 * 20 ノード クラスタ: 5千万顧客レコード、5千万受注レコード




@<strong>{クエリ}






//image[altros_nosql_benchmark_query_2_query][altros nosql benchmark query 2 query]{
//}







@<strong>{結果}






//image[altros_nosql_benchmark_query_2_result_1_0f_2][altros nosql benchmark query 2 result 1 0f 2]{
//}








//image[altros_nosql_benchmark_query_2_result_2_0f_2][altros nosql benchmark query 2 result 2 0f 2]{
//}










//footnote[bbb133363925367c0a5c46eccf1cc6ef][https://www.altoros.com/]

//footnote[10933fbacf006df6472c3df2d355ed2f][https://www.altoros.com/research-papers/nosql-performance-benchmark-2018-couchbase-server-v5-5-datastax-enterprise-v6-cassandra-and-mongodb-v3-6/]

//footnote[ba11732b1ec091926ca4e319cd264f5f][https://en.wikipedia.org/wiki/YCSB]
