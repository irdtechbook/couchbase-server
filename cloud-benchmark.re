
== NoSQL(DBaaS)性能評価：Couchbase CloudとMongoDB Atlas,AWS DynamoDBとの比較


::: paragraph
各種ソフトウェア会社が、自社のサービスをクラウド上でのマネージドサービスとして提供を始める流れの中で、NoSQLデータベースについても、同様の動きが見られます。
:::



::: paragraph
そんな中、 Altoros@<fn>{bbb133363925367c0a5c46eccf1cc6ef}
社による、マネージドサービスとしてのNoSQLドキュメント指向データベース（Couchbase
Cloud, MongoDB Atlas, AWS
DynamoDB）を対象としたパフォーマンス評価の結果が、2020年末に発表されています。
:::



::: paragraph
Performance Evaluation of NoSQLDatabases as a Service with YCSB:
Couchbase Cloud,MongoDB Atlas, and AWS
DynamoDB@<fn>{b04734f3386a84d8c4a7a5e7ece70517}
:::



::: sect3


#@# header_attribute: {#_本検証における問題}
=== 本検証における問題


::: paragraph
はじめに、このペーパーを読んで、違和感を感じた部分を指摘します。その後、ベンチマーク結果について紹介します。ホワイトペーパーの内容に進む前に、まず、Couchbase
Serverのアーキテクチャー上の特徴として、以下の二点を整理します。
:::



::: ulist
-   @<strong>{Query/Indexサービスの特殊性} Couchbase
    Serverにおいて、データベースへの検索リクエストに応答する役割を担うクエリサービスと、そのQueryサービスを成立させるために使われるIndexサービスは、オプショナルなものとして、中核を担うデータサービスとは独立しており、それらを利用する必要がない場合には、「サービス」の単位で利用しない（起動していない）状態を選択することができます。

 * @<strong>{プライマリインデックスの特殊性} CouchbaseServerには（「にも」というべきでしょうか）、プライマリインデックスとセカンダリインデックスという用語があります。ただし、その意味するところは、RDBMS等の他のデータベースとは大きく異なっています。その違いは、 N1QL TuningGuide@<fn>{965c1ba46cd68ab238ba3c37fd8ff3c9}における一つのTipsに顕著に現れています。曰く「@<strong>{Never　Create aPrimary Index inProduction}」。そこでは、以下の様に書かれています。:::



::: quoteblock


//quote{
::: paragraph
プライマリインデックス・スキャンは、RDBにおけるフルテーブル・スキャンに相当します。
:::
:::

//}


::: paragraph
@<b>{Couchbase
Serverのプライマリインデックス}は、「バケット」単位で作成する、特定の「キー」を指定しない、インデックスです。
:::



::: paragraph
対して、@<b>{Couchbase
Serverのセカンダリインデックス}は、検索条件となる（そして特には検索対象データともなる）「フィールド」を「キー」として（時に複数）使い、構築するものです。
:::



::: paragraph
「決して本番で使うべからず」と言われているプライマリインデックスの存在意義は、開発時にインデックスの設計・作成・適正化を実施する前であっても、クエリを試してみることを可能にする、というただそれだけのためにあります。
:::



::: paragraph
ホワイトペーパーの記述に関して違和感を感じたは、レンジスキャンの要件に関わる部分（Workload
E: Scanning short ranges）です。
下記がこの検証に用いられた評価クエリの一覧です。
:::



