
= Sync Gateway管理

== Sync Gateway構成

Sync Gatewayの構成(Configuration@<fn>{configuration-overview})について解説します。

//footnote[configuration-overview][https://docs.couchbase.com/sync-gateway/current/configuration-overview.html]

=== 概要

Sync Gatewayは、複数のSync Gatewayノードからなるクラスターとして構成することによって、水平方向にスケーリングすることが可能です。
単一障害点を避け、高可用性を確保するためには、少なくともふたつのSync Gatewayノードが必要になります。

Sync Gatewayは、このようなクラスターとしての運用に適した、@<em>{一元化永続的モジュラー構成}(Centralized Persistent Modular Configuration)を採用しています。

Sync Gatewayの構成情報は、Sync Gatewayノード毎に個別に管理されるのではなく、それらのノードから構成されるSync Gatewayクラスターが共通して利用するCouchbase Serverで管理されます。
具体的には、Sync Gatewayに対して、管理REST APIを通じて構成管理を行うことで、永続的かつ一元化された構成管理が実現されます。

このような一元化永続的モジュラー構成によって、たとえば、ノード毎にそれぞれの構成ファイルを管理する場合と比べて、シンプルかつ俊敏な構成の更新が可能になっています。

一元化永続的モジュラー構成の特徴について、以下に整理します。

//blankline

 * @<em>{永続的構成}: 管理REST APIを使用して行われた構成情報の変更はすべてCouchbase Serverデータベースに永続化され、Sync Gatewayの再起動後も存続します。
 * @<em>{構成情報共有とノード固有構成の両立}: Sync Gatewayクラスターを構成するノードは、共通のCouchbase Serverに接続します。ノードは、Couchbase Serverを介して、構成情報を共有します。また、各ノードはそのノード固有の構成を持つこともできます。
 * @<em>{ブートストラップ起動}: ブートストラップ設定ファイルという最小限の構成ファイルを使用して、Sync Gatewayノードを起動し、Couchbase Serverクラスターに接続します。ブートストラップ設定ファイルに記載される設定はノード固有になります。
 * @<em>{動的構成}: 実行中のSync Gatewayの構成を、管理REST APIを使用して変更することができます。これにより、俊敏なメンテナンスが可能になります。必要な場合には、自動再起動が行われます。
 * @<em>{リモート管理}: Sync Gatewayノードのトポロジーに左右されず、容易にSync Gatewayを管理することができます。これは、エッジデータセンターにSync Gatewayがデプロイされている場合に特に有用です。また、構成情報の変更・確認においては、ユーザー認証とロールベースのアクセスコントロールが適用されるため、セキュリティーを担保しながら容易な管理を実現することができます。

//blankline


Sync Gatewayの構成プロパティーは、ノードに固有の@<em>{ノードレベルプロパティー}と、Sync Gatewayクラスター内で共有される@<em>{データベースレベルプロパティー}のふたつのレベルのいずれかに属します。

=== ノードレベルプロパティー

ノードレベルプロパティーはノード固有であり、他のノードとは共有されません。
以下のようなプロパティーが、ノードレベルプロパティーに属します。

//blankline

 * @<em>{Couchbase Server接続設定}: Sync GatewayノードがCouchbase Serverへ接続するために必要な情報であり、構成プロパティーの最小限セットです。変更には再起動が必要です。
 * @<em>{システムプロパティー}: たとえば、TLS認証ファイルのパス(@<tt>{api.tls.cert_path})や、サーバー固有のファイルデスクリプター最大値(@<tt>{max_file_descriptors})のようなシステム固有のプロパティーです。変更には再起動が必要です。
 * @<em>{ロギングプロパティー}: ノードのログ出力に関するプロパティーです。管理REST APIにより変更可能であり、その場合、再起動は不要ですが、REST APIによるロギングプロパティーへの変更は、再起動後は引き継がれません。

=== データベースレベルプロパティー

データベースレベルプロパティーは、クラスターを構成するSync Gatewayノードで共有されます。
これらのプロパティーは、管理REST APIにより変更可能です。
以下のようなプロパティーが、データベースレベルプロパティーに属します。

//blankline

 * @<em>{DBプロパティー}: @<tt>{bucket}のようなデータベース構成や@<tt>{users}のようなデータベースへのアクセス制御ポリシーに関するプロパティーです。
 * @<em>{レプリケーションプロパティー}: レプリケーションに関するプロパティーです。

