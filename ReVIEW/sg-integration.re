= Sync Gatewayシステム連携


== ロードバランサー/リバースプロキシ

=== 概要

Sync GatewayとCouchbase Serverは、それぞれのアーキテクチャー上の役割に応じた異なるネットワーク環境にデプロイされることが想定されます。
これは、論理的に以下のように説明することができます。
Sync Gatewayは、エンドユーザーが利用するモバイルアプリからのリクエストを受け付けるという役割上、インターネットからのアクセスを受け付けるアプリケーション層に配置されます。
Couchbase Serverは、インターネットから直接アクセスされる必要がないデータベース層に配置されます。
一方、Sync GatewayとCouchbase Server間のコミュニケーションにおいて最適なパフォーマンスを実現するには、それらを互いに近接させておくことも重要になります。


Couchbase Serverは、分散アーキテクチャーを備えたNoSQLデータベースとしてそれ自体冗長性を備えています。
Sync Gatewayでは、複数ノードからなるクラスター構成を実現する場合には、クライアントからのアクセスを仲介するロードバランサー(Load Balancer@<fn>{load-balancer})を用います。


//footnote[load-balancer][https://docs.couchbase.com/sync-gateway/current/load-balancer.html]


また、リバースプロキシを用いて、Sync GatewayとCouchbase Liteアプリケーションとの接続を仲介するケースもあります。


=== ロードバランサー/リバースプロキシ利用の利点

ロードバランサー/リバースプロキシ利用の利点を以下に整理します。

//blankline

 * クライアントからのリクエスト負荷を複数のSync Gatewayノードに分散します。
 * インターネットに公開されているサービスから、Sync Gatewayサーバーを隠すことによって保護します。
 * アプリケーションファイアウォール機能によって、一般的なWebベースの攻撃から保護します。
 * Sync GatewayからSSLターミネーションをオフロードすることによって、多数のモバイルデバイスをサポートする場合のオーバーヘッドを分散します。
 * 各着信要求のURLを書き換えます。たとえば、Sync GatewayのパブリックREST APIポートを外部公開用のポートにマップします。

//blankline

以下では、Sync Gatewayでロードバランサー/リバースプロキシを使用する際の考慮点や参照可能な情報を紹介します。

=== WebSocket接続における考慮点

Couchbase LiteはSync Gatewayに対してWebSocket接続を開いたままにするために、デフォルトで300秒(5分)ごとにWebSocket PINGメッセージ(ハートビート)を送信します。

ハートビートを成立させるためには、介在するロードバランサーのキープアライブタイムアウト間隔がハートビート間隔より長い必要があります。そのため、必要に応じてロードバランサーのキープアライブタイムアウト間隔を設定します。または、Couchbase Liteのハートビート間隔を変更することもできます。


=== NGINX連携

Sync Gatewayのフロントエンドとして、NGINX@<fn>{nginx}を利用する方法が、Sync Gatewayのドキュメント@<fn>{load-balancer-nginx}で解説されています。


//footnote[load-balancer-nginx][https://docs.couchbase.com/sync-gateway/current/load-balancer.html#nginx]
//footnote[nginx][https://www.nginx.co.jp/]


Sync Gatewayとクライアント間の接続を保護するには、トランスポート層セキュリティー(TLS)を使用します。
NGINXとSync Gatewayの間の接続で、X.509証明書を用いたTLSを有効化する方法について、ドキュメントで解説されています。


== メトリクスREST APIによるシステム監視

=== 概要

Sync Gatewayは、システム統計情報監視(Stats Monitoring@<fn>{stats-monitoring})のためのメトリクスREST API(Metrics REST API@<fn>{rest-api-metrics})を提供しています。
メトリクスREST APIを用いて、リソース使用率や性能、サービス正常性に関する情報が収集可能です。

//footnote[stats-monitoring][https://docs.couchbase.com/sync-gateway/current/stats-monitoring.html]
//footnote[rest-api-metrics][https://docs.couchbase.com/sync-gateway/current/rest-api-metrics.html]