::: imageblock
::: content
//image[altros_dbaas_benchmark_workload_E_query][altros dbaas benchmark workload E
query]{
//}

:::
:::



::: paragraph
Couchbase
Serverに対するアクセスは、「キー」を用いたものですが、@<b>{N1QL}（というクエリ言語）を用いて実行されています。つまり、QueryサービスとIndexサービスに依存しています。
Couchbase
Serverで「キー」のみによるレンジスキャンを行うのであれば、下記の様なAPI呼び出し側での制御を行うことが考えられます。他のデータベースとの比較の意味でも妥当（Apple-to-Appleな比較）とさえ言えます（そのデータベースが備える最善の選択を行うという意味で。例えば、ワークロードAでは、Dataサービスへの直接アクセスが選択されています）。
:::



::: paragraph
@<strong>{同期型の例}
:::



::: listingblock
::: content
@<tt>{highlight
    protected List<Object> get() {
        for (String key : keys) {
            GetResult result = collection.get(String.valueOf(key));
            listResults.add(result);
        \}
        return listResults;
    \}
}
:::
:::



::: paragraph
@<strong>{非同期の例}
:::



::: listingblock
::: content
``` highlight
    protected List<Object>  get() {
        ReactiveCollection reactiveCollection = collection.reactive();


//emlist{
    Flux<Object> resultFlux = Flux.fromArray(keys.toArray())
        .flatMap( k -> reactiveCollection.get(String.valueOf(k)));

    List<Object> listResults = resultFlux.collectList().block();
    return listResults;
}
//}

//emlist{
:::
:::

::: paragraph
確かに、上記のコードのみでは、`id`
が、ある値以上のものを特定件数、取得する、という要件を、実現することはできていません。
（この要件は、上記コード外で、`id` のリストを作成し、`keys`
リストに与えることで実現されると想定されています）
:::

::: paragraph
MongoDBは、キーのシャーディングのために、「レンジ」と「ハッシュ」を選択する仕様となっています。その上で、この性能比較では、「レンジ」が採用されています（ホワイトペーパー中に下記、説明されている通り）。
:::

::: quoteblock
> ::: paragraph
> MongoDB Atlas distributes data using a shard key. There are two types
> of shard keys supported by this database: range- and hash-based. The
> range-based partitioning supports more efficient range queries. Given
> a range query on a shard key, a query router can easily determine
> which chunks overlap this range and routes the query to only those
> shards that contain such chunks. However, the range-based partitioning
> can result in an uneven data distribution, which may negate some of
> the benefits of sharding.
> :::
:::

::: paragraph
一方で、Couchbase
Serverでは、シャーディングのロジックは、ハッシュに統一されています。
一連のデータへのアクセスのために、レンジパーティションに比べて、多くのノードへアクセスしなければならないという条件を持ちながら、Couchbase
Serverでは、なぜハッシュによるシャーディングが採用されているかと言えば、それは、Couchbase
Serverのアーキテクチャーにおいて、クライアントが、キーにより特定されるドキュメントを管理しているノードへ直接アクセスすることができるメカニズム（スマートクライアント）を持っているからです。また、（上記にコード例で示した）非同期処理による恩恵も受けることができます。
:::

::: imageblock
::: content
![altros dbaas benchmark workload E
result](images/altros_dbaas_benchmark_workload_E_result.png)
:::
:::

::: paragraph
ここでの主張は、Queryサービスを介さずに、DataサービスのAPIを直接使うことによって、上記の結果よりも、格段に良い結果が導かれるはずだというものでした。
さらに言えば、ここで扱っているホワイトペーパーには、詳細が明記されていないため、あえて直接的に触れませんでしたが、クエリでの実行を前提とした場合にも、適切なインデックスを用いることで、性能の最適化を行う余地があると考えることができます（不適切なインデックスを使っている証拠は、ここで扱っているホワイトペーパーの中に見つけることができませんでしたが、同社の別のホワイトペーパーについて、本書の別の箇所でその点を指摘しています）。
例えば、レンジスキャンのワークロード以外では、結果に、下記の様な明らかな違いが生じています。
:::

::: paragraph
このワークロードAは、キーを直接指定したアクセスを比較したもので、この性能の比較は、「メモリファーストアーキテクチャー」により導き出されたと言えるでしょう。
:::

::: imageblock
::: content
![altros dbaas benchmark workload A
result](images/altros_dbaas_benchmark_workload_A_result.png)
:::
:::

::: paragraph
下記のJOINワークロードは、複数ドキュメントを結合するケースを比較したもので、セカンダリーインデックスを利用したQuery/Indexサービスの利点を生かした結果となっています。
:::

::: imageblock
::: content
![altros dbaas benchmark JOIN workload
result](images/altros_dbaas_benchmark_JOIN_workload_result.png)
:::
:::

::: paragraph
このように、このホワイトペーパーには、Couchbase
Server内部のアーキテクチャーへの洞察があれば、より適切な検証方法を選択できたはずだと思える部分がありました。
:::
:::

::: sect3
#### 総合評価 {#_総合評価}

::: paragraph
以下に、総合評価について原文を引用し、翻訳を附します。興味を持たれた方は、是非、ホワイトペーパー自体をご参照ください。
:::

::: quoteblock
> ::: paragraph
> Couchbase Cloud showed better performance across all the evaluated
> workloads in comparison to other databases. In case of queries,
> Couchbase Cloud provides sufficient functionality to handle the
> deployed workloads. Furthermore, the query engine of Couchbase Cloud
> supports aggregation, filtering, and JOIN operations on large data
> sets without the need to model data for each specific query. As
> clusters and data sets grow in size, Couchbase Cloud ensures a
> satisfactory level of scalability across these operations.
> :::
:::

::: paragraph
Couchbase
Cloudは、評価されたすべてのワークロードにおいて、他のデータベースと比較して、優れたパフォーマンスを示しました。クエリのケースでは、Couchbase
Cloudは、デプロイされたワークロードを処理するために十分な機能を提供しています。さらに、Couchbase
Cloudのクエリエンジンは、大規模なデータセットの集計、フィルタリング、およびJOIN操作をサポートしており、特定のクエリ毎に合わせてデータをモデル化する必要がありません。クラスターとデータセットのサイズが大きくなった場合にも、CouchbaseCloudはこれらの操作全てに渡って、十分なレベルのスケーラビリティを保証します。
:::

::: quoteblock
> ::: paragraph
> MongoDB Atlas produced comparatively decent results. MongoDB is
> scalable enough to handle increasing amounts of data and cluster
> extension. Under this benchmark, the only issue we observed was that
> MongoDB did not support JOIN operations on sharded collections out of
> the box. This resulted in a negative impact and poor performance in
> JOIN Workload.
> :::
:::

::: paragraph
MongoDB Atlasは、比較的にいって、かなり良い結果を生み出しました。
MongoDBは、増加するデータ量とクラスターの拡張に対応できるだけ十分にスケーラブルであるといえます。このベンチマークでは、MongoDBがシャードコレクションのJOIN操作を、アウト・オブ・ボックスでサポートしていないことが唯一の問題でした。そのため、JOINワークロードに悪影響とパフォーマンスの低下が生じました。
:::

::: quoteblock
> ::: paragraph
> Amazon DynamoDB is significantly different from the other databases,
> because it looks like a pure service without proper tuning. Only two
> parameters can be changed:read and write capacities. In this case,
> read and write capacities have been calculated depending on the cost
> of other databases for each workload. Unfortunately, Amazon DynamoDB
> did not provide competitive results. It produces a large volume of
> failed requests. Additionally, Amazon DynamoDB did not take part in
> several workloads, because the data model had to be changed to get
> competitive results and, thus, cannot be compared to other databases.
> :::
:::

::: paragraph
Amazon
DynamoDBは、他のデータベースとは著しく異なっています。というのも、それはあたかも純粋なサービスであり、適切なチューニングとは関係のないように見受けられます。変更できるパラメーターは、読み取り容量と書き込み容量の2つだけです。今回のケースでは、読み取りおよび書き込み容量は、ワークロードごとの他のデータベースのコストに応じて計算されました。残念ながら、Amazon
DynamoDBは競争力のある結果を提示しませんでした。また、大量のリクエストの失敗を生み出しました。さらに、Amazon
DynamoDBでは、競争力のある結果を得るためには、データモデルを変更する必要があり、他のデータベースと比較できないため、いくつかのワークロード評価に含めることができませんでした。
:::
:::
:::
:::

::: {#footer}
::: {#footer-text}
Last updated 2021-07-01 08:58:44 +0900
:::
:::
//}

//footnote[bbb133363925367c0a5c46eccf1cc6ef][https://www.altoros.com/]

//footnote[b04734f3386a84d8c4a7a5e7ece70517][https://resources.enterprisetalk.com/ebook/Couchbase-Q3-EN-4.pdf]

//footnote[965c1ba46cd68ab238ba3c37fd8ff3c9][https://developer.couchbase.com/resources/best-practice-guides/n1ql-tuning-guide.pdf]