=== ブートストラップ構成ファイル

以下は、ブートストラップ構成ファイルの例です。

//emlist[][Java]{
{
  "bootstrap": {
    "server": "couchbases://localhost",
    "server_tls_skip_verify": true,
    "username": "Administrator",
    "password": "password"
  },
  "logging": {
    "console": {
      "enabled": true,
      "log_level": "info",
      "log_keys": ["*"]
    }
  } 
}
//}

以下は、このようなブートストラップ構成ファイルを利用して、Sync Gatewayをコマンドラインから実行した際の出力の例です。@<fn>{memo-foreground}

//footnote[memo-foreground][サーバーソフトウェアをこのようにフォアグラウンドプロセスで実行することに疑問を持たれる方がいるかもしれませんが、ここではわかりやすさを優先しています。Sync Gatewayをサービスとしてバックグラウンドで実行する方法について、後の環境構築についての章で紹介しています。]

//cmd{
$ ./bin/sync_gateway ./basic.json 
2022-05-20T14:25:27.239+09:00 ==== Couchbase Sync Gateway/3.0.0(541;46803d1) CE ====
2022-05-20T14:25:27.241+09:00 [INF] Loading content from [./basic.json] ...
2022-05-20T14:25:27.243+09:00 [INF] Config: Starting in persistent mode using config group "default"
2022-05-20T14:25:27.243+09:00 [INF] Logging: Console to stderr
2022-05-20T14:25:27.243+09:00 [INF] Logging: Files disabled
2022-05-20T14:25:27.243+09:00 [ERR] No log_file_path property specified in config, and --defaultLogFilePath command line flag was not set. Log files required for product support are not being generated. -- base.InitLogging() at logging_config.go:71
2022-05-20T14:25:27.244+09:00 [INF] Logging: Console level: info
2022-05-20T14:25:27.244+09:00 [INF] Logging: Console keys: [* HTTP]
2022-05-20T14:25:27.244+09:00 [INF] Logging: Redaction level: partial
2022-05-20T14:25:27.244+09:00 [INF] Configured process to allow 5000 open file descriptors
2022-05-20T14:25:27.244+09:00 [INF] Logging stats with frequency: &{1m0s}
2022-05-20T14:25:27.412+09:00 [INF] Config: Successfully initialized cluster agent
2022-05-20T14:25:27.811+09:00 [WRN] Config: No database configs for group "default". Continuing startup to allow REST API database creation -- rest.(*ServerContext).initializeCouchbaseServerConnections() at server_context.go:1416
2022-05-20T14:25:27.811+09:00 [INF] Config: Starting background polling for new configs/buckets: 10s
2022-05-20T14:25:27.811+09:00 [INF] Starting metrics server on 127.0.0.1:4986
2022-05-20T14:25:27.811+09:00 [INF] Starting admin server on 127.0.0.1:4985
2022-05-20T14:25:27.811+09:00 [INF] Starting server on :4984 ...
//}


データベースの設定が含まれていないという警告が出力されていますが、管理REST APIによる構成を受け付けるために、Sync Gatewayが実行されているのがわかります。

====[column]構成グループ

Sync Gatewayエンタープライズエディションではクラスターのノードを、共通の構成を共有するグループに所属させることが可能です。
共通の構成を共有するノードのグループを@<em>{構成グループ(Config Group)}と呼びます。
グループIDプロパティー（@<tt>{bootstrap.group_id}）を使用して、Sync Gatewayノードをグループ化します。

ひとつのノードに行われた変更は、グループ内の他のノードに自動的に伝播されます。

構成グループを明示的に指定しない場合、そのノードは、@<tt>{default}グループに所属します。

====[/column]


=== 永続的構成の無効化

Sync Gatewayでは、管理REST APIによる構成設定をCouchbase Serverデータベースに永続化する永続的構成(Persistent Config)を無効に設定し、全ての構成情報を構成ファイルを用いて静的に構成するオプションも用意されています。

本格的な複数台ノードからなるクラスター運用では、一元化永続的モジュラー構成が有益ですが、たとえば開発用に環境を複製する場合など、再現性のある構成情報を簡単に共有するために静的な構成ファイル管理の利点を活用することができます。
このオプションを使用するには、ブートストラップ設定ファイルの中で@<tt>{disable_persistent_config}フラグを@<tt>{true}に設定します。

