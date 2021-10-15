= コマンドライン操作

== 基本

Couchbase Serverには、様々なコマンドラインツールが用意されています。
ここでは、それらの中から主なものについて紹介します。すべてのツールに関する詳細な情報については、ドキュメント@<fn>{cli-intro}を参照してください。


//footnote[cli-intro][https://docs.couchbase.com/server/current/cli/cli-intro.html]


=== インストール場所

Couchbase Serverをインストールした際に同時にインストールされるコマンドは、下記のパス以下(ほとんどは@<tt>{bin}直下、一部@<tt>{tools}や、@<tt>{install}ディレクトリーの下)に存在します。

 * @<strong>{Linux}: /opt/couchbase/bin
 * @<strong>{Windows}: C:\Program Files\couchbase\server\bin
 * @<strong>{Mac OS X}: /Applications/Couchbase Server.app/Contents/Resources/couchbase-core/bin


本章の実行例では、コマンドへのパスが通っていることを前提としています。



== クラスター管理: couchbase-cli


@<tt>{couchbase-cli}はCouchbase Serverの管理のため提供されている、中心となるコマンドラインインターフェイスです。


=== 基本構文

基本的な構文は以下の通りです。

//emlist[][shell]{
couchbase-cli <サブコマンド> [オプション]
//}

=== サブコマンド

@<tt>{couchbase-cli}で利用可能なサブコマンドを紹介します。

@<tt>{bucket-}で始まる、バケット管理のための各種サブコマンドが用意されています。
また、@<tt>{collection-manage}サブコマンドを用いて、スコープやコレクションの作成、削除を行うことができます。

その他、クラスターの管理のための様々なサブコマンドが存在します。

これらの操作のほとんどは、Webコンソールを使って行うことができ、特にCouchbase Serverを使い始めた人にとっては、そちらの方が直感的に利用することができるでしょう。
一方、コマンドライン操作という選択肢が用意されていることは、本格的な運用にとって、操作の再現性などの面で重要です。

ここでは、可能な操作の紹介を意図しています。利用方法については、ドキュメント@<fn>{couchbase-cli}を参照してください。

//table{
サブコマンド	内容
----------------------------
admin-role-manage	LDAPユーザーロールを管理します。
bucket-compact	バケットのコンパクションを実行します。
bucket-create	新しいバケットを作成します。
bucket-delete	バケットを削除します。
bucket-edit	バケット設定を編集します。
bucket-flush	バケットのデータをフラッシュします。
bucket-list	すべてのバケットを一覧表示します。
cluster-edit	クラスター設定を編集します。
node-to-node-encryption	ノード間暗号化の有効化/無効化を設定します。
cluster-init	クラスターを初期化します。
ip-family	クラスターで使われているIPファミリーを変更します。
collect-logs-start	ログ収集を開始します。
collect-logs-status	ログ収集ステータスを取得します。
collect-logs-stop	現在のログ収集タスクを停止します。
eventing-function-setup	Eventingサービス機能を管理します。
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
//}

//blankline

//table{
サブコマンド	内容
----------------------------
setting-audit	監査ログの設定を変更します。
setting-autofailover	自動フェイルオーバー設定を変更します。
setting-autoreprovision	自動再プロビジョニングの設定を変更します。
setting-cluster	クラスター設定を変更します。
setting-compaction	圧縮設定を変更します。
setting-index	インデックス設定を変更します。
setting-ldap	LDAP設定を変更します。
setting-notification	通知設定を変更します。
setting-password-policy	パスワードポリシーを変更します。
setting-query	クエリ設定を変更します。
setting-rebalance	リバランス設定を変更します。
setting-security	セキュリティーポリシーを変更します。
setting-xdcr	クロスデータセンターレプリケーション（XDCR）設定を変更します。
ssl-manage	SSL証明書を管理します。
user-manage	RBACユーザーを管理します。
xdcr-replicate	XDCRクラスター参照を管理します。
xdcr-setup	XDCRレプリケーションを管理します。
setting-saslauthd	saslauthd設定を管理します。
enable-developer-preview	開発者プレビューを有効にします。
collection-manage	コレクションを管理します。
//}



//footnote[couchbase-cli][https://docs.couchbase.com/server/current/cli/cbcli/couchbase-cli.html]


== クエリ: cbq

