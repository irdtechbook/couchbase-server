= ユーティリティ

== 概要

Couchbase Serverには、CLIリファレンス@<fn>{cli-intro}に含まれているコマンドラインツールの他に、C SDK(Couchbase C Client)の一部として@<tt>{libcouchbase}コマンドにカテゴライズされるユーティリティが利用可能です。@<fn>{start-using-sdk}

//footnote[cli-intro][https://docs.couchbase.com/server/current/cli/cli-intro.html]

//footnote[start-using-sdk][https://docs.couchbase.com/c-sdk/current/hello-world/start-using-sdk.html]

=== インストール

ここで紹介するユーティリティは、Couchbase Serverをインストールする際に、他のコマンドラインツール同様、インストールされます。

また、以下の方法で、Couchbase Serverがインストールされていない環境にC SDKをインストールし、これらのユーティリティを用いて、リモートのクラスターに対して操作を行うことができます。

//blankline

@<strong>{CentOS/RHEL}では、以下のようにインストールします。

//cmd{
$ sudo yum check-update
$ sudo yum search libcouchbase
$ sudo yum install libcouchbase3 libcouchbase-devel libcouchbase3-tools \
  libcouchbase3-libevent
//}

@<strong>{Mac}では、以下のようにインストールします。

//cmd{
$ brew update
$ brew install libcouchbase
//}

@<strong>{Windows}については、バイナリをダウンロードして利用します。詳細は、ドキュメント@<fn>{install-windows}を参照してください。



その他のOSについては、リリースノート@<fn>{sdk-release-notes}を参照してください。

//footnote[install-windows][https://docs.couchbase.com/c-sdk/current/project-docs/sdk-release-notes.html#install-windows]

//footnote[sdk-release-notes][https://docs.couchbase.com/c-sdk/current/project-docs/sdk-release-notes.html]

=== 共通オプション

@<tt>{libcouchbase}コマンドの共通オプションとして、以下があります。

//table[共通オプション][]{
オプション	省略形	説明 
----------------------------
--spec	-U	接続文字列 [デフォルト: couchbase://localhost/default]
--username	-u	ユーザー名
--password	-P	パスワード
--timings	-T	終了時に実行されたコマンドの待ち時間を示すヒストグラムを表示
--verbose	-v	デバッグ出力を有効にする
--cparam	-D	追加のクライアントオプション指定
--compress	-y	送信データの圧縮をオンにする
--truststorepath	なし	トラストストアへのパス
--certpath	なし	サーバーSSL証明書へのパス
--keypath	なし	クライアントSSL秘密鍵へのパス
--dump	なし	操作完了後に詳細な内部状態をダンプする
//}

=== クライアントオプション

追加のクライアントオプションを、キーと値のペアとして、Dスイッチ（@<tt>{-D})とともに指定することができます。下記に記述例を示します。

//blankline

 @<tt>{-Doption1=value1 -Doption2=value}

//blankline

あるいは、以下のように、接続文字列に含めて利用することができます。

//blankline