=== 構成ファイル内での関数定義方法

永続的構成を無効化し、全ての構成情報をファイルを用いて構成する場合、Sync関数のようなJavaScript関数による設定は、JSONフォーマットで定義される構成ファイル内に記述することになります。

Sync Gateway構成ファイル内でJavaScript関数を定義する場合には、以下のようにバッククォート(@<tt>{`})で関数定義箇所を囲みます。@<fn>{memo-sync-comment}

//emlist[][json]{
{
    "disable_persistent_config": true,
    "databases": {
        "travel-sample": {
            "sync": `
            function(doc, oldDoc, meta) {
                // Sync関数実装
            }`
        }
    }
}
//}

このようなバッククォートの利用は、JSON本来の仕様には含まれない、Sync Gateway構成ファイル独自のものです。


====[column]JavaScript関数定義方法

上記の例における関数定義では、JavaScriptの無名関数を用いています。

Couchbase Mobileのドキュメントやサンプルでは、無名関数が用いられている場合と、関数に(@<tt>{sync}のような)名前が定義されている場合の両方が見られます。

構成プロパティーの値として定義された関数は、何らかの変数に代入された上で利用されている(つまり、特定の関数命名規則に依存しているのではない)と考えると、この記載方法の違いが本質的ではないことが理解できるでしょう。

====[/column]

//footnote[memo-sync-comment][本記述例は、構造を簡潔に示すためのものであり、本来の構成とは異なります。]

== 管理REST APIによる構成情報登録

=== 概要

アクセス制御のために、管理REST APIを利用して各種アクセス権限管理エンティティーを構成する方法を解説します。

Couchbase Mobileにおけるレプリケーションの構成単位は、Couchbase Serverのバケットです。
このCouchbase Serverの「バケット」は、他のデータベースにおける「データベース」に類する位置づけと捉えることができ、Sync Gateway構成プロパティー上の命名としても「データベース(Database/DB)」という語が用いられています。アクセス制御の設定は、この「データベース」単位に行います。

設定対象データベースは、管理REST APIエンドポイントURL上のパスで表現されます。
以降の説明では、このパス中のデータベース指定を@<tt>{{db\}}と表記します。

なお、管理REST APIのデフォルトのポート番号は4985であり、例中でもデフォルトのポート番号が利用されています。

=== APIエクスプローラー

以降で紹介する実行例は、エンドポイントの全てを網羅するものではありません。

Couchbaseのドキュメンテーションでは、管理REST APIとして公開されている全てのエンドポイントを確認することのできるAPIエクスプローラー@<fn>{api-explorer-admin}が公開されています。
このAPIエクスプローラーは、OpenAPI@<fn>{openapi}仕様に基づいたフォーマットを持っています。
OpenAPI仕様は、APIドキュメンテーション化のためのフレームワークであるSwagger@<fn>{swagger-io}を起源としており、広く用いられています。

//footnote[api-explorer-admin][https://docs.couchbase.com/sync-gateway/current/rest-api-admin.html#api-explorer]
//footnote[swagger-io][https://swagger.io/]

//footnote[openapi][https://www.openapis.org/]

=== 認証

管理REST APIへのリクエストを行うためには、必要な権限のセットを有しているCouchbase Serverユーザーによる認証が必要になります。

以下は、認証情報を与えて管理REST API(のルートエンドポイント)をコールした例です。

//cmd{
$ curl -X GET http://localhost:4985 \
  -H 'authorization: Basic QWRtaW5pc3RyYXRvcjpwYXNzd29yZA=='
{"ADMIN":true,"couchdb":"Welcome","vendor":{"name":"Couchbase Sync Gateway","version":"3.0"},"version":"Couchbase Sync Gateway/3.0.0(541;46803d1) CE"
//}


なお、上記の認証用のヘッダーに指定した情報は、下記のように生成したものです。

//cmd{
$ echo -n "Administrator:password" | base64
QWRtaW5pc3RyYXRvcjpwYXNzd29yZA==
//}

認証情報を用いない場合は、下記のような結果になります。

//cmd{
$ curl -X GET http://localhost:4985
{"error":"Unauthorized","reason":"Login required"}
//}

=== データベース登録

@<tt>{/{db\}/}エンドポイントにPUTリクエストを送信して、新しいデータベースを登録します。

以下の例では、Couchbase Serverの@<tt>{mybucket}という名前のバケットをSync Gatewayで利用するために登録しています。

//cmd{
$ curl --location --request PUT 'http://localhost:4985/mybucket/'  \
  -H 'authorization: Basic QWRtaW5pc3RyYXRvcjpwYXNzd29yZA==' \
  --header 'Content-Type: application/json' \
  --data-raw '{ "bucket": "mybucket","num_index_replicas": 0}'
//}

登録されたデータベースに対して、@<tt>{/{db\}/_config}エンドポイントにPUTリクエストを送信して、構成情報の登録・変更を行うことができます。

構成プロパティーの詳細については、ドキュメント(Configure Database@<fn>{lbl-configure-db})を参照ください。

//footnote[lbl-configure-db][https://docs.couchbase.com/sync-gateway/current/configuration-schema-database.html#lbl-configure-db]

=== ユーザー作成

@<tt>{/{db\}/_user/}エンドポイントにPOSTリクエストを送信して、新しいユーザーを作成します。

以下の例は、「mydatabase」のアクセス制御のために新しいSync Gatewayユーザー「Edge1User」を追加します。

//cmd{
$ curl -vX POST "http://localhost:4985/mybucket/_user/" \
  -H "accept: application/json" -H "Content-Type: application/json" \
  -H 'authorization: Basic QWRtaW5pc3RyYXRvcjpwYXNzd29yZA==' \
  -d '{"name": "Edge1User", "password": "pass"}'
//}

既存のユーザーを更新する場合は、PUTリクエストを送信します。この場合、URLパスの最後にユーザー名を含めます。

以下の例は、既存のユーザー「Edge1User」を更新し、@<tt>{admin_channels}にエントリーを追加します。

//cmd{
$ curl -vX PUT "http://localhost:4985/mybucket/_user/Edge1User" \
  -H "accept: application/json" -H "Content-Type: application/json" \
  -H 'authorization: Basic QWRtaW5pc3RyYXRvcjpwYXNzd29yZA==' \
  -d '{"name": "Edge1User", "admin_channels": ["RandomChannel"]}'
//}

=== ロール作成


@<tt>{/{db\}/_role/}エンドポイントにPOSTリクエストを送信して、新しいロールを作成します。

以下の例は、「mybucket」のアクセス制御のために新しいSync Gatewayロール「Edge1」を追加します。

//cmd{
$ curl -vX POST "http://localhost:4985/mybucket/_role/" \
  -H "accept: application/json" -H "Content-Type: application/json" \
  -H 'authorization: Basic QWRtaW5pc3RyYXRvcjpwYXNzd29yZA==' \
  -d '{"name": "Edge1"}' 
//}

=== ユーザーへのロールとチャネルの割り当て


以下のように、ユーザーにロールとチャネルを割り当てることができます。

ここでは、「Edge1User」に、「Edge1」ロールと「Channel1」チャネルを割り当てています。

//cmd{
$ curl -vX PUT "http://localhost:4985/mybucket/_user/Edge1User" \
  -H "accept: application/json" -H "Content-Type: application/json" \
  -H 'authorization: Basic QWRtaW5pc3RyYXRvcjpwYXNzd29yZA==' \
  -d '{ "admin_roles": ["Edge1"], "admin_channels": ["Channel1"]}' 
//}


=== ロールへのチャネルの割り当て


@<tt>{/{db\}/_role/{name\}}エンドポイントにPUTリクエストを送信して、ロールにチャネルを追加します。
@<tt>{{name\}}は、更新するロール名です。

@<tt>{admin_channels}プロパティーの値に割り当てるチャネルを指定します。


//cmd{
$ curl -vX PUT "http://localhost:4985/mybucket/_role/Edge1" \
  -H "accept: application/json" -H "Content-Type: application/json" \
  -H 'authorization: Basic QWRtaW5pc3RyYXRvcjpwYXNzd29yZA==' \
  -d '{ "admin_channels": ["Channel2","Channel3"]}' 
//}

=== Sync関数登録

@<tt>{_config/sync}エンドポイントにPUTリクエストを送信して、Sync GatewayにSync関数を登録します。

//cmd{
$ curl -vX PUT 'http://localhost:4985/mybucket/_config/sync' \
  -H 'authorization: Basic QWRtaW5pc3RyYXRvcjpwYXNzd29yZA==' \
  -H 'Accept: application/json'  -H 'Content-Type: application/javascript' \
  -d 'function(doc,oldDoc, meta){ if (doc.published) { channel("public");} }'
//}

関数の登録には、ここで紹介したパラメーターとして指定する方法の他、ファイルパスを指定する方法や、URLを指定する方法があります。
詳しくはドキュメント(Using External Javascript Functions@<fn>{configuration-javascript-functions})を参照ください。

//footnote[configuration-javascript-functions][https://docs.couchbase.com/sync-gateway/current/configuration-javascript-functions.html]


=== 匿名アクセス有効化

次の例では、GUESTアカウントを有効にし、「public」という名前のチャネルへのアクセスを許可しています。

//cmd{
$ curl -X PUT localhost:4985/mybucket/_user/GUEST \
  -H 'authorization: Basic QWRtaW5pc3RyYXRvcjpwYXNzd29yZA==' \
  --data  '{"disabled":false, "admin_channels":["public"]}'
//}

== 管理REST APIによる構成情報確認

=== 概要

ここまで、管理REST APIにより設定を追加、変更する方法を見てきました。
運用上、既存の設定を確認することも必要になる場面があるでしょう。
以下では、構成対象毎に、現在の構成情報を確認する方法を紹介します。

=== データベース

@<tt>{/{db\}}エンドポイントにGETリクエストを送信します。



//cmd{
$ curl  'http://localhost:4985/mybucket/' \
 -H 'authorization: Basic QWRtaW5pc3RyYXRvcjpwYXNzd29yZA==' 
//}

以下は、上記リクエストの出力の例です。

//emlist[][JSON]{
{ 
  "db_name":"mybucket",
  "update_seq":0,"committed_update_seq":0,"instance_start_time":1653967274774798,"compact_running":false,"purge_seq":0,"disk_format_version":0,"state":"Online","server_uuid":"98274548610aa472839f796329c90f9c"
}
//}

=== ユーザー


@<tt>{/{db\}/_user/{name\}}エンドポイントにGETリクエストを送信します。


以下は、ユーザー@<tt>{Edge1User}のアクセスを確認する例です。


//cmd{
$ curl http://localhost:4985/mybucket/_user/Edge1User \
 -H 'authorization: Basic QWRtaW5pc3RyYXRvcjpwYXNzd29yZA=='
//}

以下は、上記リクエストの出力の例です。

//emlist[][JSON]{
{ 
  "name":"Edge1User",
  "admin_channels":["Channel1"],
  "all_channels":["!","Channel1","Channel2","Channel3"],
  "disabled":false,
  "admin_roles":["Edge1"],
  "roles":["Edge1"]
}
//}

ユーザー@<tt>{Edge1User}には、@<tt>{Edge1User}自身の@<tt>{admin_channels}プロパティーを通じて @<tt>{Channel1}が割り当てられており、@<tt>{Edge1}ロールの@<tt>{admin_channels}プロパティーを通じて@<tt>{Channel2}と@<tt>{Channel3}が割り当てられています。
ロールの@<tt>{admin_channels}の内容を確認しなくとも、ユーザーの@<tt>{all_channels}プロパティーから、これらのチャネルの全てを確認することができます。


=== ロール


@<tt>{/{db\}/_role/{name\}}エンドポイントにGETリクエストを送信します。


以下は、ロール@<tt>{Edge1}のアクセスを確認する例です。


//cmd{
$ curl http://localhost:4985/mybucket/_role/Edge1 \
  -H 'authorization: Basic QWRtaW5pc3RyYXRvcjpwYXNzd29yZA=='
//}

以下は、上記リクエストの出力の例です。

//emlist[][JSON]{
{
  "name":"Edge1",
  "admin_channels":["Channel2","Channel3"],
  "all_channels":["!","Channel2","Channel3"]
}
//}


=== Sync関数

Sync関数の内容を確認するには、@<tt>{/{db\}/_config/sync}エンドポイントにGETリクエストを送信します。

//cmd{
$ curl 'http://localhost:4985/mybucket/_config/sync' \
  -H 'authorization: Basic QWRtaW5pc3RyYXRvcjpwYXNzd29yZA=='
//}

