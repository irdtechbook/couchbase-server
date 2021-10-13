
= 運用


== 監視

Couchbase Serverにおける監視について見ていきます。

Couchbase Serverの監視機能の対象は、CPU、メモリー利用量などノード(サーバー)レベルの監視と、データ量やアクセス状況などデータベースやクラスターレベルの監視の両方を含んでいます。


=== Webコンソール

Couchbase ServerのWebコンソールは、様々な情報を期間と対象を変えながら表示することのできるダッシュボードを備えています。@<fn>{ui-monitoring-statistics}
//image[web_console_monitoring][]{
//}

//footnote[ui-monitoring-statistics][https://docs.couchbase.com/server/current/manage/monitor/ui-monitoring-statistics.html]


=== REST API

外部システムとの汎用的な連携をサポートするために、Couchbase Serverは、REST APIによる監視情報へのアクセスを提供しています。
Webコンソールで公開されている監視情報をREST APIを使って取得することが可能です。@<fn>{rest-statistics}

=== コマンドラインツール

監視システムとの連携の手法として、（エージェント型監視ツールを典型として）CLI(コマンドラインインターフェイス)が必要とされるケースもあります。Couchbase Serverは用途に応じた各種CLIツールを備えていますが、監視情報取得の用途として@<tt>{cbstats}@<fn>{cbstats-intro}があります。

//footnote[rest-statistics][https://docs.couchbase.com/server/current/rest-api/rest-statistics.html]

//footnote[cbstats-intro][https://docs.couchbase.com/server/current/cli/cbstats-intro.html]


=== アラートEメール

Couchbase Serverは、自動フェイルオーバー等の特定の状況が発生した際に、Eメールを送付する機能を持っており、Webコンソール画面またはCLI、REST APIから設定することが可能です。@<fn>{configure-alerts}

//footnote[configure-alerts][https://docs.couchbase.com/server/current/manage/manage-settings/configure-alerts.html]


=== Prometheus連携

Prometheus@<fn>{prometheus}は、監視対象にアクセスしてデータを収集するPull型アーキテクチャに基づいています。Prometheusによる監視対象サーバーは、Exporterと呼ばれるアクセスポイントを公開します。

Prometheusのサイト(Exporters and Integrations@<fn>{exporters})で、公開されているExporterのリストを見ることができます。
公開されているExporterの1つとして、Couchbase Exporter@<fn>{couchbase-exporter}を利用することが可能です。

//footnote[prometheus][https://prometheus.io/]

//footnote[exporters][https://prometheus.io/docs/instrumenting/exporters/]


//footnote[couchbase-exporter][https://github.com/couchbase/couchbase-exporter]


Couchbase Exporterを利用する際には、監視するクラスターを指定して、以下のようにプロセスを実行します。

//cmd{
./couchbase-exporter --couchbase.username Administrator --couchbase.password password --web.listen-address=":9420" --couchbase.url="http://example.host.a:8091"
//}



//footnote[GrafanaDashboard][https://github.com/couchbaselabs/blog-source-code/blob/master/Meliani/GrafanaDashboard.json]



ブログ記事「Couchbase Monitoring Integration with Prometheus and Grafana」@<fn>{couchbase-monitoring-integration-with-prometheus-and-grafana}にて、Couchbase ServerとPrometheusおよびGrafana@<fn>{grafana}との連携に関する詳細な手順が説明されています。

//footnote[couchbase-monitoring-integration-with-prometheus-and-grafana][https://blog.couchbase.com/couchbase-monitoring-integration-with-prometheus-and-grafana/]


//footnote[grafana][https://grafana.com/]

//image[grafana][]{
//}

 (画像は、 Couchbase Monitoring Integration with Prometheus and Grafana@<fn>{couchbase-monitoring-integration-with-prometheus-and-grafana}より引用)

== バックアップ

バックアップは、データベース管理において、欠かせないテーマです。

=== 概要

Couchbase Serverは、バックアップファイルの作成、バックアップファイルからの復元、バックアップファイルのアーカイブの機能を提供しています。
これらの機能は、以下どちらの方法でも利用することができます。

 * @<strong>{Backupサービス} WebコンソールまたはREST APIを用いて、スケジューリングを含めた操作が可能です。
 * @<strong>{コマンドライン操作} @<tt>{cbbackupmgr}が提供されています。



====[column]エディションによる差異
Backupサービスは、エンタープライズエディションでのみ利用可能です。

====[/column]

=== Backupサービス

本書では、コミュニティエディションに基づく記述を方針としているため、エンタープライズエディションでのみ利用可能なBackupサービスは対象外となります。

一方、本書で取り扱っていない他のサービスは、データ分析や全文検索など、ユースケースによって要否が決まり、必ずしも普遍的に必要な知識ではないのに対して、Backupサービスは、それらとは異なる性質を持ちます。

そこで、Backupサービスについては、ドキュメント参照先@<fn>{manage-backup-and-restore}を示すことで、機能の紹介に代えたいと思います。

//footnote[manage-backup-and-restore][https://docs.couchbase.com/server/current/manage/manage-backup-and-restore/manage-backup-and-restore.html]

=== コマンドラインツール

@<tt>{cbbackupmgr}コマンドを使ったバックアップから復元までの一連の操作については、コマンドライン操作に関する章で詳しく説明します。


ここでは、@<tt>{cbbackupmgr}の基本構文と(サブ)コマンドの一覧を示すため、@<tt>{-h}オプションの出力を掲載します。

//cmd{
$ cbbackupmgr -h
cbbackupmgr [<command>] [<args>]

  backup         Backup a Couchbase cluster
  collect-logs   Collect debugging information
  compact        Compact an incremental backup (Unsupported for 6.6.0+ backups)
  config         Create a new backup configuration
  help           Get extended help for a subcommand
  examine        Search inside a backup/repository for a specified document (Enterprise Edition Only)
  info           Provide information about the archive
  merge          Merge incremental backups together (Enterprise Edition Only)
  remove         Delete a backup permanently
  restore        Restore an incremental backup

Optional Flags:

     --version                Prints version information
  -h,--help                   Prints the help message
//}

====[column]エディションによる差異
上記の出力例中の「Enterprise Edition Only」という記載からも見て取れるように、@<tt>{cbbackupmgr}には、@<tt>{merge}や@<tt>{examine}サブコマンドのように、エンタープライズエディションでのみ利用できる機能があります。

====[/column]

