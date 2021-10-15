

= Couchbase Serverを使ってみる


== インストール

=== ダウンロード

Couchbase Serverのインストーラーは、下記のURLからダウンロードすることが可能です。

//blankline

@<href>{https://www.couchbase.com/downloads}

//blankline

ダウンロード時に、インストール先のOSとしてWindows、macOSに加え、様々なLinux系OSを選択可能です。

=== インストール手順

各OS毎のインストール手順は、ドキュメント@<fn>{install-intro}に詳述されています。

Linux系OSは、コマンドラインからパッケージシステムを介してインストールすることが可能です。

インストール以降のクラスター構築手順は、OSの別によらず、共通しています。

== Dockerコンテナ利用

=== 初回実行

下記のコマンドを実行します。 ここでは、@<tt>{--name} オプションとして @<tt>{cb}を指定しており、以降の操作でこの名称を利用します。

//cmd{
$ docker run -d --name cb -p 8091-8094:8091-8094 -p 11210:11210 couchbase:community
//}

CouchbaseのDockerリポジトリーから最新版がダウンロードされ、コンテナの実行が開始されます。

起動が成功すれば、ブラウザーで下記のURLにアクセスした際に、「Setup New Cluster」と「Join Existing Cluster」のいずれかを選択する画面が表示されます。

//blankline

@<href>{http://localhost:8091}


=== ログの確認

起動時のログを確認するには、以下のコマンドを実行します。

//cmd{
$ docker logs cb
Starting Couchbase Server -- Web UI available at http://<ip>:8091
and logs available in /opt/couchbase/var/lib/couchbase/logs
//}

=== イメージの確認

//cmd{
$ docker images
REPOSITORY                                                TAG               IMAGE ID       CREATED         SIZE
couchbase                                                 community         cb087be1f30a   7 weeks ago     1.01GB
//}

=== プロセスの確認

//cmd{
$ docker ps
CONTAINER ID   IMAGE                 COMMAND                  CREATED       STATUS         PORTS                                                                                                                                                                             NAMES
21fad80a0f9e   couchbase:community   "/entrypoint.sh couc…"   4 weeks ago   Up 2 seconds   8095-8096/tcp, 0.0.0.0:8091-8094->8091-8094/tcp, :::8091-8094->8091-8094/tcp, 11207/tcp, 0.0.0.0:11210-11211->11210-11211/tcp, :::11210-11211->11210-11211/tcp, 18091-18096/tcp   cb
//}

上記の手順により@<tt>{couchbase}レポジトリの@<tt>{community}というタグを持つ、Dockerイメージがインストールされ、そのイメージから @<tt>{cb}という名前のコンテナが作られ、稼働中の状態であることが確認されます。

=== コンテナの停止と再起動

コンテナをストップします。

//cmd{
$ docker stop cb
//}

ブラウザーでWebコンソールにアクセスできないことを確認します。

下記のコマンドを再度実行して、プロセス情報が表示されないことを確認します。

//cmd{
$ docker ps
//}

実行されていないコンテナの情報を確認するには、以下のようにオプションをつけて実行します。

//cmd{
$ docker ps -a
//}

コンテナを改めて開始してみます。

//cmd{
$ docker start cb
//}

Webコンソールにアクセスできることを確認します。


=== コンテナ上のコマンド実行

コンテナに含まれるコマンド(たとえばここでは@<tt>{cbbackupmgr}コマンド)を実行する場合、以下のように行います。

 
//cmd{
$ docker exec -it cb /opt/couchbase/bin/cbbackupmgr -h
//}

=== コンテナとイメージの削除

コンテナを削除する場合は、以下のコマンドを実行します（コンテナが起動している場合には、コマンド実行前にコンテナを停止します）。

//cmd{
$ docker rm cb
//}

ダウンロードされたコンテナイメージを削除する場合は、以下のコマンドを実行します。

//cmd{
$ docker rmi couchbase:community
//}

== クラスター初回構築

クラスターの構築にはCLIやREST APIを用いる方法もありますが、ここではWebコンソールを使った手順について解説します。

=== Webコンソールへのアクセス

@<href>{http://localhost:8091}にアクセスします。

インストール直後の状態では、以下のような画面が表示されます。

//image[initial][インストール直後の画面]{
//}

=== クラスターセットアップウィザード

 インストール直後の画面上の選択肢、「Setup New Cluster」と「Join Existing Cluster」のふたつから、「Setup New Cluster」を選択します。

 次の画面が表示されるので、適宜入力します。

//image[new_cluster][新規クラスター作成ダイアログ]{
//}

 「Next: Accept Terms」を押下します。

 適宜、チェックボックスをチェックします。

//image[terms_and_conditions][利用規約ダイアログ]{
//}

 「Configure Disk, Memory, Services」を押下します。

=== クラスター設定

クラスター設定ダイアログが表示されます。

サービス毎に割り当てるメモリーを、「Service Memory Quotas」として設定できることがわかります。

 また、不要なサービスのチェックボックスを外すことにより、このノードで、そのサービスが無効になります。
 後から有効にすることはできませんが（その場合は、クラスターから一旦削除し、再追加する際に、改めてサービスを選択します）、クラスター全体の構成を柔軟に設計することができます。
 
 ここでは、Search（全文検索）サービスを無効にしています。

//image[cluster_configure][クラスター設定ダイアログ]{
//}
 
 
 「Save & Finish」を押下します。
 
 ウィザードが終了し、Webコンソールのダッシュボード初期画面が表示されます。

//image[dashboard_initial][ダッシュボード初期画面]{
//}

ここまでの手順で、Couchbase Serverが１ノードで構成されるクラスターとして構築されました。


== Webコンソールの使い方

ここでは、Webコンソールの基本的な使い方を解説します。

=== バケット作成

まず、他の多くのデータベースにおける「データベース」に相当する、Couchbase Serverのデータ格納先であるバケットを作成します。

作成手順は、以下の通りです。

 1. サイドメニューから、「Buckets」を選択し、バケット一覧画面を表示します。
 2. 画面右上の「ADD BUCKET」リンクをクリックします。バケット追加ダイアログが表示されます。
 3. バケット名を入力します。
 4. 「Advanced bucket settings」を展開します。ここでは、「Replicas」セクションの「Enable」チェックボックスを外します(レプリカを有効にするには最低２ノード必要であるため、開発や検証のために１ノードでクラスターを構成する場合、チェックボックスを外さずにバケットを作成すると、Webコンソールに常時警告が表示されます)。また、開発時の利便性のため、「Flush」セクションの「Enable」チェックボックスにチェックし、フラッシュ機能を有効にします(フラッシュ機能を有効にすると、バケット一覧画面で「Flush」ボタンが表示され、このボタンを押下する操作で、バケットのデータをすべて削除することが可能になります)。
 6. 「Add Bucket」ボタンを押下します。


//image[add_bucket][バケット追加ダイアログ]{
//}

//image[buckets][バケット一覧画面]{
//}

=== スコープ作成

作成済みのバケットにスコープを作成する手順は、以下の通りです。

 1. サイドメニューから、「Buckets」を選択し、バケット一覧画面を表示します。
 2. スコープを追加する対象のバケットの右端の「Scopes & Collections」リンクをクリックします。スコープ一覧画面が表示されます。
 3. 画面右上の「ADD SCOPE」リンクをクリックします。スコープ追加ダイアログが表示されます。
 4. スコープ名を入力します。
 6. 「Save」ボタンを押下します。

//image[add_scope][スコープ追加ダイアログ]{
//}

=== コレクション作成

作成済みのスコープにコレクションを作成する手順は、以下の通りです。

 1. サイドメニューから、「Buckets」を選択し、バケット一覧画面を表示します。
 2. コレクションを追加する対象のバケットの右端の「Scopes & Collections」リンクをクリックします。スコープ一覧画面が表示されます。
 3. コレクションを追加する対象のスコープの右端の「Add Collection」リンクをクリックします。コレクション追加ダイアログが表示されます。
 4. コレクション名を入力します。
 6. 「Save」ボタンを押下します。

//image[add_collection][コレクション追加ダイアログ]{
//}

スコープとコレクションを作成した後の、スコープ一覧画面は、以下のように表示されます。

//image[scopes_collections][スコープ一覧画面]{
//}

=== ユーザー作成

クラスターにユーザーを追加する手順は、以下の通りです。

 1. サイドメニューから、「Security」を選択します。
 2. 画面右上の「ADD USER」リンクを押下します。ユーザー追加ダイアログが表示されます。
 3. 入力欄(Username,Full Name,Password)に入力します。
 4. 画面右半分の「Roles」ツリー画面で、必要な権限を設定します。たとえば、「Bucket」の下の「Application Access」で、アプリケーションから利用するバケットを探して、「ADD」ボタンを押下します。
 5. 「Add User」ボタンを押下します。

//image[add_user][ユーザー追加ダイアログ]{
//}

//image[users][ユーザー一覧画面]{
//}

=== ファイルからのデータ登録

既存のデータベースからの移行等の目的で、外部ファイルのデータをCouchbase Serverへ登録する手順は、以下の通りです。

外部ファイルを用いたデータ登録は、@<tt>{cbimport} コマンドを用いてコマンドラインから行うこともできます(詳しい操作方法をコマンドライン操作の章で説明しています。Webコンソールから実行する場合の各設定項目のオプションの意味についても、そちらを参照ください)。

 1. サイドメニューから、「Documents」を選択します。
 2. 上部メニューから、「Import」を選択します。
 3. 「Select File to Import...」ボタンを押下し、開いたダイアログで、登録するデータを選択します。
 4. データが「File Contents」に表示されます。ファイルの内容に基づいて、必要な設定を行います。ファイル形式（JSONまたはCSV）とデータを登録するキースペースを指定し、ドキュメントのIDの指定方法を選択します。
 5. ここまでの入力した内容に従って、@<tt>{cbimport} コマンド実行用のパラメータが更新されます。この内容を使って、コマンドラインから同じ操作を実行することも可能です。
 6. 画面下の「Import Data」ボタンを押下します。


//image[import-data][ファイルデータ登録画面]{
//}

=== サンプルデータ利用

Couchbase Serverには、サンプルデータが同梱されています。

サンプルデータを用いて新しいバケットを作成する手順は、以下の通りです。

 
 1. サイドメニューから、「Settings」を選択します。
 2. 上部メニューから、「Sample Buckets」を選択します。
 3. 「Available Samples」として示されているバケット名から、利用するデータを選択します。
 4. 「Load Sample Data」ボタンを押下します。ロードされたバケットの名前は、「Installed Samples」に移動されます。

//image[sample_buckets][サンプルバケット登録画面]{
//}
 

=== クエリ実行

Webコンソールのクエリワークベンチを用いて、クエリを実行する手順は、以下の通りです。

 1. サイドメニューから、「Query」を選択します。
 2. クエリワークベンチがデフォルトで表示されます。他の画面に遷移した後は、上部メニューから、「Query Workbench」を選択します。
 3. 「Query Editor」にN1QLを入力します。
 4. 「Execute」ボタンを押下してクエリを実行します。

//image[query_workbench][クエリワークベンチ画面]{
//}

=== インデックス作成・確認

Webコンソールのクエリワークベンチを用いてインデックスを作成する手順は、以下の通りです。

 1. サイドメニューから、「Query」を選択します。
 2. クエリワークベンチがデフォルトで表示されます。他の画面に遷移した後は、上部メニューから、「Query Workbench」を選択します。
 3. 「Query Editor」に、@<tt>{CREATE INDEX} DDL(データ定義言語)を入力します。
 4. 「Execute」ボタンを押下します。

作成されたインデックスを確認するには、以下の手順を踏みます。

 1. サイドメニューから、「Indexes」を選択します。
 2. 「Bucket & Scope」セクションで、キースペースを選びます。
 3. 選択したキースペースに作成済みのインデックスが表示されます。インデックス列の右端の「status」が「ready」になっていれば、インデックスが利用可能な状態です。

//image[indexes][インデックス一覧画面]{
//}

//footnote[install-intro][https://docs.couchbase.com/server/current/install/install-intro.html]

== クラスター構成変更

ここまで、1ノードからなるクラスターを構築し、Webコンソールでの操作方法を見てきました。
本書のサンプルアプリケーションを動かすために、複数ノードからなるクラスターを用いる必要もなければ、N1QLクエリなどの開発に必要な知識の理解において、基本的にノード数の違いを意識するところはありません。

とはいえ、クラスターは複数ノードで構成されることが前提としてあり、１ノードでの構成はそのひとつの形態にすぎないのも確かです。
そこで、ここでは既存のクラスターに対してノードの追加あるいは削除を行い、クラスターの構成を変更する手順について、概要を記します。

クラスターの構成変更にはCLIやREST APIを用いる方法もありますが、ここではWebコンソールを使った手順を用いて、クラスター構成変更のプロセスを解説します。

=== ノードの追加

クラスターに新しくノードを追加する場合、まずその追加するノード（サーバー、VMインスタンス、あるいはコンテナ）に、Couchbase Serverがインストールされていることが前提となります。

その上で、ノードの追加方法には以下の２通りがあります。

 * クラスターの既存ノード（のWebコンソール）から実行
 * クラスターへ追加するノード（のWebコンソール）から実行

クラスターの既存ノードから実行する場合、以下の手順になります。

 1. サイドメニューから、「Servers」を選択します。
 2. 画面右上の「ADD SERVER」リンクを押下します。
 3. 「ADD Server Node」ダイアログで、「Hostname/IP Address」などの入力項目を入力し、「Add Server」ボタンを押下します。

クラスターへ追加するノードから実行する場合、以下の手順になります。


 1. インストール直後のトップ画面の選択肢から、「Join Existing Cluster」を選択します。
 2. 「Cluster Host Name/IP Address」、「Cluster Admin Username」、「Cluster Admin Password」を適宜入力します。
 3. 必要に応じ、「Configure Services & Settings For This Node」を展開し、編集します。
 4. 「Join With Default Configuration」ボタンを押下します。

既にそのクラスターにバケットが作成されている状況を考えると、追加したノードを含めてバケットを均等に分散する必要があることがわかります。
この処理を実行するために、クラスターのリバランスを実施します。

リバランス実施前は、そのノードは「New Node」としてマークされており、まだそのノードではトラフィックが発生していません(この時点では「Cancel Add」を押下して、追加をキャンセルすることができます)。
リバランスを実施するには、Servers画面右上に表示される「Rebalance」ボタンを押下します。
リバランスが完了してはじめて、追加されたノードは実際にクラスターのメンバーになります。

=== ノードの削除

 1. サイドメニューから、「Servers」を選択します。
 2. 削除する対象のサーバーをクリックします。
 3. 右下の「Remove」ボタンを押下します。
 

既にそのクラスターにバケットが作成されている状況を考えると、単にノードをクラスターから取り外してしまうと、バケットが一部分失われることになり、適切ではありません。
この問題を解消するために、クラスターのリバランスを実施します。

リバランス実施前は、そのノードは「Node flagged for removal」としてマークされており、まだそのノードでトラフィックが発生し続けています(この時点では、「Cancel Remove」を押下して、削除をキャンセルすることができます)。
リバランスを実施するには、Servers画面右上に表示される「Rebalance」ボタンを押下します。
リバランスが完了してはじめて、削除されたノードは、実際にクラスターのメンバーではなくなります。