@<tt>{cbq}を用いて、コマンドラインから、N1QLクエリを実行することができます。


=== 実行例


@<tt>{--script}オプションを用いて、下記のように、クエリを実行することが可能です。

//cmd{
$ cbq -u Administrator -p password -e "http://localhost:8091" \
--script="SELECT * FROM `travel-sample`.inventory.airline LIMIT 1;"
//}

@<tt>{--script}オプションを用いずに、下記のように、インタラクティブシェルモードで利用することも可能です。

//cmd{
$ cbq -u Administrator -p password -e "http://localhost:8091"
 Connected to : http://localhost:8091/. Type Ctrl-D or \QUIT to exit.

 Path to history file for the shell : /Users/<user name>/.cbq_history 
cbq>SELECT * FROM `travel-sample`.inventory.airline LIMIT 1;
//}

オプションや、シェルコマンドの詳細についてはドキュメント@<fn>{cbq-shell}を参照してください。

//footnote[cbq-shell][https://docs.couchbase.com/server/current/tools/cbq-shell.html]

== データのインポート: cbimport

=== 概要

@<tt>{cbimport}を用いて、Couchbase Serverへファイルからデータをインポートをすることができます。
入力に用いるファイルのフォーマットとして、CSVとJSONをサポートしています。

ここでは、基本的な機能を紹介し、最後に、CSVファイルからデータをインポートするケースについて、利用例を示して解説します。

詳細については、ドキュメント@<fn>{cbimport}を参照してください。

//footnote[cbimport][https://docs.couchbase.com/server/current/tools/cbimport.html]


=== 基本構文

//emlist[][]{
cbimport [--version] [--help] <csv|json> [<args>]
//}


//emlist[][]{
cbimport csv [--cluster <url>] [--bucket <bucket_name>] [--dataset <path>]
             [--username <username>] [--password <password>] [--generate-key <key_expr>]
             [--limit-rows <num>] [--skip-rows <num>] [--field-separator <char>]
             [--cacert <path>] [--no-ssl-verify] [--threads <num>] [--error-log <path>]
             [--log-file <path>] [--verbose] [--field-delimiter <char>]
             [--generator-delimiter <char>] [--ignore-fields <fields>]
             [--scope-collection-exp <scope_collection_expression>]
//}

//emlist[][]{
cbimport json [--cluster <url>] [--bucket <bucket_name>] [--dataset <path>]
              [--format <data_format>][--username <username>] [--password <password>]
              [--generate-key <key_expr>][--cacert <path>][--no-ssl-verify]
              [--threads <num>] [--error-log <path>][--log-file <path>] [--verbose]
              [--ignore-fields <fields>] [--field-delimiter <char>]
              [--generator-delimiter <char>]
              [--scope-collection-exp <scope_collection_expression>]
              [--bucket-quota <quota>]
//}


=== ホストフォーマット

