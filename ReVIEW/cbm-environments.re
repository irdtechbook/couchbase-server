= Couchbase Mobile環境構築


Couchbase Mobile環境構築の概要を記します。

== Sync Gatewayインストール

はじめに、Sync Gatewayのインストール(Install Sync Gateway@<fn>{get-started-install})について説明します。

//footnote[get-started-install][https://docs.couchbase.com/sync-gateway/current/get-started-install.html]

=== 入手方法


各種Linux系OSや、Windows、macOS用のSyncGatewayをCouchbaseのサイト@<fn>{downloads-extend-with-mobile}からダウンロードできます。
コミュニティエディションとエンタープライズエディションの両方をここから入手可能です。

サポートされるOSの詳細については、ドキュメント(Supported Operating Systems@<fn>{supported-operating-systems})を参照ください。

//footnote[supported-operating-systems][https://docs.couchbase.com/sync-gateway/current/get-started-prepare.html#supported-operating-systems]


=== インストール

Red Hat/CentOSの場合は、RPM(Red Hat Package Manager)を使って、以下のようにインストールできます。

//cmd{
$ rpm -i couchbase-sync-gateway-community_3.0.0_x86_64.rpm
//}

UbuntuとDebianについては、@<tt>{dpkg}コマンドを使って、以下のようにインストールします。

//cmd{
$ dpkg -i couchbase-sync-gateway-community_3.0.0_x86_64.deb
//}

//footnote[downloads-extend-with-mobile][https://www.couchbase.com/downloads#extend-with-mobile]


macOSの場合、ダウンロードしたファイルを解凍して利用します。
ドキュメントの記載は、@<tt>{/opt}ディレクトリに解凍して利用することを想定したものになっています。


//cmd{
$ sudo unzip couchbase-sync-gateway-community_3.0.0_x86_64.zip -d /opt
//}


Windowsでは、インストーラーを用いてインストールします。
Windoesへのインストールの詳細については、ドキュメント(Install for Windows@<fn>{install-for-windows})を参照ください。
この後の記載では、Windowsについての説明は省略します。

//footnote[install-for-windows][https://docs.couchbase.com/sync-gateway/current/get-started-install.html#install-for-windows]

=== インストールロケーション

デフォルトのSync Gatewayインストールでは、次のロケーションが使用されます。

//table[tbl1][]{
コンテンツ	ロケーション	実例
-----------------
バイナリ	インストールディレクトリ	@<tt>{/opt/couchbase-sync-gateway/bin/}
サンプル構成ファイル	インストールディレクトリ	@<tt>{/opt/couchbase-sync-gateway/examples/}
スクリプト	インストールディレクトリ	@<tt>{/opt/couchbase-sync-gateway/service/}
サービス(Mac)	ライブラリディレクトリ	@<tt>{<LIBRARY_ROOT>/com.couchbase.mobile.sync_gateway.plist}
サービス(Linux)	ライブラリディレクトリ	@<tt>{<LIBRARY_ROOT>/sync_gateway.service}
//}

@<tt>{<LIBRARY_ROOT>}は、macOSの場合@<tt>{/Library/LaunchDaemons}、Linuxの場合@<tt>{/lib/systemd/system}または、@<tt>{/usr/lib/systemd/system}です。

また、構成ファイルとログファイルのデフォルトのロケーションは、以下の通りです。

 * 構成ファイル: @<tt>{<HOME_ROOT>/sync_gateway/}
 * ログファイル: @<tt>{<HOME_ROOT>/sync_gateway/logs/}


@<tt>{<HOME_ROOT>}は、macOSの場合@<tt>{/Users}、Linuxの場合@<tt>{/home}です。

== Sync Gateway実行

Sync Gatewayは、サービスとして実行することも、コマンドラインから直接実行することもできます。

=== コマンドライン実行

インストールディレクトリに解凍されたバイナリファイル(@<tt>{sync_gateway})に対して、実行時引数としてブートストラップ構成ファイルのパスを指定して実行することができます。

特定のシャットダウン手順はなく、SIGINTシグナル(Ctrl+C)を使用して安全に停止することができます。

コマンドライン実行時のコマンドラインオプションの詳細については、ドキュメント(Using the Command Line@<fn>{command-line-options})を参照ください。

//footnote[command-line-options][https://docs.couchbase.com/sync-gateway/current/command-line-options.html]


=== サービス実行

Linux環境で、@<tt>{systemd}を利用する場合のサービスの開始と停止は以下のようになります。

//cmd{
$ systemctl start sync_gateway
$ systemctl stop sync_gateway
//}

Linux環境で、@<tt>{init}を利用している場合は、以下のようになります。

//cmd{
$ service sync_gateway start
$ service sync_gateway stop
//}


macOSでサービスとして実行するための手順についてはドキュメント@<fn>{lbl-service}を参照ください。

//footnote[lbl-service][https://docs.couchbase.com/sync-gateway/current/get-started-install.html#lbl-service]


== Couchbase Server環境

=== 概要

Sync Gatewayを含めた、Couchbase Mobile環境を構築する際には、Couchbase ServerをSync Gateway用にセットアップします。