メトリクスREST APIは、JSONフォーマットで情報を提供する他、Prometheus@<fn>{prometheus-io}との連携に用いることも可能です。
Prometeusとの連携についてはドキュメント(Integrate Prometheus@<fn>{stats-prometheus})を参照ください。
また、Couchbase Blog: Monitor Couchbase Sync Gateway with Prometheus and Grafana@<fn>{monitoring-sync-gateway-prometheus-grafana}が公開されています。

メトリクスREST APIが提供する統計情報は、ノードが再起動するたびにリセットされます。
つまり、合計値、カウント、平均値はノードが起動された後の状況を反映した値になります。

//footnote[prometheus-io][https://prometheus.io/]

=== 統計情報カテゴリー

統計情報のカテゴリー構造は、以下のように整理できます。

//image[monitoring_1][メトリクスAPI統計情報カテゴリー構造]{
//}

(図は、Couchbase Blog: Monitor Couchbase Sync Gateway with Prometheus and Grafana@<fn>{monitoring-sync-gateway-prometheus-grafana}より引用)

//footnote[monitoring-sync-gateway-prometheus-grafana][https://blog.couchbase.com/monitoring-sync-gateway-prometheus-grafana/]

//blankline

統計情報スキーマの詳細はドキュメント@<fn>{sync-gateway-stats-schema}を参照ください。

//footnote[sync-gateway-stats-schema][https://docs.couchbase.com/sync-gateway/current/stats-monitoring.html#sync-gateway-stats-schema]


=== エンドポイント

メトリクスREST APIには、以下のようなふたつのエンドポイントがあり、JSONまたはPrometheus形式のいずれかのレポートフォーマットでデータを受け取ることができます。

//blankline

 * @<tt>{_expvars}: JSONフォーマット
 * @<tt>{_metrics}: Prometheusフォーマット

//blankline

メトリクスREST APIのデフォルトのポート番号は4986です。

以下は、@<tt>{_expvar}エンドポイントに対してリクエストを行う例です。


//cmd{
$ curl -X GET "http://localhost:4986/_expvar" -H "accept: application/json" \
  -H 'authorization: Basic QWRtaW5pc3RyYXRvcjpwYXNzd29yZA=='
//}



//footnote[stats-prometheus][https://docs.couchbase.com/sync-gateway/current/stats-prometheus.html]
//footnote[monitoring-sync-gateway-prometheus-grafana][https://blog.couchbase.com/monitoring-sync-gateway-prometheus-grafana/]


=== APIエクスプローラー


メトリクスREST APIについて、以下のようなAPIエクスプローラー@<fn>{api-explorer-metrics}が公開されています。
APIエクスプローラーは、すべてのエンドポイントを機能別にグループ化しています。
ラベルをクリックすると展開され、それぞれのエンドポイントに関する詳細を確認できます。


//image[metrics-rest-api][メトリクスREST APIエクスプローラー]{
//}


//footnote[api-explorer-metrics][https://docs.couchbase.com/sync-gateway/current/rest-api-metrics.html#api-explorer]




== パブリックREST APIによるアプリケーション連携

=== 概要

Sync Gatewayは、Couchbase Serverデータベースに対するドキュメント操作などの機能を提供する、パブリックREST APIを持ちます。
このパブリックREST APIを、他のアプリケーションとの連携の目的で利用することができます。

パブリックREST APIの利用はアプリケーションの要件次第であり、Couchbase LiteとCouchbase Serverの同期のためだけに、Sync Gatewayを利用するためには必ずしも必要ありません。

パブリックREST APIのデフォルトのポート番号は4984です。

パブリックREST APIを利用する場合の留意事項、その他詳細はドキュメント@<fn>{public-rest-api}を参照ください。

//footnote[public-rest-api][https://docs.couchbase.com/sync-gateway/current/rest-api.html]

=== APIエクスプローラー


パブリックREST APIについても、以下のようなAPIエクスプローラー@<fn>{api-explorer-public}が公開されています。


//image[public-rest-api][パブリックREST APIエクスプローラー]{
//}


//footnote[api-explorer-public][https://docs.couchbase.com/sync-gateway/current/rest-api.html#api-explorer]