@<tt>{couchbase://host/bucket?option1=value1&option2=value2}

//blankline

クライアントオプションには、たとえば以下のものがあります。

//blankline

 * @<tt>{operation_timeout=SECONDS}: タイムアウト秒数を指定します。これは、クライアントが操作の完了を待つ時間です。デフォルトは2.5秒です。
 * @<tt>{config_cache=PATH}: クライアントからクラスターへの接続時に、ファイルベースの構成キャッシュを利用できるようにします。ファイルが存在しない場合、クライアントがクラスターに接続した際に、ブートストラップ情報をファイルにキャッシュします。

//blankline

@<tt>{libcouchbase}の詳細については、Githubリポジトリー(Couchbase C Client@<fn>{github-libcouchbase})を参照してください。

//footnote[github-libcouchbase][https://github.com/couchbase/libcouchbase]

== ストレステストツール

=== 概要

データベースを運用するに当たって、ベンチマーク(ストレス)テストは重要な要素です。

Couchbase Serverでは、DataサービスとQueryサービスという２種類のサービスにより、データアクセス方法を提供していますが、それぞれのサービスに対して、下記のように異なるツールが用意されています。

//blankline

//table{
サービス	ツール
----------------------------
Dataサービス	pillowfight
Queryサービス	n1qlback
//}



=== pillowfight

@<tt>{pillowfight}@<fn>{md_doc_cbc-pillowfight}を使って、Dataサービスに対する、ドキュメント操作のストレステストを実施することができます。

//footnote[md_doc_cbc-pillowfight][https://docs.couchbase.com/sdk-api/couchbase-c-client-3.2.0/md_doc_cbc-pillowfight.html]

@<tt>{pillowfight}のオプションを示します。

//table[pillowfightオプション][]{
オプション	省略形	説明 
----------------------------
--batch-size	-B	バッチ処理の数 [デフォルト: 100] 
--num-items	-I	アイテムの数 [デフォルト: 1000]
--key-prefix	-p	キー接頭辞の指定
--num-threads 	-t	スレッドの数 [デフォルト: 1]
--random-body	-R	ドキュメント本文をランダム化する [デフォルト: FALSE]
--set-pct	-r	ミューテーション操作の割合 [デフォルト: 33]
--no-population	-n	ポピュレーション（入力）をスキップする[デフォルト: FALSE]
--min-size	-m	最小データ(ドキュメント)サイズ [デフォルト: 50]
--max-size	-M	最大データ(ドキュメント)サイズ [デフォルト: 5120]
--pause-at-end	-E	実行終了時に一時停止する（ユーザー入力まで接続を保持する)[デフォルト: FALSE]
--num-cycles	-c	終了するまで実行されるサイクル数。-1に設定すると、無限にループする[デフォルト: -1 ]
--sequential	なし	(ランダムアクセスでなく)シーケンシャルアクセスを使用[デフォルト: FALSE]
--start-at	なし	シーケンシャルアクセスの場合に、最初の値を設定[デフォルト: 0]
--docs	なし	ロードするユーザードキュメント（@<tt>{--min-s-ize} と@<tt>{--max-size} を上書き）
--json	-J	(bytesではなく)JSONデータの書き込みを有効にする[デフォルト: FALSE]
--subdoc	なし	fulldoc操作の代わりにsubdoc操作を使用[デフォルト: FALSE]
--noop	なし	ドキュメント操作の代わりに、NOOPオペレーション(軽量ワークロード)を使用[デフォルト: FALSE]
--pathcount	なし	コマンドあたりのサブドキュメントパスの数[デフォルト: 1]
--populate-only	なし	ドキュメントのポピュレーション（入力）のみを実施し、終了する[デフォルト: FALSE]
--expiry	-e	アイテムのTTLを設定 [デフォルト: 0]
--collection	なし	コレクションの（スコープを含む）フルパス（複数指定可能）
--durability	-d	耐久レベル [デフォルト: @<tt>{none}]
--persist-to	なし	アイテムが指定した数のノードに永続化されるまでブロックする（"-1"の場合、対象となるすべてのノード）[デフォルト: 0]
--replicate-to	なし	アイテムが指定した数のノードに複製されるまでブロックする（"-1"の場合、すべてのレプリカ） [デフォルト: 0]
--lock	なし	更新用ロックキーへの時間設定(ゼロの場合、ロックされない）[デフォルト: 0]
--help 	-？	ヘルプメッセージを表示する
//}

//blankline

@<tt>{pillowfight}の実行例を示します。

//blankline

@<strong>{１スレッド当たり１バッチ処理を20スレッドで実行}

//cmd{
$ cbc-pillowfight -t 20 -B 1 -u Administrator -P password
//}

@<strong>{50アイテムのデータセットを使った100回のイテレーション}

//cmd{
$ cbc-pillowfight -c 100 -I 50 -u Administrator -P password 
//}

@<strong>{サイズ100kのJSONドキュメントを利用}

//cmd{
$ cbc-pillowfight --json -m 100000 -M 100000 -u Administrator -P password
//}

@<strong>{JSONドキュメントの有効期限を10秒に設定}

//cmd{
$ cbc-pillowfight -U couchbase://localhost/pillow --json -e 10 \
  -u Administrator -P password
//}

最後の例では、接続先を明示的に指定しています。その他の例では、デフォルトの接続先（@<tt>{couchbase://localhost/default}）が利用されています。

=== n1qlback

@<tt>{n1qlback}@<fn>{md_doc_cbc-n1qlback}を使って、Queryサービスに対する、クエリ操作のストレステストを実施することができます。


//footnote[md_doc_cbc-n1qlback][https://docs.couchbase.com/sdk-api/couchbase-c-client-3.2.0/md_doc_cbc-n1qlback.html]

@<tt>{n1qlback}は、指定された数のスレッドを作成し、それぞれがユーザー定義のクエリのセットを実行します。

@<tt>{n1qlback}では、実行するクエリを含むファイルへのパスを渡します。このファイルでは、下記のようなJSON形式でクエリリクエストを表現し、1行に1リクエストを記載します。

単純なクエリの場合、@<tt>{statement}フィールドのみを設定します。

//emlist[][]{
{"statement":"SELECT country FROM `travel-sample`"}
{"statement":"SELECT country, COUNT(country) FROM `travel-sample` GROUP BY country"}
//}

次の例は、名前付きパラメータの使用方法を示しています。

//emlist[][]{
{"statement":"SELECT RAW META().id FROM `travel-sample` WHERE type=$type LIMIT 1", "$type":"airline"}
//}


@<tt>{n1qlback}のオプションを示します。

//blankline

 * @<tt>{-f}, @<tt>{--queryfile=PATH}: 実行するクエリをJSON形式で記述したファイルへのパスを指定します。
 * @<tt>{-t}, @<tt>{--num-threads=NTHREADS}: 同時に実行するスレッド数(クライアントインスタンスの数)を設定します。
 * @<tt>{-e}, @<tt>{--error-log=PATH}: エラーログを出力するファイルへのパスを指定します。失敗したクエリとエラーの詳細がファイルへ書き込まれます。

//blankline

最後に、@<tt>{n1qlback}の実行例を示します。

以下の例では、クエリリクエストを記載したファイル@<tt>{queries.txt}を指定し、5つのスレッドで実行しています。

//cmd{
$ cbc-n1qlback -U couchbase://localhost/travel-sample \
  -t 5 -f queries.txt -u Administrator -P password \
  -e n1qlback.err
//}

== その他のツール

@<tt>{pillowfight}と@<tt>{n1qlback}以外に、以下のツールがあります。

ここでは紹介にとどめますが、詳細は、各ツールのAPI Docを参照してください。

//blankline

 * @<tt>{cbc}@<fn>{md_doc_cbc}: クラスターと通信するためのユーティリティ
 * @<tt>{cbc-subdoc}@<fn>{md_doc_cbc-subdoc}: サブドキュメントAPIに由来するコマンドを用いてデータにアクセスすることのできるインタラクティブシェルを実行するためのユーティリティ
 * @<tt>{cbcrc}@<fn>{md_doc_cbcrc}: @<tt>{cbc}と@<tt>{cbc-pillowfight}のためのデフォルト値を定義するために利用される、オプションの構成ファイル


//footnote[md_doc_cbc][https://docs.couchbase.com/sdk-api/couchbase-c-client-3.2.1/md_doc_cbc.html]

//footnote[md_doc_cbc-subdoc][https://docs.couchbase.com/sdk-api/couchbase-c-client-3.2.1/md_doc_cbc-subdoc.html]

//footnote[md_doc_cbcrc][https://docs.couchbase.com/sdk-api/couchbase-c-client-3.2.1/md_doc_cbcrc.html]