@<tt>{cbimport}(および@<tt>{cbexport})コマンドに対して、ホスト名を指定する際には、以下のフォーマットが利用可能です。

 * @<tt>{<addr>:<port>}
 * @<tt>{http://<addr>:<port>}
 * @<tt>{https://<addr>:<port>}
 * @<tt>{couchbase://<addr>:<port>}
 * @<tt>{couchbases://<addr>:<port>}
 * @<tt>{couchbase://<srv>}
 * @<tt>{couchbases://<srv>}
 * @<tt>{<addr>:<port>,<addr>:<port>}
 * @<tt>{<schema>:<addr>:<port>,<addr>:<port>}

末尾に@<tt>{s}のついた@<tt>{couchbases://}は、(@<tt>{http://}と@<tt>{https://}の関係と同様)、セキュアな接続を意味します。

ホストフォーマットでは、複数のノードを指定したり、SRVレコードを用いることができます。
詳細な解説や有効な接続文字列の実例について、ドキュメント@<fn>{cbimport-host-formats}を参照してください。

//footnote[cbimport-host-formats][https://docs.couchbase.com/server/current/tools/cbimport-csv.html#host-formats]


=== JSONデータセットフォーマット

@<tt>{json}サブコマンドを用いて実行される場合、@<tt>{cbimport}は、以下のフォーマットを持つファイルを入力として扱うことが可能です。

//blankline


@<strong>{LINES}

@<tt>{-format}オプションとして、「lines」を指定します。

各行に、ひとつのJSONオブジェクトが記載されているファイルを入力とします。

以下に例を示します。


//emlist[][]{
{"key": "mykey1", "value": "myvalue1"}
{"key": "mykey2", "value": "myvalue2"}
{"key": "mykey3", "value": "myvalue3"}
{"key": "mykey4", "value": "myvalue4"}
//}

@<strong>{LIST}

@<tt>{-format}オプションとして、「list」を指定します。

各要素がJSONドキュメントである配列を含むファイルを入力とします。

ファイルには単一の配列のみを含めることができます。JSONデータとして成立している限り、配列の要素がファイル上１行である必要はなく、複数行で指定できます。

以下に例を示します。

//emlist[][]{
[
  {
    "key": "mykey1",
    "value": "myvalue1"
  },
  {"key": "mykey2", "value": "myvalue2"},
  {"key": "mykey3", "value": "myvalue3"},
  {"key": "mykey4", "value": "myvalue4"}
]
//}

=== CSVデータセットフォーマット

@<tt>{csv}サブコマンドを用いて実行される場合、@<tt>{cbimport}は、CSVファイルを入力とします。

JSONデータに変換するために、フィールド名が必須になるため、CSVファイルの１行目はデータではなく、フィールド(カラム)名である必要があります。

デフォルトでは、カンマ区切りが想定されていますが、@<tt>{--field-separator}フラグを使用して、別の文字を区切り文字として扱うことが可能です。


=== ドキュメントキー

ファイルからデータをインポートする際、@<tt>{--generate-key}(@<tt>{-g})オプションを利用して、ドキュメントのキーとして用いる項目を指定することができます。

指定方法は、@<tt>{%user_id%}のように、フィールド名の前後を@<tt>{%}で修飾します。

また、@<tt>{#UUID#}というキーワードを用いて、キーを自動的に生成することもできます。キーは、@<tt>{5565431f-9e0e-480d-abd6-c844a3ff6f9b}のようなUUIDになります。

=== インポート先の指定

ファイルからデータをインポートする際、@<tt>{-b}オプションにより、インポート先のバケット名を指定します。

また、ドキュメントを保存するスコープやコレクションを指定する@<tt>{--scope-collection-exp}オプションがあります。
このオプションでは、スコープとコレクションの指定に、「<スコープ名>.<コレクション名>」のようにドット表記を用います。
入力データ中の特定のフィールド(カラム)の値と同じ名前のスコープやコレクションを保存先に指定することができ、その場合には、@<tt>{--generate-key}(@<tt>{-g})オプション同様に、フィールド名の前後を@<tt>{%}で修飾したものを用います。

=== データ型推論

@<tt>{--infer-types}を指定すると、フィールド(カラム)の値からデータ型を推論して、対応するJSONのデータ型へ変換することができます。

=== 利用例

下記のようなデータファイルがあるとします。

//emlist{
user_id,region,name
"001",japan,田中
"002",japan,佐藤
"011",us,Smith
//}

以下のようなコマンドを用いて、このデータをインポートすることができます。

//cmd{
$ cbimport csv -c http://localhost:8091 -u Administrator -p password \
  -d file://user_data.csv -b user_management \
  -g %user_id% --scope-collection-exp %region%.users
//}

@<tt>{-g}(@<tt>{--generate-key})オプションとして、データに含まれる@<tt>{user_id}が(@<tt>{%}で修飾して)指定されている他、@<tt>{--scope-collection-exp}オプションで、スコープ名に、@<tt>{region}が指定されているのがわかります。
@<tt>{--infer-types}を指定していないので、@<tt>{user_id}は、文字列として扱われます。


== データのエクスポート:cbexport

=== 概要

@<tt>{cbexport}を用いて、Couchbase Serverからデータをファイルにエクスポートすることができます。
出力されるファイルのフォーマットはJSONです。

ここでは、基本的な機能を紹介します。詳細については、ドキュメント@<fn>{cbexport-json}を参照してください。

//footnote[cbexport-json][https://docs.couchbase.com/server/current/tools/cbexport-json.html]


=== 基本構文

//emlist[][]{
cbexport json [--cluster <url>] [--bucket <bucket_name>] [--format <data_format>]
              [--username <username>] [--password <password>] [--cacert <path>]
              [--no-ssl-verify] [--threads <num>] [--log-file <path>]
              [--include-key <key>] [--include-data <collection_string_list>]
              [--exclude-data <collection_string_list>] [--verbose]
              [--scope-field <scope_field>] [--collection-field <collection_field>]
//}

=== データセットフォーマット

@<tt>{cbexport}は、@<tt>{cbimport}におけるJSONデータの扱いと同様に、以下のフォーマットでファイルを出力することが可能です。

 * @<strong>{LINES} 各行が、ひとつのJSONオブジェクト
 * @<strong>{LIST} JSONオブジェクトを要素として持つ、ひとつの配列

== バックアップ:cbbackupmgr

データベースにとって、バックアップは必須の機能です。

Couchbase Serverのバックアップのために用意されている、@<tt>{cbbackupmgr}コマンドの使用例を紹介します。

ここで紹介しきれない使用可能なオプションや、全てのオプションの詳細については、ドキュメント@<fn>{cbbackupmgr-doc}を参照してください。


本セクションの内容は、公式ドキュメントのチュートリアル@<fn>{cbbackupmgr-tutorial}に基づいています。
説明にあたって、Couchbase Serverにサンプルデータとして準備されている@<tt>{travel-sample}バケットと@<tt>{beer-sample}バケットを利用します。


//footnote[cbbackupmgr-tutorial][https://docs.couchbase.com/server/current/backup-restore/cbbackupmgr-tutorial.html]

=== バックアップリポジトリーの作成


@<tt>{cbbackupmgr}の使用を開始するにあたって、まずバックアップを保存するディレクトリーを決定する必要があります。このディレクトリーは、@<strong>{バックアップアーカイブ}と呼ばれます。

バックアップアーカイブには、@<strong>{バックアップリポジトリー}が含まれます。
バックアップリポジトリーは、@<tt>{config}サブコマンドを使用して作成します。

このチュートリアルでは、@<tt>{/data/backup}をバックアップアーカイブとして使用します。
バックアップアーカイブには、空のディレクトリーを選択します。
@<tt>{cbbackupmgr}コマンド初回実行時に、バックアップアーカイブとして構成されます。

バックアップリポジトリーを構成する最も単純な方法は、クラスター全体をバックアップ対象とすることです。
以下のように、バックアップ対象を指定するオプションを用いていない場合、クラスター全体がバックアップ対象となります。

ここでは、「cluster」という名前でバックアップリポジトリーを作成します。


//cmd{
$ cbbackupmgr config -a /data/backup -r cluster

Backup repository `cluster` created successfully in archive `/data/backup`
//}


バックアップリポジトリー「cluster」がアーカイブ@<tt>{/data/backup}に正常に作成されました


バックアップリポジトリーを作成する際に、バックアップの対象を選択することができます。
たとえば、@<tt>{travel-sample}バケット内のインデックス定義のみをバックアップするとします。
以下のように、@<tt>{--include-data}オプションを用いて、バックアップ対象のバケットを指定することができます。
また、@<tt>{--disable-data}オプションにより、データのバックアップを無効化することにより、インデックス定義のみがバックアップされます。

ここでは「single」という名前でバックアップリポジトリーを作成します。


//cmd{
$ cbbackupmgr config -a /data/backup -r single \
  --include-data travel-sample --disable-data

Backup repository `single` created successfully in archive `/data/backup`
//}



@<tt>{config}サブコマンドは、データのバックアップ方法をカスタマイズするための多くのオプションを提供します。



=== バックアップ構成の確認


バックアップリポジトリーを作成したので、バックアップアーカイブの情報を確認します。
そのために、@<tt>{info}サブコマンドを使用することができます。



//cmd{
$ cbbackupmgr info -a /data/backup --all
Name         | UUID                                  | Size  | # Repos  |
backup_repo  | 32c97d5f-821a-4284-840b-9ee7cf8733a3  | 0B    | 2        |
*  Name     | Size  | # Backups  |
*  cluster  | 0B    | 0          |
//}


@<tt>{info}サブコマンドは、バックアップアーカイブ内のすべてのバックアップリポジトリーとバックアップの情報を表示します。

ここではバックアップはまだ作成されていないので、バックアップリポジトリーの情報のみが確認できます。


=== バックアップの作成


バックアップの作成を開始します。バックアップリポジトリーには、バックアップの作成方法に関する情報が含まれています。そのため、バックアップ作成時にはバックアップアーカイブの場所と、バックアップリポジトリー(の名前)、そしてバックアップするターゲットクラスターの情報(認証情報を含む)を指定します。

「cluster」バックアップリポジトリーで、ローカルホストで稼働しているクラスターのバックアップを作成します。


//cmd{
$ cbbackupmgr backup -c 127.0.0.1 -u Administrator -p password -a /data/backup -r cluster
Backing up to 2020-03-25T08_08_11.770436Z
Copied all data in 33.02s (Avg. 759.44KB/Sec)  38894 items / 24.47MB
beer-sample             [===================================] 100.00%
travel-sample           [===================================] 100.00%

Backup successfully completed
Backed up bucket "beer-sample" succeeded
Mutations backed up: 7303, Mutations failed to backup: 0
Deletions backed up: 0, Deletions failed to backup: 0
Backed up bucket "travel-sample" succeeded
Mutations backed up: 31591, Mutations failed to backup: 0
Deletions backed up: 0, Deletions failed to backup: 0
//}


コマンドを実行すると、プログレスバーが出力されます。これは、バックアップの完了にかかる時間とデータ移動の速度を理解するのに役立ちます。
バックアップの実行中、完了までの推定時間が示されます。
すでにバックアップされているデータとアイテムの合計、および現在のデータ移動速度に関する情報も提供されます。

バックアップが正常に完了すると、「Backup successfully completed」というメッセージが表示され、バックアップされた変更(Mutations)と削除(Deletions)の内訳がバケットごとに表示されます。


「single」バックアップリポジトリーでも、バックアップを実行して、ふたつのバックアップ実行の違いを確認してみましょう。



//cmd{
$ cbbackupmgr backup -a /data/backup -r single \
  -c couchbase://127.0.0.1 -u Administrator -p password

Backing up to 2020-03-25T08_08_58.903046Z
Copied all data in 1s (Avg. 480B/Sec)                 0 items / 480B
travel-sample           [==================================] 100.00%
Backup successfully completed
Backed up bucket "travel-sample" succeeded
Mutations backed up: 0, Mutations failed to backup: 0
Deletions backed up: 0, Deletions failed to backup: 0
//}


「single」バックアップリポジトリーは、@<tt>{travel-sample}バケットからインデックス定義のみをバックアップするように構成されているため、@<tt>{beer-sample}バケットの進行状況バーは表示されていません。
また、バックアップするデータがはるかに少ないため、バックアップがより迅速に実行されたことがわかります。



これで、バックアップアーカイブにバックアップが作成されたので、@<tt>{info}サブコマンドを使用してバックアップアーカイブの状態を確認してみましょう。



//cmd{
$ cbbackupmgr info -a /data/backup --all
Name         | UUID                                  | Size     | # Repos  |
backup_repo  | 32c97d5f-821a-4284-840b-9ee7cf8733a3  | 54.35MB  | 2        |

*  Name     | Size     | # Backups  |
*  cluster  | 54.33MB  | 1          |

+    Backup                       | Size     | Type  | Source                 | Cluster UUID                      | Range  | Events  | Aliases  | Complete  |
+    2020-03-25T08_08_11.770436Z  | 54.33MB  | FULL  | http://localhost:9000  | ffa8024f899ca6acc7c59cf0f541dbdd  | N/A    | 0       | 0        | true      |

-      Bucket       | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      beer-sample  | 18.43MB  | 7303   | 7303       | 0           | 1      | 0    | 1        | 0     |

-      Bucket         | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      travel-sample  | 35.91MB  | 31591  | 31591      | 0           | 0      | 0    | 10       | 0     |

*  Name    | Size     | # Backups  |
*  single  | 16.20KB  | 1          |

+    Backup                       | Size     | Type  | Source                 | Cluster UUID                      | Range  | Events  | Aliases  | Complete  |
+    2020-03-25T08_08_58.903046Z  | 16.20KB  | FULL  | http://localhost:9000  | ffa8024f899ca6acc7c59cf0f541dbdd  | N/A    | 0       | 0        | true      |

-      Bucket         | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      travel-sample  | 16.20KB  | 0      | 0          | 0           | 0      | 0    | 10       | 0     |
//}


いくつかのバックアップが作成されたので、@<tt>{info}サブコマンドの出力内容が変わっています。「cluster」バックアップリポジトリーには、バックアップが作成された時間に対応する名前のバックアップがひとつ含まれていることがわかります。
そのバックアップにはふたつのバケットが含まれており、バケット内のミューテーション、トゥームストーン、インデックスなどの数を確認できます。
「single」バックアップリポジトリーにはひとつのバックアップが含まれています。このバックアップには@<tt>{travel-sample}バケットのみが含まれ、データ項目は0個で、10個のインデックスがあることがわかります。

なお、@<tt>{info}コマンドはJSONフォーマットでの出力もサポートしています。


=== 増分バックアップ


@<tt>{cbbackupmgr}の重要な特徴として、増分バックアップがあります。
２回目以降のバックアップでは、バックアップリポジトリーに既に存在しているデータは、バックアップの対象とはなりません。

増分バックアップの確認のため、一部のデータを変更した後、「cluster」バックアップリポジトリーで@<tt>{backup}サブコマンドを再度実行してみます。



//cmd{
$ cbbackupmgr backup -a /data/backup -r cluster -c couchbase://127.0.0.1 -u Administrator -p password

Backing up to 2020-03-25T08_41_21.461311Z
Copied all data in 3s (Avg. 18.98KB/Sec)           4 items / 56.95KB
travel-sample           [==================================] 100.00%
beer-sample             [==================================] 100.00%

Backup successfully completed
Backed up bucket "beer-sample" succeeded
Mutations backed up: 0, Mutations failed to backup: 0
Deletions backed up: 0, Deletions failed to backup: 0
Backed up bucket "travel-sample" succeeded
Mutations backed up: 4, Mutations failed to backup: 0
Deletions backed up: 0, Deletions failed to backup: 0
//}


この実行例では、バックアップ実行前にふたつのアイテムを更新して、ふたつのアイテムを新規作成しています。
バックアップされたのはそれらのデータのみであることがわかります。

@<tt>{info}サブコマンドを使用して、バックアップアーカイブの情報を確認します。
新しくバックアップが追加されており、そのバックアップの@<tt>{Items}の数は4で、そのすべてが@<tt>{Mutations}であることがわかります。これは、ふたつの変更とふたつの追加に対応しています。



//cmd{
$ cbbackupmgr info -a /data/backup --all
Name         | UUID                                  | Size     | # Repos  |
backup_repo  | 32c97d5f-821a-4284-840b-9ee7cf8733a3  | 86.37MB  | 2        |

*  Name     | Size     | # Backups  |
*  cluster  | 86.35MB  | 2          |

+    Backup                       | Size     | Type  | Source                 | Cluster UUID                      | Range  | Events  | Aliases  | Complete  |
+    2020-03-25T08_08_11.770436Z  | 54.33MB  | FULL  | http://localhost:9000  | ffa8024f899ca6acc7c59cf0f541dbdd  | N/A    | 0       | 0        | true      |

-      Bucket       | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      beer-sample  | 18.43MB  | 7303   | 7303       | 0           | 1      | 0    | 1        | 0     |

-      Bucket         | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      travel-sample  | 35.91MB  | 31591  | 31591      | 0           | 0      | 0    | 10       | 0     |

+    Backup                       | Size     | Type  | Source                 | Cluster UUID                      | Range  | Events  | Aliases  | Complete  |
+    2020-03-25T08_41_21.461311Z  | 32.02MB  | INCR  | http://localhost:9000  | ffa8024f899ca6acc7c59cf0f541dbdd  | N/A    | 0       | 0        | true      |

-      Bucket       | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      beer-sample  | 16.00MB  | 0      | 0          | 0           | 1      | 0    | 1        | 0     |

-      Bucket         | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      travel-sample  | 16.02MB  | 4      | 4          | 0           | 0      | 0    | 10       | 0     |

*  Name    | Size     | # Backups  |
*  single  | 16.20KB  | 1          |

+    Backup                       | Size     | Type  | Source                 | Cluster UUID                      | Range  | Events  | Aliases  | Complete  |
+    2020-03-25T08_08_58.903046Z  | 16.20KB  | FULL  | http://localhost:9000  | ffa8024f899ca6acc7c59cf0f541dbdd  | N/A    | 0       | 0        | true      |

-      Bucket         | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      travel-sample  | 16.20KB  | 0      | 0          | 0           | 0      | 0    | 10       | 0     |
//}



=== バックアップの復元


バックアップデータができたので、そのバックアップデータをクラスターに復元してみましょう。
データを復元するには、復元するバックアップの名前を知っている必要があります。名前を確認するには、@<tt>{info}サブコマンドを使用して、バックアップアーカイブの内容を表示します。
バックアップ名は常にタイムスタンプになります。

ここでは、「cluster」バックアップリポジトリーの１回目のバックアップから復元します。


//cmd{
$ cbbackupmgr restore -a /data/backup -r cluster \
 -c http://127.0.0.1:8091 -u Administrator -p password \
 --start 2020-03-25T08_08_11.770436Z \
 --end 2020-03-25T08_08_11.770436Z --force-updates

(1/1) Restoring backup 2020-03-25T08_08_11.770436Z
Copied all data in 2s (Avg. 19.96MB/Sec)       38894 items / 39.91MB
travel-sample           [==================================] 100.00%
beer-sample             [==================================] 100.00%

Restore completed successfully
//}


復元を行う際には、@<tt>{--start}フラグと@<tt>{--end}フラグを使用して、復元するバックアップの範囲を指定します。
ここでは、バックアップをひとつだけ復元するため、@<tt>{--start}と@<tt>{--end}の両方に同じものを指定しています。

また、@<tt>{--force-updates}フラグが利用されています。これにより、競合解決プロセスがスキップされ、クラスター上の値が新しく更新されている場合でも、バックアップされたデータで強制的に上書きします。
ここで、２回目のバックアップ前に更新したふたつの値を確認すると、最初のバックアップを作成したときの値に戻っていることがわかります。

@<tt>{restore}サブコマンドでは、バックアップデータの一部を復元から除外するなど、さまざまなオプションを利用することができます。


=== バックアップのマージ



増分バックアップを行うと、バックアップを作成するたびにディスク容量が増加します。ディスクスペースは無限ではないため、スペースを再利用できるようにすることが重要です。

@<tt>{merge}サブコマンドを使用してふたつ以上のバックアップをマージすることができます。

「cluster」バックアップリポジトリーにはふたつのバックアップがあるので、これらのバックアップをマージしてみます。


//cmd{
$ cbbackupmgr merge -a /data/backup -r cluster \
  --start oldest --end latest

Merge completed successfully
//}

====[column]エディションによる差異

@<tt>{merge}サブコマンドは、エンタープライズエディションでのみ利用できます。

====[/column]

バックアップのマージが成功したので、@<tt>{list}サブコマンドを使用して、@<tt>{merge}サブコマンドがバックアップアーカイブに与えた影響を確認します。



//cmd{
$ cbackupmgr info -a /data/backup --all                                                                                                                                                                                                                                                                                                                                                                                     20-03-25 - 8:52:36
Name         | UUID                                  | Size     | # Repos  |
backup_repo  | 32c97d5f-821a-4284-840b-9ee7cf8733a3  | 54.35MB  | 2        |

*  Name     | Size     | # Backups  |
*  cluster  | 54.33MB  | 1          |

+    Backup                       | Size     | Type          | Source  | Cluster UUID                      | Range                                      | Events  | Aliases  | Complete  |
+    2020-03-25T08_41_21.461311Z  | 54.33MB  | MERGE - FULL  | Merge   | ffa8024f899ca6acc7c59cf0f541dbdd  | 2020-03-25T08_08_11-2020-03-25T08_41_21:2  | 0       | 0        | true      |

-      Bucket       | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      beer-sample  | 18.43MB  | 7303   | 7303       | 0           | 1      | 0    | 1        | 0     |

-      Bucket         | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      travel-sample  | 35.91MB  | 31593  | 31593      | 0           | 0      | 0    | 10       | 0     |

*  Name    | Size     | # Backups  |
*  single  | 16.20KB  | 1          |

+    Backup                       | Size     | Type  | Source                 | Cluster UUID                      | Range  | Events  | Aliases  | Complete  |
+    2020-03-25T08_08_58.903046Z  | 16.20KB  | FULL  | http://localhost:9000  | ffa8024f899ca6acc7c59cf0f541dbdd  | N/A    | 0       | 0        | true      |

-      Bucket         | Size     | Items  | Mutations  | Tombstones  | Views  | FTS  | Indexes  | CBAS  |
-      travel-sample  | 16.20KB  | 0      | 0          | 0           | 0      | 0    | 10       | 0     |
//}


「cluster」バックアップリポジトリーに単一のバックアップがあることがわかります。このバックアップの名前は、最新のバックアップの名前を反映しています。

現在、@<tt>{travel-sample}バケットのバックアップには、31593個のデータ項目があります。
マージ前の2番目のバックアップには、ふたつの新しいアイテムが含まれていました。
そのため、最初のバックアップのデータ項目数(31591個)からふたつ増えています。
２番目のバックアップに含まれていた残りのふたつのアイテムは、新規アイテムではなく更新であるため、マージ中に重複が取り除かれ、マージ後のバックアップのアイテムの個数に影響を与えていません。



=== バックアップリポジトリーの削除


バックアップリポジトリーが不要になった場合は、@<tt>{remove}サブコマンドを使用してバックアップリポジトリーを削除できます。

以下は、「cluster」バックアップリポジトリーを削除する例です。


//cmd{
$ cbbackupmgr remove -a /data/backup -r cluster

Backup repository `cluster` deleted successfully from archive `/data/backup`
//}


バックアップリポジトリー「cluster」がアーカイブ@<tt>{/data/backup}から正常に削除されました。

ここで@<tt>{list}サブコマンドを実行すると、「cluster」バックアップリポジトリーが存在していないことを確認できます。



//footnote[cbbackupmgr-doc][https://docs.couchbase.com/server/current/backup-restore/cbbackupmgr.html]


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
$ sudo yum install libcouchbase3 libcouchbase-devel libcouchbase3-tools libcouchbase3-libevent
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

 * @<tt>{operation_timeout=SECONDS}: タイムアウト秒数を指定します。これは、クライアントが操作の完了を待つ時間です。デフォルトは2.5秒です。
 * @<tt>{config_cache=PATH}: クライアントからクラスターへの接続時に、ファイルベースの構成キャッシュを利用できるようにします。ファイルが存在しない場合、クライアントがクラスターに接続した際に、ブートストラップ情報をファイルにキャッシュします。

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
$ cbc-pillowfight -U couchbase://localhost/pillow --json -e 10 -u Administrator -P password
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

 * @<tt>{-f}, @<tt>{--queryfile=PATH}: 実行するクエリをJSON形式で記述したファイルへのパスを指定します。
 * @<tt>{-t}, @<tt>{--num-threads=NTHREADS}: 同時に実行するスレッド数(クライアントインスタンスの数)を設定します。
 * @<tt>{-e}, @<tt>{--error-log=PATH}: エラーログを出力するファイルへのパスを指定します。失敗したクエリとエラーの詳細がファイルへ書き込まれます。


最後に、@<tt>{n1qlback}の実行例を示します。

以下の例では、クエリリクエストを記載したファイル@<tt>{queries.txt}を指定し、5つのスレッドで実行しています。

//cmd{
$ cbc-n1qlback -U couchbase://localhost/travel-sample \
  -t 5 -f queries.txt -u Administrator -P password \
  -e n1qlback.err
//}

== その他のツール

@<tt>{pillowfight}と@<tt>{n1qlback}以外に、以下のツールがあります。

ここでは紹介に留めますが、詳細は、各ツールのAPI Docを参照してください。

 * @<tt>{cbc}@<fn>{md_doc_cbc}: クラスターと通信するためのユーティリティ
 * @<tt>{cbc-subdoc}@<fn>{md_doc_cbc-subdoc}: サブドキュメントAPIに由来するコマンドを用いてデータにアクセスすることのできるインタラクティブシェルを実行するためのユーティリティ
 * @<tt>{cbcrc}@<fn>{md_doc_cbcrc}: @<tt>{cbc}と@<tt>{cbc-pillowfight}のためのデフォルト値を定義するために利用される、オプションの構成ファイル


//footnote[md_doc_cbc][https://docs.couchbase.com/sdk-api/couchbase-c-client-3.2.1/md_doc_cbc.html]

//footnote[md_doc_cbc-subdoc][https://docs.couchbase.com/sdk-api/couchbase-c-client-3.2.1/md_doc_cbc-subdoc.html]

//footnote[md_doc_cbcrc][https://docs.couchbase.com/sdk-api/couchbase-c-client-3.2.1/md_doc_cbcrc.html]