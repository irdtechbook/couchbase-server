

= Couchbase Serverを使ってみる

 

== 環境構築手順

=== インストーラー利用

==== ダウンロード

Couchbase Serverのインストーラーは、@<href>{https://www.couchbase.com/downloads}からダウンロードすることが可能です。

ダウンロード時に、インストール先のOSとしてWindows、macOSに加え、様々なLinux系OSを選択可能です。


==== インストール手順

各OS毎のインストール手順は、ドキュメント@<fn>{install-intro}に詳述されています。

Linux系OSは、コマンドラインからパッケージシステムを介してインストールすることが可能です。インストール以降のクラスター構築手順は、OSの別に依らず、共通しています。

=== Docker利用

==== 初回実行

下記のコマンドを実行します。 ここでは、 @<tt>{--name} オプションとして @<tt>{cb}を指定しており、以降の操作でこの名称を利用します。

//cmd{
$ docker run -d --name cb -p 8091-8094:8091-8094 -p 11210:11210 couchbase:community
//}

CouchbaseのDockerリポジトリから最新版がダウンロードされ、コンテナの実行が開始されます。

@<href>{http://localhost:8091}にアクセスします。

下記の様な画面が表示されます。

//image[initial][initial]{
//}

==== ログの確認

下記以降のコマンドで用いている @<tt>{cb} は、@<tt>{docker run} コマンドの @<tt>{--name}オプションで指定したものです。

//cmd{
$ docker logs cb
//}

==== Dockerイメージの確認

//cmd{
$ docker images
//}

==== プロセスの確認

//cmd{
$ docker ps
//}

上記の手順により@<tt>{couchbase}REPOSITORYの@<tt>{community}というTAGを持つ、Dockerイメージがインストールされ、そのイメージから @<tt>{cb}という名前のコンテナが作られ、稼働中の状態であることが確認されます。

==== コンテナの停止・起動

コンテナをストップします。

//cmd{
$ docker stop cb
//}

実行中のコンテナを確認します。下記のコマンドにより、結果が表示されず、ブラウザでWEBコンソールにアクセスできないことも確認します。

//cmd{
$ docker ps
//}

実行されていないコンテナを確認するには、以下のようにオプションをつけて実行します。

//cmd{
$ docker ps -a
//}

コンテナを改めて開始してみます。

//cmd{
$ docker start cb
//}

WEBコンソールにアクセスできることを確認します。


==== コンテナ上のコマンド実行

コンテナに含まれる　@<tt>{cbbackup}　コマンドを実行する場合、以下のように行います。

 
//cmd{
$ docker exec -it cb /opt/couchbase/bin/cbbackup -h
//}

==== コンテナとイメージの削除

コンテナを削除する場合は、以下のコマンドを実行します（コンテナが起動している場合には、コマンド実行前にコンテナを停止をします）。

//cmd{
$ docker rm cb
//}

ダウンロードされたコンテナイメージを削除する場合は、以下のコマンドを実行します。

//cmd{
$ docker rmi couchbase:community
//}

=== クラスター初回構築

====[column]
クラスターの構築、構成変更には、CLIやREST APIを用いる方法もありますが、ここでは、WEB管理コンソール を使った手順について解説します。

====[/column]

==== WEB管理コンソールへのアクセス

@<href>{http://localhost:8091}にアクセスします。

インストール直後の状態では、下記の様な画面が表示されます。

//image[initial][initial]{
//}

==== 新規クラスターセットアップ

 1. 上記の画面に見える通り、インストール直後にWEBコンソールにアクセスした際の初めの選択肢は、「Setup New Cluster」と、「Join Existing Cluster」の二つです。ここでは、前者を選択します。
 2. 次の様な画面が表示されるので、適宜入力します。

//image[new_cluster][new cluster]{
//}

 3. 「Next: Accept Terms」を押下します。
 4. 適宜、チェックボックスをチェックします。

//image[terms_and_conditions][terms and conditions]{
//}

 5. 「Configure Disk, Memory, Services」を押下します。

@<strong>{クラスター設定}

//image[cluster_configure_top][cluster configure top]{
//}

//image[cluster_configure_bottom][cluster configure bottom]{
//}

 6. 不要なサービスのチェックボックスを外すことにより、このノードでは、そのサービスが無効になります。後から有効にすることはできませんが（その場合は、クラスターからの削除、再追加が必要）クラスター全体の構成を柔軟に設計することができます。上の画面の例では、Search（全文検索）を無効にしています。
 7. 「Save & Finish」を押下し、終了します。最終的にこの様な画面が表示されます。

//image[dashboard_initial][dashboard initial]{
//}

ここまでの手順を経て、１ノードで構成されるクラスターとしてCouchbase Serverが構築されたことになります。

=== クラスター構成変更

====[column]
クラスターの構築、構成変更には、CLIやREST APIを用いる方法もありますが、ここでは、WEB管理コンソール を使った手順について解説します。

====[/column]

既存のクラスターにノードを追加あるいは削除することによって、クラスターの構成を変更する手順について概要を記します。

==== ノードの追加

クラスターに新しくノードを追加する場合、まずその追加するノード（サーバー、VMインスタンス、あるいはコンテナ）に、Couchbase Serverがインストールされていることが前提となります。

その上で、ノードの追加には以下の２通りがあります。

 * クラスターの既存ノード（のWEB管理コンソール）から実行
 * クラスターへ追加するノード（のWEB管理コンソール）から実行

===== クラスターの既存ノードから実行

 1. サイドメニューから、「Servers」を選択します。
 2. トップバーから、「ADD Server」を選択します。
 3. 「ADD Server Node」ダイアログで、「Hostname/IP Address」などの入力項目を入力し、「ADD SERVER」を押下します。

===== クラスターへ追加するノードから実行

インストール直後の状態では、下記の様な画面が表示されます。

//image[initial][initial]{
//}

 1. 「Join Existing Cluster」を押下します。
 2. 「Cluster Host Name/IP Address」、「Cluster Admin Username」、「Cluster Admin Password」を適宜入力します。
 3. 必要に応じ、「Configure Services & Settings For This Node」を展開し、編集します（ここでは説明を割愛します）。
 4. 「Join With Default Configuration」を押下します。

クラスターがバケットを持っていない場合には問題ありませんが、既にそのクラスターにバケットがあり、バケットにデータが格納されている場合には、追加したノードを含めてデータを均等に分散する必要があります。そのためには、Servers画面右上の「Rebalance」ボタンを押下します。

==== ノードの削除

 1. サイドメニューから、「Servers」を選択します。
 2. 削除する対象のサーバーをクリックし、拡大します。
 3. 右下の「Remove」ボタンを押下します。
 

クラスターがバケットを持っていない場合には問題ありませんが、既にそのクラスターにバケットがあり、バケットにデータが格納されている場合には、ノードをクラスターから取り外した直後の状態では、アクティブコピーとレプリカコピーがそれぞれ一部分失われており適切な状態ではありません。この問題を解消するためにServers画面右上の「Rebalance」ボタンを押下します。

== 管理コンソールの使い方

=== バケット作成

 1. データの格納の単位であるBucketを作るため、「Bucket」リンクをクリックします。
 2. 表示されるのは、バケット一覧画面です。右上の「ADD BUCKET」リンクをクリックします。

//image[buckets_initial][buckets initial]{
//}

 3. ここでは、バケット名として、「MyFirstBucket」を入力しています。
 4. 「Advanced bucket settings」を展開し、ReplicasセクションののEnableチェックボックスを外します。（レプリカを有効にするには最低２ノード必要であるため、今回の手順で用いる１ノード構成のクラスターでは、チェックボックスを外さない場合は、バケット作成後、常時ワーニングが表示されてしまいます）
 5. 開発時の利便性のため、Flushを有効にしておきます。（Flushボタンが有効になり、このボタンを押下することで、バケットのデータをすべて削除することが可能になります。）

//image[add_bucket][add bucket]{
//}

 6. 「Add Bucket」ボタンを押下します。

=== ユーザー作成

 1. サイドメニューから、「Security」を選択します。
 2. 画面右上の「ADD USER」リンクを押下します。
 3. 「Add New User」ダイアログで、 入力欄(Username,Full Name,Password)に入力します。
 4. 画面右半分の「Roles」ツリー画面で、必要な権限を設定します。例えば、アプリケーションから利用するバケット名を開き、「Application Access」にチェックします。
 5. 「Add User」ボタンを押下します。

=== データ登録

==== 外部データファイル利用

既存のデータベースからの移行等の目的で、ファイルを介して、データをCouchbase Serverへ登録する場合があります。

ここでは、WEB管理コンソールからの外部ファイルを用いたデータ登録手順を示します。

 1. サイドメニューから、「Documents」を選択します。
 2. 上部メニューから、「Import Documents」を選択します。
 3. 左上部の「Select File to Import...」ボタンを押下し、開いたダイアログで、登録するデータを選択します。
 4. データが「File Contents」に表示されます。下記の通り、設定を行います。

 * Parse File As: ファイル形式（JSON, CSV）
 * Destination Bucket: データを登録するバケット名
 * Import With Document ID: ドキュメントのキーとして利用するフィールド(トグルボタンの「Value of Field」を選択の上、リストから選択)

 5. ここまでの入力した内容に従って、左上部画面の @<tt>{cbimport} コマンド実行用のパラメータが更新されます。この内容を使って、コマンドラインから同じ操作を実行することが可能です。
 6. 画面下の「Import Data」ボタンを押下します。

==== サンプルデータ利用

Couchbase Serverには、サンプルデータが同梱されており、これを使って、Couchbase Serverをどのように利用するか、理解を深めることができます。

 
 1. サイドメニューから、「Settings」を選択します。
 2. 上部メニューから、「Sample Buckets」を選択します。
 3. 「Available Samples」として示されているバケット名から、利用するデータを選択します。
 4. 「Load Sample Data」ボタンを押下します。ロードされたバケットの名前は、「Installed Samples」に移動されます。
 

=== クエリ実行

 1. サイドメニューから、「Query」を選択します。
 2. 上部メニューから、「Query Workbench」を選択します（デフォルトで表示されていない場合）。
 3. 左上部の「Query Editor」にN1QLを入力します。
 4. 「Execute」ボタンを押下します。

=== インデックス作成

==== 実行

 1. サイドメニューから、「Query」を選択します。
 2. 上部メニューから、「Query Workbench」を選択します（デフォルトで表示されていない場合）。
 3. 左上部の「Query Editor」に、N1QLを用いて、@<tt>{CREATE INDEX} DDLを入力します。
 4. 「Execute」ボタンを押下します。

==== 確認

 1. サイドメニューから、「Indexes」を選択します。
 2. 上記で作成したインデックスが表示され、それぞれ（列左端の）「status」が「ready」であることを確認します。

====[column]
N1QLクエリやインデックスの詳細については、別章を参照ください。

====[/column]

//footnote[install-intro][https://docs.couchbase.com/server/current/install/install-intro.html]