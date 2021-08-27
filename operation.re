

= コマンドライン操作

== コマンド

=== couchbase-cli

@<tt>{couchbase-cli}はCouchbase　Serverの管理のため提供されている、中心となるコマンド・ライン・インターフェイスです。

基本的な構文は以下の通りです。

//cmd{
couchbase-cli <コマンド> [オプション]
//}

==== 基本オプション

 * @<tt>{–version} バージョン情報を出力します。

 * @<tt>{–help} (@<tt>{-h}) ヘルプメッセージを出力します。

==== コマンド

@<tt>{couchbase-cli}で利用可能なコマンドを紹介します。詳細な利用方法は公式ドキュメント@<fn>{couchbase-cli}をご覧ください。

//table{
コマンド	内容
----------------------------
admin-role-manage	LDAPユーザーロールを管理します。
bucket-compact	Couchbaseデータバケットを圧縮します。
bucket-create	新しいCouchbaseデータバケットを作成します。
bucket-delete	Couchbaseデータバケットを削除します。
bucket-edit	Couchbaseデータバケット設定を編集します。
bucket-flush	Couchbaseデータバケットのデータをフラッシュします。
bucket-list	すべてのCouchbaseデータバケットを一覧表示します。
cluster-edit	クラスター設定を編集します。
node-to-node-encryption	ノード間暗号化の有効化/無効化を設定します。
cluster-init	ouchbaseクラスターを初期化します。
ip-family	ouchbaseクラスターを初期化します。
collect-logs-start	ログ収集を開始します。
collect-logs-status	ログ収集ステータスを取得します。
collect-logs-stop	現在のログ収集タスクを停止します。
eventing-function-setup	ventingサービス機能を管理します。
failover	クラスター内のサーバーをフェイルオーバーします。
group-manage	サーバーグループを管理します。
host-list	クラスター内のすべてのホストを一覧表示します。
node-init	ノードを初期化します。
rebalance	クラスター内のノード間でデータのリバランスを行います。
rebalance-status	現在のリバランスステータスを表示します。
rebalance-stop	現在のリバランスタスクを停止します。
recovery 	以前にフェイルオーバーしたノードを回復します。
reset-admin-password	管理者パスワードをリセットします。
reset-cipher-suites	暗号スイートをデフォルトにリセットします。
server-add	サーバーをクラスターに追加します。
server-info	サーバーレベルの情報と統計を表示します。
server-list	クラスター内のすべてのサーバーを一覧表示します。
server-readd	フェイルオーバー後にサーバーをクラスターに追加し直します。
setting-alert	アラート設定を変更します。
setting-audit	監査ログの設定を変更します。
setting-autofailover	自動フェイルオーバー設定を変更します。
setting-autoreprovision	自動再プロビジョニングの設定を変更します。
setting-cluster	クラスター設定を変更します。
setting-compaction	圧縮設定を変更します。
setting-index	インデックス設定を変更します。
setting-ldap	DAP設定を変更します。
setting-notification	通知設定を変更します。
setting-password-policy	パスワードポリシーを変更します。
setting-query	クエリ設定を変更します。
setting-rebalance	リバランス設定を変更します。
setting-security	セキュリティポリシーを変更します。
setting-xdcr	クロスデータセンターレプリケーション（XDCR）設定を変更します。
ssl-manage	SSL証明書を管理します。
user-manage	RBACユーザーを管理します。
xdcr-replicate	XDCRクラスター参照を管理します。
xdcr-setup	XDCRレプリケーションを管理します。
setting-saslauthd	saslauth設定を管理します。
enable-developer-preview	開発者プレビューを有効にします。
collection-manage	コレクションを管理します。
//}



//footnote[couchbase-cli][https://docs.couchbase.com/server/current/cli/cbcli/couchbase-cli.html]

== ストレスツール

データベースを運用するに当たって、ベンチマーク(ストレス)テストは避けて通れない要素です。

Couchbase Serverでは、Dataサービスと、Queryサービスという二種類のサービスが、基本的なデータ操作に関係しています。

それぞれのサービスには、下記のように異なるツールが用意されています。

//blankline

//table{
サービス	ストレスツール	データ操作
----------------------------
Dataサービス	pillowfight	全て
Queryサービス	n1qlback	（主に）READ
//}