=== クラスター構成

Couchbase Serverをクラスターとしてセットアップする際には、要件に応じて、利用するサービスの有効化およびクラスター内での配置を実施します。

Sync Gatewayと共に利用するCouchbase Serverクラスターでは、以下のサービスが稼働している必要があります。

//blankline

 * Dataサービス
 * Indexサービス
 * Queryサービス

=== バケット

Couchbase Serverでは、データはバケットと呼ばれる論理エンティティに格納されます。
バケットには、データをメモリとディスクの両方に保持するCouchbaseバケットと、データをメモリにのみ保持するEphemeralバケットとがあります。
Couchbase Liteとデータ同期を行うリモートデータベースとして利用する場合には、Couchbaseバケットを利用します。

Couchbase Liteと同期されるバケットには、Sync Gatewayのメタデータも保存されます。


=== Couchbase Serverユーザー

Sync GatewayからCouchbase Serverにアクセスするため、Couchbase Serverユーザーを、適切なRBAC(Role Based Access Control)の元、構成する必要があります。
ユーザーは、同期対象のバケットに対する適切なアクセス権限を持っている必要があります。
このユーザーは、Sync GatewayからCouchbase Serverへの接続に用いられます。

Sync Gatewayの管理REST APIとメトリクスREST APIへアクセスする際のユーザーは、Couchbase Serverユーザーです。
つまり、Couchbase Serverのアクセス権限管理に従います。
要件に応じて、適切な権限が付与された専用のユーザーを用います。

なお、パブリックREST APIへのアクセスや、Couchbase Liteアプリケーション(レプリケーター)がSync Gatewayに接続する際に用いるユーザーは、Couchbase Serverユーザーではなく、Sync Gatewayユーザーです。
これらのユーザー情報はSync Gatewayメタデータの一部という位置づけにて、Couchbase Serverのバケットにドキュメントとして格納されます。


=== 開発参考情報

Sync Gatewayを利用するために必要となるCouchbase Serverの構成方法の詳細については、ドキュメント(Configure Server for Sync Gateway@<fn>{configure-server})を参照してください。

また、Couchbase Serverのインストールや操作方法についてはCouchbase Serverのドキュメント@<fn>{doc-couchbase-server}を参照ください。
特に、Sync Gatewayの管理REST APIへアクセスするユーザーを構成する際に必要となる、Couchbase ServerのRBAC(Role-Based Access Control)については、承認(Authorization@<fn>{authorization-overview})に関するドキュメントを参照ください。

また、Couchbase Serverに関する日本語の情報として、拙著『NoSQLドキュメント指向データベースCouchbase Serverファーストステップガイド』(インプレスR&D、2021年)を参照いただくことができます。

//footnote[configure-server][https://docs.couchbase.com/sync-gateway/current/get-started-prepare.html#configure-server]
//footnote[doc-couchbase-server][https://docs.couchbase.com/home/server.html]
//footnote[authorization-overview][https://docs.couchbase.com/server/current/learn/security/authorization-overview.html]


== コンテナ利用

=== Docker

Sync GatewayのDockereコンテナイメージは、Docker Hubの以下のURLで公開されています。

//blankline

https://hub.docker.com/r/couchbase/sync-gateway

//blankline

Couchbase Serverのコンテナイメージも、以下に公開されています。

//blankline

https://hub.docker.com/_/couchbase

//blankline


次章で、Docker環境を使って、Couchbase ServerとCouchbase ServerクライアントWebアプリケーションを含めたCouchbase Mobileの機能を体験するための環境構築と操作方法について解説します。

=== Kubernetes

//footnote[operator-overview][https://docs.couchbase.com/operator/current/overview.html]
//footnote[howto-couchbase-create][https://docs.couchbase.com/operator/current/howto-couchbase-create.html]
//footnote[operator-tutorial-sync-gateway][https://docs.couchbase.com/operator/current/tutorial-sync-gateway.html]

Couchbase ServerやSync Gatewayを、Kubernetes環境で利用するための技術として、@<em>{Couchbase Autonomous Operator}@<fn>{operator-overview}があります。
Autonomous Operatorは、KubernetesおよびRedHat OpenShiftとのネイティブ統合を提供します。

=== 開発参考情報

Autonomous Operatorを使ってSync Gatewayをデプロイする方法については、ドキュメント(Connecting Sync Gateway to a Couchbase Cluster@<fn>{operator-tutorial-sync-gateway})を参照してください。

また、Couchbase ServerをAutonomous Operatorを使ってデプロイする方法についても、ドキュメント(Create a Couchbase Deployment@<fn>{howto-couchbase-create})を参照することができます。

KubernetesプラットフォームにCouchbaseをデプロイする際のベストプラクティスとAWS EKS環境にCouchbase ServerとSync Gatewayをセットアップする手順について、Couchbase Blog: Get set to the edge with Sync Gateway@<fn>{get-set-to-the-edge-with-sync-gateway}を参照することができます。
//footnote[get-set-to-the-edge-with-sync-gateway][https://blog.couchbase.com/get-set-to-the-edge-with-sync-gateway/]