//blankline

Queryサービス(N1QLクエリ)は、@<tt>{INSERT} 、 @<tt>{DELETE} や @<tt>{UPDATE}もサポートしていますが、アプリケーション連携において、キーによる操作が可能なところで、クエリを用いることは、アンチパターンとなるため（@<tt>{WHERE}句を用いた条件によるデータ削除・更新や、@<tt>{INSERT .. FROM SELECT…}のような処理が必要な要件がある場合は、その限りではありません）、実際的には、READの処理が中心となります。

これらのツールは、Couchbase ServerのC言語SDK @<tt>{libcouchbase}の一部として提供されます。

=== インストール

==== CentOS/RHEL

//cmd{
$ sudo yum check-update
$ sudo yum search libcouchbase
$ sudo yum install libcouchbase3 libcouchbase-devel libcouchbase3-tools libcouchbase3-libevent
//}

==== Mac

//cmd{
$ brew update
$ brew install libcouchbase
//}

==== Windows

SDKリリースノート@<fn>{install-windows}から、バイナリをダウンロード・解凍して利用します。

その他のOSについてもドキュメント@<fn>{sdk-release-notes}を参照してください。

//footnote[install-windows][https://docs.couchbase.com/c-sdk/current/project-docs/sdk-release-notes.html#install-windows]

//footnote[sdk-release-notes][https://docs.couchbase.com/c-sdk/current/project-docs/sdk-release-notes.html]

=== 共通オプション

以下のオプションは、クラスターへの接続に関連する、@<tt>{libcouchbase}コマンドの共通オプションです。

//table[共通オプション][]{
オプション	省略形	説明 
----------------------------
--spec	-U	接続文字列 [デフォルト:@<tt>{couchbase：//localhost/default}]
--username	-u	ユーザー名
-password	-P	パスワード
--timings	-T	終了時に実行されたコマンドの待ち時間を示すヒストグラムを表示
--verbose	-v	デバッグ出力を有効にする
--cparam	-D	接続のための追加のオプション(例. @<tt>{-D-timeout=[秒数]})
--compress	-y	送信データの圧縮をオンにする
--truststorepath	なし	トラストストアへのパス
--certpath	なし	サーバーSSL証明書へのパス
--keypath	なし	クライアントSSL秘密鍵へのパス
--dump	なし	操作完了後に詳細な内部状態をダンプする
//}


=== pillowfight

@<tt>{pillowfight}を使って、Dataサービスに対する、ドキュメント操作のストレステストを実施することができます。

@<tt>{pillowfight} は、Couchbase C SDK @<tt>{libcouchbase}の一部として提供されます。

==== オプション

//table[pillowfightオプション][]{
オプション	省略形	説明 
----------------------------
--batch-size	-B	バッチ処理の数 [デフォルト: 100] 
--num-items	-I	アイテムの数 [デフォルト: 1000]
--key-prefix	-p	キー接頭辞の指定
--num-threads 	-t	スレッドの数 [デフォルト: 1]
--random-body	-R	ドキュメント本文をランダム化する [デフォルト: FALSE]
--set-pct	-r	ミューテーション操作の割合 [デフォルト: 33]
--no-population	-n	ポピュレーション（入力）をスキップする[デフォルト:FALSE]
--min-size	-m	最小ペイロードサイズ [デフォルト: 50]
--max-size	-M	最大ペイロードサイズ [デフォルト:5120]
--pause-at-end	-E	実行の終了時に一時停止する（ユーザー入力まで接続を保持する)[デフォルト: FALSE]
--num-cycles	-c	終了するまで実行されるサイクル数。-1に設定すると、無限にループする[デフォルト: -1 ]
--sequential	なし	ランダムアクセスでなく）シーケンシャルアクセスを使用[デフォルト: FALSE]
--start-at	なし	シーケンシャルアクセスの場合に、最初の値を設定[デフォルト:0]
--rate-limit	なし	秒あたり操作の制限を設定(スレッド単位）[デフォルト: 0]
--docs	なし	ロードするユーザードキュメント（@<tt>{–-min-s-ize} と@<tt>{–-max-size} を上書き）
--json	-J	bytesではなく）JSONデータの書き込みを有効にする[デフォルト: FALSE]
--subdoc	なし	fulldoc操作の代わりにsubdoc操作を使用[デフォルト: ALSE]
--noop	なし	ドキュメント操作の代わりにNOOPを使用[デフォルト: FALSE]
--pathcount	なし	コマンドあたりのサブドキュメントパスの数[デフォルト: 1]
--populate-only	なし	ドキュメントのポピュレーション（入力）のみを実施し、終了する[デフォルト: FALSE]
--expiry	-e	アイテムのTTLを設定 [デフォルト: 0]
--collection	なし	コレクションの（スコープを含む）フルパス（複数指定可能）
--durability	-d	耐久レベル [デフォルト: @<tt>{none}]
--persist-to	なし	アイテムを指定した数のノードに永続化するまでブロックする（\"-1\"の場合、対象となるすべてのノード）[デフォルト: 0]
--replicate-to	なし	アイテムを指定した数のノードに複製されるまでブロックする（\"-1\"の場合、すべてのレプリカ） [デフォルト: 0]
--lock	なし	更新用ロックキーへ時間設定ゼロの場合、ロックされない）[デフォルト: 0]
--config-cache	-Z	キャッシュ設定へのパス
--help 	-？	ヘルプメッセージを表示する
//}

==== 実行例

20スレッドで実行。１スレッド当たり１バッチ処理実行。

//cmd{
$ cbc-pillowfight -t 20 -B 1
//}

50アイテムのデータセットを使って、１００回のイテレーションを行う。

//cmd{
$ cbc-pillowfight -c 100 -I 50
//}

サイズ100kのJSONドキュメントを利用。

//cmd{
$ cbc-pillowfight --json -m 100000 -M 100000
//}

サブドキュメント操作によるストレステスト。

//cmd{
$ cbc-pillowfight --json --subdoc --set-pct 100
//}

接続先、アカウントの指定。JSONドキュメントを、10秒後に期限切れにする。

//cmd{
$ cbc-pillowfight -U couchbase://localhost/pillow -u Administrator -P password --json -e 10
//}

==== 参考情報


Couchbase公式ドキュメント C SDK Release Notes and Archives@<fn>{sdk-release-notes}

Couchbase公式ドキュメント Couchbase C Client 3.2.0 cbc-pillowfight@<fn>{md_doc_cbc-pillowfight}

Couchbaseブログ Performance Testing and Load Testing Couchbase with Pillowfight@<fn>{performance-testing-load-testing-couchbase-pillowfight}

//footnote[sdk-release-notes][hhttps://docs.couchbase.com/c-sdk/current/project-docs/sdk-release-notes.html]

//footnote[md_doc_cbc-pillowfight][https://docs.couchbase.com/sdk-api/couchbase-c-client-3.2.0/md_doc_cbc-pillowfight.html]

//footnote[performance-testing-load-testing-couchbase-pillowfight][https://blog.couchbase.com/performance-testing-load-testing-couchbase-pillowfight/]

=== n1qlback

@<tt>{n1qlback} は、@<tt>{pillowfight} 同様、Couchbase C SDK @<tt>{libcouchbase}の一部として提供されます。

@<tt>{n1qlback}は、指定された数のスレッドを作成し、それぞれがユーザー定義のクエリのセットを実行します。

@<tt>{n1qlback}では、実行するクエリを含むファイル（1行に1クエリ）へのパスを渡す必要があります。クエリは、下記のように、サーバーに送信される実際の
@<tt>{HTTP POST}本文の形式（JSON形式）である必要があります。単純なクエリの場合、ステートメントフィールドのみを設定します。

//cmd{
{"statement":"SELECT country FROM `travel-sample`"\}
{"statement":"SELECT country, COUNT(country) FROM `travel-sample` GROUP BY country"\}
//}

次の例は、名前付きパラメータの使用方法を示しています。

//cmd{
{"statement":"SELECT RAW meta().id FROM `travel-sample` WHERE type=$type LIMIT 1", "$type":"airline"\}
//}

処理を実行する前にコマンドにより削除される特別なオプション「n1qlback」が、あります。実行前に特定のクエリを準備する必要があるかどうかを判断できます。

//cmd{
{"statement":"SELECT * FROM `travel-sample` WHERE type=$type LIMIT 10", "$type":"airline", "n1qlback": {"prepare": true\}\}
//}

@<tt>{n1qlback} を利用する際には、Couchbase Serverに必要なリソース（データ、インデックス）が定義されている必要があります（当たり前のようですが、ツールが必要になるインデックスを事前に作成するようなような補助機能は備えていないということです）。

==== オプション

 * @<tt>{-f} @<tt>{–queryfile=PATH}:
JSON形式で実行するクエリ本文を含むファイルへのパス。1行に1つのクエリ。

 * @<tt>{-t}, @<tt>{–num-threads=NTHREADS}:同時に実行するスレッド数（クライアントインスタンスの数）を設定します。各スレッドには、それぞれクライアントオブジェクトが割り当てられます。
 * @<tt>{-e}, @<tt>{–error-log=PATH}:ファイルへのパスを指定します。そのファイルへコマンドは失敗したクエリとエラーの詳細を書き込みます。このオプションの使用により、ERRORSメトリックがゼロでない理由を把握することができます。

次のオプションは、@<tt>{couchbase：//host/bucket？option1=value1＆option2=value2}
のような、URIスタイルのクエリパラメータ（例）として、接続文字列に含めることができます。
あるいは、個々のキー=値のペアとしてDスイッチと共に利用することができます（
@<tt>{-D}
は内部で接続文字列を作成し、コマンドラインでオプションを簡単に渡すための便宜のために提供されています）。

 * @<tt>{operation_timeout=SECONDS}:
操作タイムアウトを秒単位で指定します。これは、タイムアウトするまでに、クライアントが操作の完了を待つ時間です。デフォルトは2.5です。

 * @<tt>{config_cache=PATH}:クライアントがブートストラップ操作によって接続するのではなく、ファイルベースの構成キャッシュを利用できるようにします。ファイルが存在しない場合、クライアントは最初にクラスターに接続し、次にブートストラップ情報をファイルにキャッシュします。
 * @<tt>{certpath=PATH}:サーバーのSSL証明書へのパス。これは通常、証明書がシステムのopensslインストールにすでに追加されていない限り、SSL接続に必要です（@<tt>{couchbases：//}スキームでのみ適用可能）
 * @<tt>{ssl=no_verify}:SSLの証明書検証を一時的に無効にします（@<tt>{couchbases：//}スキームでのみ適用可能）。これは、SSL機能をすばやくデバッグするためにのみ使用してください。
 * @<tt>{sasl_mech_force=MECHANISM}:初期接続を実行するときに、特定のSASLメカニズムを強制的に使用します。これは、デバッグ目的でのみ変更する必要があります。現在サポートされているメカニズムは、PLAINとCRAM-MD5です。

==== 実行例

以下は、3つのクエリが5つのスレッドで実行されるファイルを作成します。また、クエリに必要なインデックスの作成を事前に行っています。

//cmd{
cbc n1ql -U couchbase://192.168.72.101/a_bucket 'CREATE INDEX ix_name ON}travel-sample@<tt>{(name)'
cbc n1ql -U couchbase://192.168.72.101/a_bucket 'CREATE INDEX ix_country ON}travel-sample`(country)'

cat queries.txt <<EOF
{"statement":"SELECT country FROM travel-sample WHERE travel-sample.country === \"United States\""}
{"statement":"SELECT name FROM travel-sample LIMIT 10"}
{"statement":"SELECT country, COUNT(country) FROM travel-sample GROUP BY country"}
EOF

cbc-n1qlback -U couchbase://192.168.72.101/a_bucket -t 5 -f queries.txt
//}

==== 参考情報

Couchbase公式ドキュメント Install and Start Using the C SDK with Couchbase Server@<fn>{start-using-sdk}

Couchbase Githubリポジトリ cbc-n1qlback@<fn>{cbc-n1qlback}

//footnote[start-using-sdk][https://docs.couchbase.com/c-sdk/current/hello-world/start-using-sdk.html]

//footnote[cbc-n1qlback][https://github.com/couchbase/libcouchbase/blob/master/doc/cbc-n1qlback.markdown]

